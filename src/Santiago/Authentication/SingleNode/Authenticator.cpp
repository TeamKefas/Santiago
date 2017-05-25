#include <boost/date_time/posix_time/posix_time.hpp>

#include "Authenticator.h"

namespace Santiago{ namespace Authentication{ namespace SingleNode
{

    Authenticator::Authenticator(ThreadSpecificDbConnection& databaseConnection_,
                                 boost::asio::io_service& ioService_,
                                 const boost::property_tree::ptree& config_)
        :Authentication::AuthenticatorBase(ioService_,config_)
        ,_databaseConnection(databaseConnection_)
    {
        srand ( time(NULL) );
        std::error_code error;
        std::vector<SantiagoDBTables::SessionsRec> activeSessionsRec =
            _databaseConnection.get().getActiveSessions(error);
        if(error && (ErrorCode::ERR_DATABASE_GET_RETURNED_ZERO_RESULTS != error.value()))
        {
            ST_LOG_ERROR("Authenticator contruction failed. Error message:"<<
                         error.message());
            throw std::runtime_error("Authenticator contruction failed. Error message:" +
                                     error.message());
        }
        
        for(std::vector<SantiagoDBTables::SessionsRec>::iterator it = activeSessionsRec.begin();
            it != activeSessionsRec.end();++it)
        {
            _cookieStringSessionsRecMap.insert(std::make_pair(it->_cookieString,*it));
            boost::optional<SantiagoDBTables::UsersRec> usersRecOpt =
                _databaseConnection.get().getUsersRecForUserName(it->_userName,error);
            if(error)
            {
                ST_LOG_ERROR("Authenticator contruction failed. Error message:"<<
                             error.message());
                throw std::runtime_error("Authenticator contruction failed. Error message:" +
                                         error.message());
            }
            
            ST_ASSERT(usersRecOpt);
            // _userNameUserDataMap.insert(std::make_pair(it->_userName,UserData(userRec->_emailAddress)));
            std::map<std::string,UserData>::iterator userNameUserDataMapIter =
                _userNameUserDataMap.find(usersRecOpt->_userName);
            if(_userNameUserDataMap.end() == userNameUserDataMapIter)
            {
                bool isInsertionSuccessfulFlag;
                std::tie(userNameUserDataMapIter,isInsertionSuccessfulFlag) =
                    _userNameUserDataMap.insert(std::make_pair(usersRecOpt->_userName,
                                                               UserData(usersRecOpt->_emailAddress)));
                ST_ASSERT(isInsertionSuccessfulFlag);
                ST_ASSERT(userNameUserDataMapIter != _userNameUserDataMap.end());
            }    
            userNameUserDataMapIter->second._cookieList.push_back(it->_cookieString);
            
            if(boost::posix_time::second_clock::universal_time() - it->_lastActiveTime >=
               boost::posix_time::time_duration(MAX_SESSION_DURATION,0,0,0))
            {
                cleanupCookieDataAndUpdateSessionRecord(it->_cookieString);
                /* it->_logoutTime = boost::posix_time::second_clock::universal_time();
                   _databaseConnection.get().updateSessionsRec(*it,error);*/
            }
        }
    }

    std::string Authenticator::generateSHA256(const std::string str)
    {
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, str.c_str(), str.size());
        SHA256_Final(hash, &sha256);
        std::stringstream ss;
        for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        {
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
        }
        return ss.str();
    }
     
    void Authenticator::createUserImpl(const std::string& userName_,
                                       const std::string& emailAddress_,
                                       const std::string& password_,
                                       const ErrorCodeCallbackFn& onCreateUserCallbackFn_)
    {

        boost::optional<SantiagoDBTables::UsersRec> usersRecOpt;
        std::error_code error;

        //check for existing accounts with same userName
        std::tie(error,usersRecOpt) = verifyUserNamePasswordAndGetUsersRec(userName_,"");
        if((ErrorCode::ERR_DATABASE_EXCEPTION == error.value()) ||
           (ErrorCode::ERR_DATABASE_INVALID_USER_INPUT == error.value()))
        {
            onCreateUserCallbackFn_(error);
            return;
        }
        else if(usersRecOpt)
        {
            onCreateUserCallbackFn_(std::error_code(ErrorCode::ERR_USERNAME_ALREADY_EXISTS,
                                                    ErrorCategory::GetInstance()));
            return;
        }

        //check for existing accounts with same emailAddress
        std::tie(error,usersRecOpt) = verifyEmailAddressPasswordAndGetUsersRec(emailAddress_,"");
        if((ErrorCode::ERR_DATABASE_EXCEPTION == error.value()) ||
           (ErrorCode::ERR_DATABASE_INVALID_USER_INPUT == error.value()))
        {
            onCreateUserCallbackFn_(error);
            return;
        }
        else if(usersRecOpt)
        {
            onCreateUserCallbackFn_(std::error_code(ErrorCode::ERR_EMAIL_ADDRESS_ALREADY_EXISTS,
                                                    ErrorCategory::GetInstance()));
            return;
        }

        //create new usersRec and add to db
        SantiagoDBTables::UsersRec usersRec;
        usersRec._userName = userName_;
        usersRec._emailAddress = emailAddress_;
        usersRec._password = generateSHA256(password_);
        _databaseConnection.get().addUsersRec(usersRec,error);
        if(error)
        {
            onCreateUserCallbackFn_(error);
            return;
        }

        ST_LOG_INFO("Create user successfull for userName:"<<userName_<<std::endl);
        onCreateUserCallbackFn_(std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance()));
        return;
    }

    void Authenticator::loginUserImpl(const std::string& userNameOrEmailAddress_,
                                      bool isUserNameNotEmailAddress_,
                                      const std::string& password_,
                                      const ErrorCodeUserInfoStringPairCallbackFn& onLoginUserCallbackFn_)
    {
        //verify username-password
        boost::optional<SantiagoDBTables::UsersRec> usersRecOpt;
        std::error_code error;

        if(isUserNameNotEmailAddress_)
        {
            std::tie(error,usersRecOpt) =
                verifyUserNamePasswordAndGetUsersRec(userNameOrEmailAddress_,
                                                     password_);
        }
        else
        {
            std::tie(error,usersRecOpt) =
                verifyEmailAddressPasswordAndGetUsersRec(userNameOrEmailAddress_,
                                                         password_);
        }

        if(error)
        {
            onLoginUserCallbackFn_(error,boost::none);
            return;
        }
        ST_ASSERT(usersRecOpt);


        //create new session record and add to db
        SantiagoDBTables::SessionsRec sessionsRec;
        sessionsRec._userName = usersRecOpt->_userName;
        sessionsRec._loginTime = boost::posix_time::second_clock::universal_time();
        sessionsRec._lastActiveTime = sessionsRec._loginTime;

        for(unsigned i=0;i<5;i++)
        { //5 attempts with different cookie strings
            sessionsRec._cookieString = generateUniqueCookie();
            _databaseConnection.get().addSessionsRec(sessionsRec,error);
            if(!error)
            {
                break;
            }
        }

        if(error)
        {
            onLoginUserCallbackFn_(error,boost::none);
            return;
        }
        
        //update the _cookieStringSessionsRecMap;
        bool isInsertionSuccessfulFlag =
            _cookieStringSessionsRecMap.insert(std::make_pair(sessionsRec._cookieString,sessionsRec)).second;
        ST_ASSERT(isInsertionSuccessfulFlag);
        
        //update the _userNameUserSessionsInfoMap;
        std::map<std::string,UserData>::iterator userNameUserDataMapIter =
            _userNameUserDataMap.find(usersRecOpt->_userName);
        if(_userNameUserDataMap.end() == userNameUserDataMapIter)
        {
            std::tie(userNameUserDataMapIter,isInsertionSuccessfulFlag) =
                _userNameUserDataMap.insert(std::make_pair(usersRecOpt->_userName,
                                                           UserData(usersRecOpt->_emailAddress)));
            ST_ASSERT(isInsertionSuccessfulFlag);
            ST_ASSERT(userNameUserDataMapIter != _userNameUserDataMap.end());
        }
        userNameUserDataMapIter->second._cookieList.push_back(sessionsRec._cookieString);


        onLoginUserCallbackFn_(std::error_code(ErrorCode::ERR_SUCCESS,
                                               ErrorCategory::GetInstance()),
                               std::make_pair(UserInfo(usersRecOpt->_userName,
                                                       usersRecOpt->_emailAddress),
                                              sessionsRec._cookieString));
        return;
    }

    void Authenticator::verifyCookieAndGetUserInfoImpl(const std::string& cookieString_,
                                                    const ErrorCodeUserInfoCallbackFn& onVerifyUserCallbackFn_)
    {
        std::map<std::string,SantiagoDBTables::SessionsRec>::iterator cookieStringSessionsRecMapIter;
        std::error_code error;
        std::tie(error,cookieStringSessionsRecMapIter) = checkForCookieInMapAndGetSessionsRecIter(cookieString_);
        if(error)
        {
            onVerifyUserCallbackFn_(error,boost::none);
            return;
        }

        //check if the lastActiveTime older than MAX_SESSION_DURATION. If yes then logout
        if((boost::posix_time::second_clock::universal_time() -
            cookieStringSessionsRecMapIter->second._lastActiveTime) >
           boost::posix_time::time_duration(MAX_SESSION_DURATION,0,0,0))
        {

            ST_LOG_INFO("Session lastActiveTime older than MAX_SESSION_DURATION. Going to log out. cookieString:"
                     <<cookieString_<<std::endl);            
            cleanupCookieDataAndUpdateSessionRecord(cookieString_);

            onVerifyUserCallbackFn_(std::error_code(ErrorCode::ERR_INVALID_SESSION_COOKIE,
                                                    ErrorCategory::GetInstance()),
                                    boost::none);
            return;
        }
        else //set the lastActiveTime to now
        {
            cookieStringSessionsRecMapIter->second._lastActiveTime = boost::posix_time::second_clock::universal_time();
//            std::error_code error; //calling db for every verify is very inefficient. So commenting out
//            _databaseConnection.get().updateSessionsRec(*cookieStringSessionsRecMapIter->second,error_);
        }

        ST_LOG_INFO("Verify cookie successfull. cookieString:"<<cookieString_<<std::endl);

        //update the _userNameUserSessionsInfoMap;
        std::map<std::string,UserData>::iterator userNameUserDataMapIter =
            _userNameUserDataMap.find(cookieStringSessionsRecMapIter->second._userName);
        ST_ASSERT(_userNameUserDataMap.end() != userNameUserDataMapIter);


        onVerifyUserCallbackFn_(std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance()),
                                UserInfo(cookieStringSessionsRecMapIter->second._userName,
                                         userNameUserDataMapIter->second._emailAddress));
        return;
    }

    void Authenticator::logoutUserForCookieImpl(const std::string& cookieString_,
                                             const ErrorCodeCallbackFn& onLogoutCookieCallbackFn_)
    {
        std::error_code error;
        std::tie(error,std::ignore) = checkForCookieInMapAndGetSessionsRecIter(cookieString_);
        if(error)
        {
            onLogoutCookieCallbackFn_(error);
            return;
        }

        cleanupCookieDataAndUpdateSessionRecord(cookieString_);

        onLogoutCookieCallbackFn_(std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance()));
        return;        
    }

    void Authenticator::logoutUserForAllCookiesImpl(const std::string& userName_,
                                                    const ErrorCodeCallbackFn& onLogoutAllCookiesCallbackFn_)
    {
        //verify if such a user is already logged in
        std::map<std::string,UserData >::iterator userNameUserDataMapIter = _userNameUserDataMap.find(userName_);
        if(userNameUserDataMapIter == _userNameUserDataMap.end())
        {
            onLogoutAllCookiesCallbackFn_(std::error_code(ErrorCode::ERR_NO_ACTIVE_SESSION_FOR_USERNAME,
                                                          ErrorCategory::GetInstance()));
            return;
        }
        
        cleanupCookieDataAndUpdateSessionRecordsForAllCookies(userName_);
        onLogoutAllCookiesCallbackFn_(std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance()));
        return;
    }

    void Authenticator::changeUserPasswordImpl(const std::string& cookieString_,
                                               const std::string& oldPassword_,
                                               const std::string& newPassword_,
                                               const ErrorCodeCallbackFn& onChangePasswordCallbackFn_)
    {
        std::map<std::string,SantiagoDBTables::SessionsRec>::iterator cookieStringSessionsRecMapIter;
        std::error_code error;
        std::tie(error,cookieStringSessionsRecMapIter) = checkForCookieInMapAndGetSessionsRecIter(cookieString_);
        if(error)
        {
            onChangePasswordCallbackFn_(error);
            return;
        }

        std::string userName = cookieStringSessionsRecMapIter->second._userName;
        //verify username-password
        boost::optional<SantiagoDBTables::UsersRec> usersRecOpt;
        std::tie(error,usersRecOpt) =
            verifyUserNamePasswordAndGetUsersRec(cookieStringSessionsRecMapIter->second._userName,generateSHA256(oldPassword_));
        if(error)
        {
            onChangePasswordCallbackFn_(error);
            return;
        }
        ST_ASSERT(usersRecOpt);
            
        //change and update the password
        usersRecOpt->_password = generateSHA256(newPassword_);
        _databaseConnection.get().updateUsersRec(*usersRecOpt,error);
        //whether succeed or db error...it will be passed to the onChangePasswordCallbackFn
        onChangePasswordCallbackFn_(error);
        return;
    }
    
     void Authenticator::createAndReturnRecoveryStringImpl(const std::string& emailAddress_,
                                                           const ErrorCodeStringCallbackFn& onCreateAndReturnRecoveryStringCallbackFn_)
    {
        boost::optional<SantiagoDBTables::UsersRec> usersRecOpt;
        std::error_code error;
        usersRecOpt = _databaseConnection.get().getUsersRecForEmailAddress(emailAddress_,error);
        if(error && (ErrorCode::ERR_DATABASE_GET_RETURNED_ZERO_RESULTS != error.value()))
        {
            
            onCreateAndReturnRecoveryStringCallbackFn_(error,boost::none);
            return;
        }
        if(ErrorCode::ERR_DATABASE_GET_RETURNED_ZERO_RESULTS == error.value())
        {
            onCreateAndReturnRecoveryStringCallbackFn_(std::error_code(ErrorCode::ERR_EMAIL_ADDRESS_NOT_REGISTERED,
                                                                       ErrorCategory::GetInstance()),
                                                       boost::none);
            return;
        }
            
        ST_ASSERT(usersRecOpt);
        
        SantiagoDBTables::UsersRec newUsersRec = *usersRecOpt;
        newUsersRec._recoveryString = generateRecoveryString();
        newUsersRec._recoveryStringCreateTime = boost::posix_time::second_clock::universal_time();
        _databaseConnection.get().updateUsersRec(newUsersRec,error);
        if(error)
        {     
            onCreateAndReturnRecoveryStringCallbackFn_(error,boost::none);
            return;
        }
        ST_LOG_INFO("Created recovery string successfully for emailAddress:"<<emailAddress_<<std::endl);
        onCreateAndReturnRecoveryStringCallbackFn_(std::error_code(ErrorCode::ERR_SUCCESS,
                                                                   ErrorCategory::GetInstance()),
                                                   newUsersRec._recoveryString);
        return;
    }

     void Authenticator::getUserForEmailAddressAndRecoveryStringImpl(const std::string& emailAddress_,
                                                                     const std::string& recoveryString_,
                                                                     const ErrorCodeStringCallbackFn& onGetUserForEmailAddressAndRecoveryStringCallbackFn_)
     {
          boost::optional<SantiagoDBTables::UsersRec> usersRecOpt;
          std::error_code error;
          usersRecOpt = _databaseConnection.get().getUsersRecForEmailAddress(emailAddress_,error);
          if(error && (ErrorCode::ERR_DATABASE_GET_RETURNED_ZERO_RESULTS != error.value()))
          {
              onGetUserForEmailAddressAndRecoveryStringCallbackFn_(error,
                                                                   boost::none);
              return;
          }
          if(ErrorCode::ERR_DATABASE_GET_RETURNED_ZERO_RESULTS == error.value())
          {
              onGetUserForEmailAddressAndRecoveryStringCallbackFn_(std::error_code(ErrorCode::ERR_EMAIL_ADDRESS_NOT_REGISTERED,
                                                                                   ErrorCategory::GetInstance()),
                                                                   boost::none);
              return;
          }
          ST_ASSERT(usersRecOpt);
          
          if(usersRecOpt->_recoveryString != recoveryString_)
          {
              onGetUserForEmailAddressAndRecoveryStringCallbackFn_(std::error_code(ErrorCode::ERR_INVALID_EMAIL_ADDRESS_RECOVERY_STRING,
                                                                                   ErrorCategory::GetInstance()),
                                                                   boost::none);
              return;
          }

          onGetUserForEmailAddressAndRecoveryStringCallbackFn_(std::error_code(ErrorCode::ERR_SUCCESS,
                                                                               ErrorCategory::GetInstance()),
                                                               usersRecOpt->_userName);
          return;
     }

    void Authenticator::changeUserPasswordForEmailAddressAndRecoveryStringImpl(const std::string& emailAddress_,
                                                                               const std::string& recoveryString_,
                                                                               const std::string& newPassword_,
                                                                               const ErrorCodeCallbackFn& onChangePasswordForEmailAddressAndRecoveryStringCallbackFn_)
    {
        boost::optional<SantiagoDBTables::UsersRec> usersRecOpt;
        std::error_code error;
        std::tie(error,usersRecOpt) =
            verifyEmailAddressRecoveryStringAndGetUsersRec(emailAddress_,recoveryString_);
        if(error)
        {
            onChangePasswordForEmailAddressAndRecoveryStringCallbackFn_(error);
            return;
        }
        ST_ASSERT(usersRecOpt);
            
        //change and update the password
        usersRecOpt->_password = generateSHA256(newPassword_);
        usersRecOpt->_recoveryString = boost::none;
        usersRecOpt->_recoveryStringCreateTime = boost::none;
        _databaseConnection.get().updateUsersRec(*usersRecOpt,error);
        //whether succeed or db error...it will be passed to the onChangePasswordCallbackFn
        onChangePasswordForEmailAddressAndRecoveryStringCallbackFn_(error);
        return;
    }

    void Authenticator::changeUserEmailAddressImpl(const std::string& cookieString_,
                                                const std::string& newEmailAddress_,
                                                const std::string& password_,
                                                const ErrorCodeCallbackFn& onChangeEmailAddressCallbackFn_)
    {
        std::map<std::string,SantiagoDBTables::SessionsRec>::iterator cookieStringSessionsRecMapIter;
        std::error_code error;
        std::tie(error,cookieStringSessionsRecMapIter) = checkForCookieInMapAndGetSessionsRecIter(cookieString_);
        if(error)
        {
            onChangeEmailAddressCallbackFn_(error);
            return;
        }

        std::string userName = cookieStringSessionsRecMapIter->second._userName;
        //verify password
        boost::optional<SantiagoDBTables::UsersRec> usersRecOpt;
        std::tie(error,usersRecOpt) =
            verifyUserNamePasswordAndGetUsersRec(cookieStringSessionsRecMapIter->second._userName,generateSHA256(password_));
        if(error)
        {
            onChangeEmailAddressCallbackFn_(error);
            return;
        }
        ST_ASSERT(usersRecOpt);

        SantiagoDBTables::UsersRec newUsersRec = *usersRecOpt;

        //verify no other existing user with same email address
        usersRecOpt = _databaseConnection.get().getUsersRecForEmailAddress(newEmailAddress_,error);
        if(error && (ErrorCode::ERR_DATABASE_GET_RETURNED_ZERO_RESULTS != error.value()))
        {
            onChangeEmailAddressCallbackFn_(error);
            return;
        }        
        if(usersRecOpt)
        {
            onChangeEmailAddressCallbackFn_(std::error_code(ErrorCode::ERR_EMAIL_ADDRESS_ALREADY_EXISTS,
                                                            ErrorCategory::GetInstance()));
            return;
        }
        ST_ASSERT(ErrorCode::ERR_DATABASE_GET_RETURNED_ZERO_RESULTS == error.value());

        //change and update the password
        newUsersRec._emailAddress = newEmailAddress_;
        _databaseConnection.get().updateUsersRec(newUsersRec,error);
        //whether succeed or db error...it will be passed to the onChangePasswordCallbackFn
        onChangeEmailAddressCallbackFn_(error);
        return;

    }

    void Authenticator::deleteUserImpl(const std::string& cookieString_,
                                    const ErrorCodeCallbackFn& onDeleteUserCallbackFn_)
    {
        //verify if the cookie is in the cookieStringSessionsRecMap.
        std::map<std::string,SantiagoDBTables::SessionsRec>::iterator cookieStringSessionsRecMapIter;
        std::error_code error;
        std::tie(error,cookieStringSessionsRecMapIter) = checkForCookieInMapAndGetSessionsRecIter(cookieString_);
        if(error)
        {
            onDeleteUserCallbackFn_(error);
            return;
        }

        //delete from db
        boost::optional<SantiagoDBTables::UsersRec> usersRecOpt;
        std::tie(error,usersRecOpt) =
            verifyUserNamePasswordAndGetUsersRec(cookieStringSessionsRecMapIter->second._userName,"");
        if((ErrorCode::ERR_DATABASE_EXCEPTION == error.value()) ||
           (ErrorCode::ERR_DATABASE_INVALID_USER_INPUT == error.value()))
        {
            onDeleteUserCallbackFn_(error);
            return;
        }

        ST_ASSERT(usersRecOpt);
        _databaseConnection.get().deleteUsersRec(usersRecOpt->_userName,error);
        if(error)
        {
            onDeleteUserCallbackFn_(error);
            return;
        }

        //remove from memory
        cleanupCookieDataAndUpdateSessionRecordsForAllCookies(cookieStringSessionsRecMapIter->second._userName);
        onDeleteUserCallbackFn_(std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance()));
        return;
    }


    std::pair<std::error_code,boost::optional<SantiagoDBTables::UsersRec> > 
    Authenticator::verifyUserNamePasswordAndGetUsersRec(const std::string& userName_, const std::string& password_)
    {
        //get the UsersRec from db
        std::error_code error;
        boost::optional<SantiagoDBTables::UsersRec> usersRecOpt = 
            _databaseConnection.get().getUsersRecForUserName(userName_,error);
        if(error && (ErrorCode::ERR_DATABASE_GET_RETURNED_ZERO_RESULTS != error.value()))
        {
            return std::make_pair(error,usersRecOpt);
        }

        //check if the username/password matches
        if(!usersRecOpt || (usersRecOpt->_password != generateSHA256(password_)))
        {
            ST_LOG_INFO("Wrong username_password. userName:"<<userName_<<std::endl);
            return std::make_pair(std::error_code(ErrorCode::ERR_INVALID_USERNAME_PASSWORD,
                                                  ErrorCategory::GetInstance()),
                                  usersRecOpt);
        }
        ST_ASSERT(!error);

        ST_LOG_INFO("Username password verified for userName:"<<userName_<<std::endl);
        return std::make_pair(std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance()),usersRecOpt);
    }

    std::pair<std::error_code,boost::optional<SantiagoDBTables::UsersRec> > 
    Authenticator::verifyEmailAddressPasswordAndGetUsersRec(const std::string& emailAddress_,
                                                            const std::string& password_)
    {
        //get the UsersRec from db
        std::error_code error;
        boost::optional<SantiagoDBTables::UsersRec> usersRecOpt = 
            _databaseConnection.get().getUsersRecForEmailAddress(emailAddress_,error);
        if(error && (ErrorCode::ERR_DATABASE_GET_RETURNED_ZERO_RESULTS != error.value()))
        {
            return std::make_pair(error,usersRecOpt);
        }
        
        //check if the username/password matches
        if(!usersRecOpt || (usersRecOpt->_password != generateSHA256(password_)))
        {
            ST_LOG_INFO("Wrong emailaddress_password. emailAddress:"<<emailAddress_<<std::endl);
            return std::make_pair(std::error_code(ErrorCode::ERR_INVALID_EMAIL_ADDRESS_PASSWORD,
                                                  ErrorCategory::GetInstance()),
                                  usersRecOpt);
        }
        ST_ASSERT(!error);
        
        ST_LOG_INFO("EmailAddress password verified for emailAddress:"<<emailAddress_<<std::endl);
        return std::make_pair(std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance()),usersRecOpt);
    }

    std::pair<std::error_code,boost::optional<SantiagoDBTables::UsersRec> > 
    Authenticator::verifyEmailAddressRecoveryStringAndGetUsersRec(const std::string& emailAddress_,
                                                                  const std::string& recoveryString_)
    {
        //get the UsersRec from db
        std::error_code error;
        boost::optional<SantiagoDBTables::UsersRec> usersRecOpt = 
            _databaseConnection.get().getUsersRecForEmailAddress(emailAddress_,error);
        if(error && (ErrorCode::ERR_DATABASE_GET_RETURNED_ZERO_RESULTS != error.value()))
        {
            return std::make_pair(error,usersRecOpt);
        }

        //check if the username/password matches
        if(!usersRecOpt || (usersRecOpt->_recoveryString != recoveryString_))
        {
            ST_LOG_INFO("Wrong emailaddress_recoverystring. emailAddress:"<<emailAddress_<<std::endl);
            return std::make_pair(std::error_code(ErrorCode::ERR_INVALID_EMAIL_ADDRESS_RECOVERY_STRING,
                                                  ErrorCategory::GetInstance()),
                                  usersRecOpt);
        }
        ST_ASSERT(!error);

        ST_LOG_INFO("EmailAddress recovery string verified for emailAddress:"<<emailAddress_<<std::endl);
        return std::make_pair(std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance()),usersRecOpt);
    }


    std::pair<std::error_code,std::map<std::string,Santiago::SantiagoDBTables::SessionsRec>::iterator > 
    Authenticator::checkForCookieInMapAndGetSessionsRecIter(const std::string& cookieString_)
    {
        std::map<std::string,SantiagoDBTables::SessionsRec>::iterator cookieStringSessionsRecMapIter =
            _cookieStringSessionsRecMap.find(cookieString_);
        if(cookieStringSessionsRecMapIter == _cookieStringSessionsRecMap.end())
        {
            ST_LOG_INFO("Cookie not in _cookieStringSessionsRecMap. cookieString:" <<cookieString_<<std::endl);
            return std::make_pair(std::error_code(ErrorCode::ERR_INVALID_SESSION_COOKIE,ErrorCategory::GetInstance()),
                                  _cookieStringSessionsRecMap.end());
        }

        return std::make_pair(std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance()),
                              cookieStringSessionsRecMapIter);
    }
    
    void Authenticator::cleanupCookieDataAndUpdateSessionRecord(const std::string& cookieString_)
    {
        //internal fn. so when this fn is called the cookie should already be verified.
        std::map<std::string,SantiagoDBTables::SessionsRec>::iterator cookieStringSessionsRecMapIter =
            _cookieStringSessionsRecMap.find(cookieString_);
        ST_ASSERT(cookieStringSessionsRecMapIter != _cookieStringSessionsRecMap.end());

        //update the db
        cookieStringSessionsRecMapIter->second._logoutTime = boost::posix_time::second_clock::universal_time();
        std::error_code error;
        _databaseConnection.get().updateSessionsRec(cookieStringSessionsRecMapIter->second,error);
        if(error)
        {
            ST_LOG_INFO("updateSessionsRec failed. Logging out without writing to db. CookieString:"
                     <<std::endl << cookieStringSessionsRecMapIter->first <<std::endl);
        }

        //remove from _userNameUserDataMap
        std::map<std::string,UserData>::iterator userNameUserDataMapIter =
            _userNameUserDataMap.find(cookieStringSessionsRecMapIter->second._userName);
        ST_ASSERT(userNameUserDataMapIter != _userNameUserDataMap.end());
        std::vector<std::string>::iterator cookieListIter =
            std::find(userNameUserDataMapIter->second._cookieList.begin(),
                      userNameUserDataMapIter->second._cookieList.end(),
                      cookieString_);
        ST_ASSERT(cookieListIter != userNameUserDataMapIter->second._cookieList.end());
        userNameUserDataMapIter->second._cookieList.erase(cookieListIter);
        //if all cookie are removed for a user, remove the use from the userNameUserDataMap
        if(0 == userNameUserDataMapIter->second._cookieList.size())
        {
            _userNameUserDataMap.erase(userNameUserDataMapIter);
        }
        
        //remove from _cookieStringSessionsRecMap
        _cookieStringSessionsRecMap.erase(cookieStringSessionsRecMapIter);

        return;
    }

    void Authenticator::cleanupCookieDataAndUpdateSessionRecordsForAllCookies(const std::string& userName_)
    {
        //internal fn. the username should exist.
        std::map<std::string,UserData >::iterator userNameUserDataMapIter =
            _userNameUserDataMap.find(userName_);
        ST_ASSERT(userNameUserDataMapIter != _userNameUserDataMap.end());
        
        while(userNameUserDataMapIter->second._cookieList.size() > 1)
        {
            cleanupCookieDataAndUpdateSessionRecord(userNameUserDataMapIter->second._cookieList[0]);
        }
        //now there is 1 cookie remaining in the cookieList vector
        ST_ASSERT(userNameUserDataMapIter->second._cookieList.size() == 1);
        cleanupCookieDataAndUpdateSessionRecord(userNameUserDataMapIter->second._cookieList[0]);
        //ST_ASSERT(userNameUserDataMapIter == _userNameUserDataMap.end());

        return;
    }

    std::string Authenticator::generateUniqueCookie()
    {
        std::string str;
        static const char alphanum[] =
            "0123456789"
            "@#$%^*"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
        
        int stringLength = sizeof(alphanum) - 1;
	
        //return alphanum[rand() % stringLength];
	for(unsigned int i = 0; i < 46; ++i)
	{
            str += alphanum[rand() % stringLength];
	}
        return str;
    }

    std::string Authenticator::generateRecoveryString()
    {
        std::string str;
        static const char alphanum[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
                    
        int stringLength = sizeof(alphanum) - 1;
	
        //return alphanum[rand() % stringLength];
	for(unsigned int i = 0; i < 8; ++i)
	{
            str += alphanum[rand() % stringLength];
	}
        return str;
    }
        
    }}}
