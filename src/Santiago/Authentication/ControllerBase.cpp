#include <boost/date_time/posix_time/posix_time.hpp>

#include "ControllerBase.h"

namespace Santiago{ namespace Authentication
{
    template<typename ClientIdType>
    ControllerBase<ClientIdType>::ControllerBase(ThreadSpecificDbConnection& databaseConnection_,
                                                 boost::asio::io_service& ioService_,
                                                 const boost::property_tree::ptree& config_):
        _databaseConnection(databaseConnection_),
        _localData()
    {
        srand ( time(NULL) );
        std::error_code error;
        std::vector<SantiagoDBTables::SessionsRec> activeSessionsRec =
            _databaseConnection.get().getActiveSessions(error);
        if(error && (ErrorCode::ERR_DATABASE_GET_RETURNED_ZERO_RESULTS != error.value()))
        {
            ST_LOG_ERROR("Authenticator construction failed. Error message:"<<
                         error.message());
            throw std::runtime_error("Authenticator construction failed. Error message:" +
                                     error.message());
        }
        
        for(std::vector<SantiagoDBTables::SessionsRec>::iterator it = activeSessionsRec.begin();
            it != activeSessionsRec.end();++it)
        {
            boost::optional<SantiagoDBTables::UsersRec> usersRecOpt =
                _databaseConnection.get().getUsersRecForUserName(it->_userName,error);
            if(error)
            {
                ST_LOG_ERROR("Authenticator construction failed. Error message:"<<
                             error.message());
                throw std::runtime_error("Authenticator construction failed. Error message:" +
                                         error.message());
            }
            
            ST_ASSERT(usersRecOpt);
            _localData.addCookie(usersRecOpt->_userName,usersRecOpt->_emailAddress,*it,boost::none);
            
            if(boost::posix_time::second_clock::universal_time() - it->_lastActiveTime >=
               boost::posix_time::time_duration(MAX_SESSION_DURATION,0,0,0))
            {
                std::error_code error = cleanupLocalCookieDataAndUpdateSessionsRecordImpl(*it);                    
                ST_ASSERT(!error);
            }
        }
    }

    template<typename ClientIdType>
    std::error_code ControllerBase<ClientIdType>::createUser(const std::string& userName_,
                                                             const std::string& emailAddress_,
                                                             const std::string& password_,
                                                             boost::asio::yield_context yield_)
    {

        boost::optional<SantiagoDBTables::UsersRec> usersRecOpt;
        std::error_code error;

        //check for existing accounts with same userName
        std::tie(error,usersRecOpt) = verifyUserNamePasswordAndGetUsersRec(userName_,"");
        if((ErrorCode::ERR_DATABASE_EXCEPTION == error.value()) ||
           (ErrorCode::ERR_DATABASE_INVALID_USER_INPUT == error.value()))
        {
            return error;
        }
        else if(usersRecOpt)
        {
            return std::error_code(ErrorCode::ERR_USERNAME_ALREADY_EXISTS, ErrorCategory::GetInstance());
        }

        //check for existing accounts with same emailAddress
        std::tie(error,usersRecOpt) = verifyEmailAddressPasswordAndGetUsersRec(emailAddress_,"");
        if((ErrorCode::ERR_DATABASE_EXCEPTION == error.value()) ||
           (ErrorCode::ERR_DATABASE_INVALID_USER_INPUT == error.value()))
        {
            return error;
        }
        else if(usersRecOpt)
        {
            return std::error_code(ErrorCode::ERR_EMAIL_ADDRESS_ALREADY_EXISTS, ErrorCategory::GetInstance());
        }

        //create new usersRec and add to db
        SantiagoDBTables::UsersRec usersRec;
        usersRec._userName = userName_;
        usersRec._emailAddress = emailAddress_;
        usersRec._password = generateSHA256(password_);
        _databaseConnection.get().addUsersRec(usersRec,error);
        if(error)
        {
            return error;
        }

        ST_LOG_INFO("Create user successfull for userName:"<<userName_<<std::endl);
        return std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance());
    }

    template<typename ClientIdType>
    std::pair<std::error_code,boost::optional<std::pair<UserInfo,std::string> > > ControllerBase<ClientIdType>::
    loginUser(const ClientIdType& clientId_,
              const std::string& userNameOrEmailAddress_,
              bool isUserNameNotEmailAddress_,
              const std::string& password_,
              boost::asio::yield_context yield_)
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
            return std::pair<std::error_code,boost::optional<std::pair<UserInfo,std::string> > >(error,boost::none);
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
            return std::pair<std::error_code,boost::optional<std::pair<UserInfo,std::string> > >(error,boost::none);
        }
     
        _localData.addCookie(usersRecOpt->_userName,usersRecOpt->_emailAddress,sessionsRec,clientId_);
        
        return std::pair<std::error_code,boost::optional<std::pair<UserInfo,std::string> > >(
            std::error_code(ErrorCode::ERR_SUCCESS,
                            ErrorCategory::GetInstance()),
            std::make_pair(UserInfo(usersRecOpt->_userName,
                                    usersRecOpt->_emailAddress),
                           sessionsRec._cookieString));
    }

    template<typename ClientIdType>
    std::pair<std::error_code,boost::optional<UserInfo> > ControllerBase<ClientIdType>::
    verifyCookieAndGetUserInfo(const ClientIdType& clientId_,
                               const std::string& cookieString_,
                               boost::asio::yield_context yield_)
    {
        boost::optional<SantiagoDBTables::SessionsRec> sessionsRecOpt = _localData.getCookieSessionsRec(cookieString_);
        if(!sessionsRecOpt)
        {
            ST_LOG_INFO("Cookie not valid. cookieString:" <<cookieString_<<std::endl);
            return std::pair<std::error_code,boost::optional<UserInfo> >(
                std::error_code(ErrorCode::ERR_INVALID_SESSION_COOKIE,ErrorCategory::GetInstance()),
                boost::none);
        }

        //check if the lastActiveTime older than MAX_SESSION_DURATION. If yes then logout
        if((boost::posix_time::second_clock::universal_time() - sessionsRecOpt->_lastActiveTime) >
           getMaxSessionInactiveDuration())
        {

            ST_LOG_INFO("Session lastActiveTime older than MAX_SESSION_DURATION. Going to log out. cookieString:"
                     <<cookieString_<<std::endl);            
            std::error_code error = cleanupCookieDataAndUpdateSessionRecord(cookieString_);

            return std::pair<std::error_code,boost::optional<UserInfo> >(
                std::error_code(ErrorCode::ERR_INVALID_SESSION_COOKIE,
                                ErrorCategory::GetInstance()),
                boost::none);
        }
        else //set the lastActiveTime to now
        {
            sessionsRecOpt->_lastActiveTime = boost::posix_time::second_clock::universal_time();
//            std::error_code error; //calling db for every verify is very inefficient. So commenting out
//            _databaseConnection.get().updateSessionsRec(*cookieStringSessionsRecMapIter->second,error_);
        }
        ST_LOG_INFO("Verify cookie successfull. cookieString:"<<cookieString_<<std::endl);

        boost::optional<std::string> emailAddressOpt = _localData.getUserEmailAddress(sessionsRecOpt->_userName);
        ST_ASSERT(emailAddressOpt);

        _localData.updateCookie(*sessionsRecOpt,clientId_);

        return std::pair<std::error_code,boost::optional<UserInfo> >(
            std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance()),
            UserInfo(sessionsRecOpt->_userName,
                     *emailAddressOpt));
    }

    template<typename ClientIdType>
    std::error_code ControllerBase<ClientIdType>::logoutUserForCookie(const std::string& cookieString_,
                                                                      boost::asio::yield_context yield_)
    {
        boost::optional<SantiagoDBTables::SessionsRec> sessionsRecOpt = _localData.getCookieSessionsRec(cookieString_);
        if(!sessionsRecOpt)
        {
            ST_LOG_INFO("Cookie not valid. cookieString:" <<cookieString_<<std::endl);
            return std::error_code(ErrorCode::ERR_INVALID_SESSION_COOKIE,ErrorCategory::GetInstance());
        }

        return cleanupCookieDataAndUpdateSessionRecord(cookieString_,yield_);
    }

        template<typename ClientIdType>
        std::error_code ControllerBase<ClientIdType>::logoutUserForAllCookiesImpl(const std::string& userName_,
                                                                                  boost::asio::yield_context yield_)
    {
        //verify if such a user is already logged in
        if(!_localData.getUserEmailAddress(userName_))
        {
            ST_LOG_INFO("No active session for username. username:"<<userName_);
            return std::error_code(ErrorCode::ERR_NO_ACTIVE_SESSION_FOR_USERNAME,
                                                          ErrorCategory::GetInstance());
        }
        
        return cleanupCookieDataAndUpdateSessionRecordsForAllCookies(userName_,yield_);
    }

    template<typename ClientIdType>
    std::error_code ControllerBase<ClientIdType>::changeUserPassword(
        const std::string& cookieString_,
        const std::string& oldPassword_,
        const std::string& newPassword_,
        boost::asio::yield_context yield_)
    {
        boost::optional<SantiagoDBTables::SessionsRec> sessionsRecOpt = _localData.getCookieSessionsRec(cookieString_);
        if(!sessionsRecOpt)
        {
            ST_LOG_INFO("Cookie not valid. cookieString:" <<cookieString_<<std::endl);
            return std::error_code(ErrorCode::ERR_INVALID_SESSION_COOKIE,ErrorCategory::GetInstance());
        }

        //verify username-password
        std::error_code error;
        boost::optional<SantiagoDBTables::UsersRec> usersRecOpt;
        std::tie(error,usersRecOpt) =
            verifyUserNamePasswordAndGetUsersRec(sessionsRecOpt->_userName, oldPassword_);
        if(error)
        {
            return error;
        }
        ST_ASSERT(usersRecOpt);

        //change and update the password
        usersRecOpt->_password = generateSHA256(newPassword_);
        _databaseConnection.get().updateUsersRec(*usersRecOpt,error);
        //whether succeed or db error...it will be passed to the onChangePasswordCallbackFn
        return error;
    }

    template<typename ClientIdType>
    std::pair<std::error_code,boost::optional<std::string> > ControllerBase<ClientIdType>::
    createAndReturnRecoveryStringImpl(const std::string& emailAddress_,boost::asio::yield_context yield_)
    {
        boost::optional<SantiagoDBTables::UsersRec> usersRecOpt;
        std::error_code error;
        usersRecOpt = _databaseConnection.get().getUsersRecForEmailAddress(emailAddress_,error);
        if(error && (ErrorCode::ERR_DATABASE_GET_RETURNED_ZERO_RESULTS != error.value()))
        {
            return std::pair<std::error_code,boost::optional<std::string> >(error,boost::none);
        }
        if(ErrorCode::ERR_DATABASE_GET_RETURNED_ZERO_RESULTS == error.value())
        {
            return std::pair<std::error_code,boost::optional<std::string> >(
                std::error_code(ErrorCode::ERR_EMAIL_ADDRESS_NOT_REGISTERED,
                                ErrorCategory::GetInstance()),
                boost::none);
        }
            
        ST_ASSERT(usersRecOpt);
        
        SantiagoDBTables::UsersRec newUsersRec = *usersRecOpt;
        newUsersRec._recoveryString = generateRecoveryString();
        newUsersRec._recoveryStringCreateTime = boost::posix_time::second_clock::universal_time();
        _databaseConnection.get().updateUsersRec(newUsersRec,error);
        if(error)
        {
            return std::pair<std::error_code,boost::optional<std::string> >(error,boost::none);
        }
        
        ST_LOG_INFO("Created recovery string successfully for emailAddress:"<<emailAddress_<<std::endl);
        return std::pair<std::error_code,boost::optional<std::string> >(
            std::error_code(ErrorCode::ERR_SUCCESS,
                            ErrorCategory::GetInstance()),
            newUsersRec._recoveryString);
    }

    template<typename ClientIdType>
    std::pair<std::error_code,boost::optional<std::string> > ControllerBase<ClientIdType>::
    getUserForEmailAddressAndRecoveryString(const std::string& emailAddress_,
                                            const std::string& recoveryString_,
                                            boost::asio::yield_context yield_)
    {
        boost::optional<SantiagoDBTables::UsersRec> usersRecOpt;
        std::error_code error;
        usersRecOpt = _databaseConnection.get().getUsersRecForEmailAddress(emailAddress_,error);
        if(error && (ErrorCode::ERR_DATABASE_GET_RETURNED_ZERO_RESULTS != error.value()))
        {
            return std::pair<std::error_code,boost::optional<std::string> >(error,boost::none);
        }
        if(ErrorCode::ERR_DATABASE_GET_RETURNED_ZERO_RESULTS == error.value())
        {
            return std::pair<std::error_code,boost::optional<std::string> >(
                std::error_code(ErrorCode::ERR_EMAIL_ADDRESS_NOT_REGISTERED,
                                ErrorCategory::GetInstance()),
                boost::none);
        }
        ST_ASSERT(usersRecOpt);

        if(usersRecOpt->_recoveryString != recoveryString_)
        {
            return std::pair<std::error_code,boost::optional<std::string> >(
                std::error_code(ErrorCode::ERR_INVALID_EMAIL_ADDRESS_RECOVERY_STRING,
                                ErrorCategory::GetInstance()),
                boost::none);
        }

        return std::pair<std::error_code,boost::optional<std::string> >(
            std::error_code(ErrorCode::ERR_SUCCESS,
                            ErrorCategory::GetInstance()),
            usersRecOpt->_userName);
    }

    template<typename ClientIdType>
    std::error_code ControllerBase<ClientIdType>::
    changeUserPasswordForEmailAddressAndRecoveryStringImpl(const std::string& emailAddress_,
                                                           const std::string& recoveryString_,
                                                           const std::string& newPassword_,
                                                           boost::asio::yield_context yield_)
    {
        boost::optional<SantiagoDBTables::UsersRec> usersRecOpt;
        std::error_code error;
        std::tie(error,usersRecOpt) =
            verifyEmailAddressRecoveryStringAndGetUsersRec(emailAddress_,recoveryString_);
        if(error)
        {
            return error;
        }
        ST_ASSERT(usersRecOpt);
            
        //change and update the password
        usersRecOpt->_password = generateSHA256(newPassword_);
        usersRecOpt->_recoveryString = boost::none;
        usersRecOpt->_recoveryStringCreateTime = boost::none;
        _databaseConnection.get().updateUsersRec(*usersRecOpt,error);
        //whether succeed or db error...it will be passed to the onChangePasswordCallbackFn
        return error;
    }

    template<typename ClientIdType>
    std::error_code ControllerBase<ClientIdType>::changeUserEmailAddress(
        const std::string& cookieString_,
        const std::string& newEmailAddress_,
        const std::string& password_,
        boost::asio::yield_context yield_)
    {
        boost::optional<SantiagoDBTables::SessionsRec> sessionsRecOpt = _localData.getCookieSessionsRec(cookieString_);
        if(!sessionsRecOpt)
        {
            ST_LOG_INFO("Cookie not valid. cookieString:" <<cookieString_<<std::endl);
            return std::error_code(ErrorCode::ERR_INVALID_SESSION_COOKIE,ErrorCategory::GetInstance());
        }
        
        std::error_code error;
        //verify password
        boost::optional<SantiagoDBTables::UsersRec> usersRecOpt;
        std::tie(error,usersRecOpt) =
            verifyUserNamePasswordAndGetUsersRec(sessionsRecOpt->_userName, password_);
        
        if(error)
        {
            //onChangeEmailAddressCallbackFn_(error);
            return;
        }
        ST_ASSERT(usersRecOpt);

        SantiagoDBTables::UsersRec newUsersRec = *usersRecOpt;

        //verify no other existing user with same email address
        usersRecOpt = _databaseConnection.get().getUsersRecForEmailAddress(newEmailAddress_,error);
        if(error && (ErrorCode::ERR_DATABASE_GET_RETURNED_ZERO_RESULTS != error.value()))
        {
            return error;
        }        
        if(usersRecOpt)
        {
            return std::error_code(ErrorCode::ERR_EMAIL_ADDRESS_ALREADY_EXISTS,
                                   ErrorCategory::GetInstance());
        }
        ST_ASSERT(ErrorCode::ERR_DATABASE_GET_RETURNED_ZERO_RESULTS == error.value());

        //change and update the password
        newUsersRec._emailAddress = newEmailAddress_;
        _databaseConnection.get().updateUsersRec(newUsersRec,error);
        //whether succeed or db error...it will be passed to the onChangePasswordCallbackFn
        return error;

    }

    template<typename ClientIdType>
    std::error_code ControllerBase<ClientIdType>::deleteUserImpl(const std::string& cookieString_,
                                                                 boost::asio::yield_context yield_)
    {
        //verify if the cookie is in the cookieStringSessionsRecMap.
        boost::optional<SantiagoDBTables::SessionsRec> sessionsRecOpt = _localData.getCookieSessionsRec(cookieString_);
        if(!sessionsRecOpt)
        {
            ST_LOG_INFO("Cookie not valid. cookieString:" <<cookieString_<<std::endl);
            return std::error_code(ErrorCode::ERR_INVALID_SESSION_COOKIE,ErrorCategory::GetInstance());
        }
        
        std::error_code error;
        //delete from db
        boost::optional<SantiagoDBTables::UsersRec> usersRecOpt;
        std::tie(error,usersRecOpt) =
            verifyUserNamePasswordAndGetUsersRec(sessionsRecOpt->_userName,"");
        if((ErrorCode::ERR_DATABASE_EXCEPTION == error.value()) ||
           (ErrorCode::ERR_DATABASE_INVALID_USER_INPUT == error.value()))
        {
            return error;
        }

        ST_ASSERT(usersRecOpt);
        _databaseConnection.get().deleteUsersRec(usersRecOpt->_userName,error);
        if(error)
        {
            return error;
        }

        //remove from memory
        return cleanupCookieDataAndUpdateSessionRecordsForAllCookies(sessionsRecOpt->_userName);
    }

    template<typename ClientIdType>
    std::pair<std::error_code,boost::optional<SantiagoDBTables::UsersRec> > ControllerBase<ClientIdType>::
    verifyUserNamePasswordAndGetUsersRec(const std::string& userName_, const std::string& password_)
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

    template<typename ClientIdType>
    std::pair<std::error_code,boost::optional<SantiagoDBTables::UsersRec> > ControllerBase<ClientIdType>::
    verifyEmailAddressPasswordAndGetUsersRec(const std::string& emailAddress_,
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

    template<typename ClientIdType>
    std::pair<std::error_code,boost::optional<SantiagoDBTables::UsersRec> > 
    ControllerBase<ClientIdType>::verifyEmailAddressRecoveryStringAndGetUsersRec(const std::string& emailAddress_,
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

    template<typename ClientIdType>
    std::error_code ControllerBase<ClientIdType>::
    cleanupLocalCookieDataAndUpdateSessionsRecordImpl(const SantiagoDBTables::SessionsRec& sessionsRec_)
    {
        //update the db
        sessionsRec_._logoutTime = boost::posix_time::second_clock::universal_time();
        std::error_code error;
        SantiagoDBTables::SessionsRec sessionsRec = sessionsRec_;
        _databaseConnection.get().updateSessionsRec(sessionsRec,error);
        if(error)
        {
            ST_LOG_INFO("updateSessionsRec failed. Logging out without writing to db. CookieString:"
                        <<sessionsRec._cookieString);
        }

        //remove from _userNameUserDataMap
        _localData.removeCookie(sessionsRec._cookieString);
        return std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance());
    }

    template<typename ClientIdType>
    std::error_code ControllerBase<ClientIdType>::
    cleanupCookieDataAndUpdateSessionRecord(const std::string& cookieString_,
                                            boost::asio::yield_context yield_)
    {
        //internal fn. so when this fn is called the cookie should already be verified.
        boost::optional<SantiagoDBTables::SessionsRec> sessionsRecOpt = _localData.getCookieSessionsRec(cookieString_);
        ST_ASSERT(sessionsRecOpt);

        //logout from all clients
        std::error_code error;
        error = logoutCookieFromAllClients(cookieString_,yield_);
        if(error)
        {
            ST_LOG_INFO("logoutCookieFromAllClients failed. CookieString:" << cookieString_ <<std::endl);
            return error;
        }

        return cleanupLocalCookieDataAndUpdateSessionsRecordImpl(*sessionsRecOpt);
    }

    template<typename ClientIdType>
    std::error_code ControllerBase<ClientIdType>::
    cleanupCookieDataAndUpdateSessionRecordsForAllCookies(const std::string& userName_,
                                                          boost::asio::yield_context yield_)
    {
        //internal fn. the username should exist.
        ST_ASSERT(_localData.getUserEmailAddress(userName_));
        
        //logout from all clients
        std::error_code error;
        error = logoutUserFromAllClients(userName_,yield_);
        if(error)
        {
            ST_LOG_INFO("logoutUserFromAllClients failed. userName:" << userName_ <<std::endl);
            return error;
        }

        std::vector<std::string> userCookieList = _localData.getAllCookieStringsForUser(userName_);
        _databaseConnection.get().updateSessionLogoutTimes(userCookieList,
                                                           boost::posix_time::second_clock::universal_time(),
                                                           error);
        
        if(error)
        {
            ST_LOG_INFO("updateSessionsRecsWithCurrentLogoutTime failed. userName:" << userName_);
            return error;
        }
        _localData.removeUser(userName_);

        //the user should also be removed by now
        ST_ASSERT(!_localData.getUserEmailAddress(userName_));
        return std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance());
    }

    template<typename ClientIdType>
    std::string ControllerBase<ClientIdType>::generateSHA256(const std::string str)
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

    template<typename ClientIdType>
    std::string ControllerBase<ClientIdType>::generateUniqueCookie()
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

    template<typename ClientIdType>
    std::string ControllerBase<ClientIdType>::generateRecoveryString()
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

    template<typename ClientIdType>
    boost::posix_time::time_duration ControllerBase<ClientIdType>::getMaxSessionInactiveDuration() const
    {
        return boost::posix_time::time_duration(MAX_SESSION_DURATION,0,0,0);   
    }
        
}}
