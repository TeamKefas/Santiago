#include <boost/date_time/posix_time/posix_time.hpp>

#include "Controller.h"

namespace Santiago{ namespace User{ namespace SingleNode
{

    Controller::Controller(ThreadSpecificDbConnection& databaseConnection_,
                           boost::asio::io_service& ioService_,
                           const boost::property_tree::ptree& config_)
        :User::ControllerBase(ioService_,config_)
        ,_databaseConnection(databaseConnection_)
    {
        srand ( time(NULL) );
        std::error_code error;
        std::vector<SantiagoDBTables::SessionsRec> activeSessionsRec =
            _databaseConnection.get().getActiveSessions(error);
        for(std::vector<SantiagoDBTables::SessionsRec>::iterator it = activeSessionsRec.begin();
            it != activeSessionsRec.end();++it)
        {
            _cookieStringSessionsRecMap.insert(std::make_pair(it->_cookieString,*it));
            boost::optional<SantiagoDBTables::UsersRec> usersRecOpt =
                _databaseConnection.get().getUsersRecForUserName(it->_userName,error);
            BOOST_ASSERT(usersRecOpt);
            // _userNameUserDataMap.insert(std::make_pair(it->_userName,UserData(userRec->_emailAddress)));
            std::map<std::string,UserData>::iterator userNameUserDataMapIter =
                _userNameUserDataMap.find(usersRecOpt->_userName);
            if(_userNameUserDataMap.end() == userNameUserDataMapIter)
            {
                bool isInsertionSuccessfulFlag;
                std::tie(userNameUserDataMapIter,isInsertionSuccessfulFlag) =
                    _userNameUserDataMap.insert(std::make_pair(usersRecOpt->_userName,
                                                               UserData(usersRecOpt->_emailAddress)));
                BOOST_ASSERT(isInsertionSuccessfulFlag);
                BOOST_ASSERT(userNameUserDataMapIter != _userNameUserDataMap.end());
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

     void Controller::createUser(const std::string& userName_,
                                 const std::string& emailAddress_,
                                 const std::string& password_,
                                 const ErrorCodeCallbackFn& onCreateUserCallbackFn_)
    {
        _strand.post(std::bind(&Controller::createUserImpl,
                               this,
                               userName_,
                               emailAddress_,
                               password_,
                               onCreateUserCallbackFn_));
    }

    void Controller::loginUser(const std::string& userNameOrEmailAddress_,
                               bool isUserNameNotEmailAddress_,
                               const std::string& password_,
                               const ErrorCodeUserInfoStringPairCallbackFn& onLoginUserCallbackFn_)
    {
        _strand.post(std::bind(&Controller::loginUserImpl,
                               this,
                               userNameOrEmailAddress_,
                               isUserNameNotEmailAddress_,
                               password_,
                               onLoginUserCallbackFn_));
    }

    void Controller::verifyCookieAndGetUserInfo(const std::string& cookieString_,
                                                const ErrorCodeUserInfoCallbackFn& onVerifyUserCallbackFn_)
    {
        _strand.post(std::bind(&Controller::verifyCookieAndGetUserInfoImpl,
                               this,
                               cookieString_,
                               onVerifyUserCallbackFn_));
    }

    void Controller::logoutUserForCookie(const std::string& cookieString_,
                                         const ErrorCodeCallbackFn& onLogoutCookieCallbackFn_)
    {
        _strand.post(std::bind(&Controller::logoutUserForCookieImpl,this,cookieString_,onLogoutCookieCallbackFn_));
    }

    void Controller::logoutUserForAllCookies(const std::string& currentCookieString_,
                                             const ErrorCodeCallbackFn& onLogoutAllCookiesCallbackFn_)
    {
        _strand.post(std::bind(&Controller::logoutUserForAllCookiesImpl,
                               this,
                               currentCookieString_,
                               onLogoutAllCookiesCallbackFn_));
    }

    void Controller::changeUserPassword(const std::string& cookieString_,
                                        const std::string& oldPassword_,
                                        const std::string& newPassword_,
                                        const ErrorCodeCallbackFn& onChangePasswordCallbackFn_)
    {
        _strand.post(std::bind(&Controller::changeUserPasswordImpl,
                               this,
                               cookieString_,
                               oldPassword_,
                               newPassword_,
                               onChangePasswordCallbackFn_));
    }

    void Controller::changeUserEmailAddress(const std::string& cookieString_,
                                            const std::string& newEmailAddress_,
                                            const std::string& password_,
                                            const ErrorCodeCallbackFn& onChangeEmailAddressCallbackFn_)
    {
        _strand.post(std::bind(&Controller::changeUserEmailAddressImpl,
                               this,
                               cookieString_,
                               newEmailAddress_,
                               password_,
                               onChangeEmailAddressCallbackFn_));
    }

    void Controller::deleteUser(const std::string& cookieString_,
                                const ErrorCodeCallbackFn& onDeleteUserCallbackFn_)
    {
        _strand.post(std::bind(&Controller::deleteUserImpl,this,cookieString_,onDeleteUserCallbackFn_));
    }

    void Controller::postCallbackFn(const ErrorCodeUserInfoCallbackFn& errorCodeUserInfoCallbackFn_,
                                    const std::error_code& error_,
                                    const boost::optional<UserInfo>& userInfoOpt_)
    {
        std::function<void()> errorCodeUserInfoCallbackFnImpl =
            std::bind(errorCodeUserInfoCallbackFn_,error_, userInfoOpt_);
        _ioService.post(errorCodeUserInfoCallbackFnImpl);
    }

    void Controller::postCallbackFn(
        const ErrorCodeUserInfoStringPairCallbackFn& errorCodeUserInfoStringPairCallbackFn_,
        const std::error_code& error_,
        const boost::optional<std::pair<UserInfo,std::string> >& userInfoStringPair_)
    {
        std::function<void()> errorCodeUserInfoStringPairCallbackFnImpl = 
            std::bind(errorCodeUserInfoStringPairCallbackFn_, error_, userInfoStringPair_);
        _ioService.post(errorCodeUserInfoStringPairCallbackFnImpl);
    }

    void Controller::postCallbackFn(const ErrorCodeCallbackFn& errorCodeCallbackFn_,
                                    const std::error_code& error_)
    {
        std::function<void()> errorCodeCallbackFnImpl = std::bind(errorCodeCallbackFn_,error_);
        _ioService.post(errorCodeCallbackFnImpl);
    }

    void Controller::createUserImpl(const std::string& userName_,
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
            postCallbackFn(onCreateUserCallbackFn_,error);
            return;
        }
        else if(usersRecOpt)
        {
            postCallbackFn(onCreateUserCallbackFn_,std::error_code(ErrorCode::ERR_USERNAME_ALREADY_EXISTS,
                                                                   ErrorCategory::GetInstance()));
            return;
        }

        //check for existing accounts with same emailAddress
        std::tie(error,usersRecOpt) = verifyEmailAddressPasswordAndGetUsersRec(emailAddress_,"");
        if((ErrorCode::ERR_DATABASE_EXCEPTION == error.value()) ||
           (ErrorCode::ERR_DATABASE_INVALID_USER_INPUT == error.value()))
        {
            postCallbackFn(onCreateUserCallbackFn_,error);
            return;
        }
        else if(usersRecOpt)
        {
            postCallbackFn(onCreateUserCallbackFn_,std::error_code(ErrorCode::ERR_EMAIL_ADDRESS_ALREADY_EXISTS,
                                                                   ErrorCategory::GetInstance()));
            return;
        }

        //create new usersRec and add to db
        SantiagoDBTables::UsersRec usersRec;
        usersRec._userName = userName_;
        usersRec._emailAddress = emailAddress_;
        usersRec._password = password_;
        _databaseConnection.get().addUsersRec(usersRec,error);
        if(error)
        {
            postCallbackFn(onCreateUserCallbackFn_,error);
            return;
        }

        ST_LOG_INFO("Create user successfull for userName:"<<userName_<<std::endl);
        postCallbackFn(onCreateUserCallbackFn_,std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance()));
        return;
    }

    void Controller::loginUserImpl(const std::string& userNameOrEmailAddress_,
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
            postCallbackFn(onLoginUserCallbackFn_,error,boost::none);
            return;
        }
        BOOST_ASSERT(usersRecOpt);


        //create new session record and add to db
        SantiagoDBTables::SessionsRec sessionsRec;
        sessionsRec._userName = usersRecOpt->_userName;
        //  sessionsRec._cookieString = "12345";//TODO: make this unique
        //   srand ( time(NULL) );
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
            postCallbackFn(onLoginUserCallbackFn_,error,boost::none);
            return;
        }
        
        //update the _cookieStringSessionsRecMap;
        bool isInsertionSuccessfulFlag =
            _cookieStringSessionsRecMap.insert(std::make_pair(sessionsRec._cookieString,sessionsRec)).second;
        BOOST_ASSERT(isInsertionSuccessfulFlag);
        
        //update the _userNameUserSessionsInfoMap;
        std::map<std::string,UserData>::iterator userNameUserDataMapIter =
            _userNameUserDataMap.find(usersRecOpt->_userName);
        if(_userNameUserDataMap.end() == userNameUserDataMapIter)
        {
            std::tie(userNameUserDataMapIter,isInsertionSuccessfulFlag) =
                _userNameUserDataMap.insert(std::make_pair(usersRecOpt->_userName,
                                                           UserData(usersRecOpt->_emailAddress)));
            BOOST_ASSERT(isInsertionSuccessfulFlag);
            BOOST_ASSERT(userNameUserDataMapIter != _userNameUserDataMap.end());
        }
        userNameUserDataMapIter->second._cookieList.push_back(sessionsRec._cookieString);


        postCallbackFn(onLoginUserCallbackFn_,std::error_code(ErrorCode::ERR_SUCCESS,
                                                              ErrorCategory::GetInstance()),
                       std::make_pair(UserInfo(usersRecOpt->_userName,
                                               usersRecOpt->_emailAddress),
                                      sessionsRec._cookieString));
        return;
    }

    void Controller::verifyCookieAndGetUserInfoImpl(const std::string& cookieString_,
                                                    const ErrorCodeUserInfoCallbackFn& onVerifyUserCallbackFn_)
    {
        std::map<std::string,SantiagoDBTables::SessionsRec>::iterator cookieStringSessionsRecMapIter;
        std::error_code error;
        std::tie(error,cookieStringSessionsRecMapIter) = checkForCookieInMapAndGetSessionsRecIter(cookieString_);
        if(error)
        {
            postCallbackFn(onVerifyUserCallbackFn_,error,boost::none);
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

            postCallbackFn(onVerifyUserCallbackFn_,std::error_code(ErrorCode::ERR_INVALID_SESSION_COOKIE,
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
        BOOST_ASSERT(_userNameUserDataMap.end() != userNameUserDataMapIter);


        postCallbackFn(onVerifyUserCallbackFn_,std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance()),
                       UserInfo(cookieStringSessionsRecMapIter->second._userName,
                                userNameUserDataMapIter->second._emailAddress));
        return;
    }

    void Controller::logoutUserForCookieImpl(const std::string& cookieString_,
                                             const ErrorCodeCallbackFn& onLogoutCookieCallbackFn_)
    {
        std::error_code error;
        std::tie(error,std::ignore) = checkForCookieInMapAndGetSessionsRecIter(cookieString_);
        if(error)
        {
            postCallbackFn(onLogoutCookieCallbackFn_,error);
            return;
        }

        cleanupCookieDataAndUpdateSessionRecord(cookieString_);

        postCallbackFn(onLogoutCookieCallbackFn_,std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance()));
        return;        
    }

    void Controller::logoutUserForAllCookiesImpl(const std::string& cookieString_,
                                                 const ErrorCodeCallbackFn& onLogoutAllCookiesCallbackFn_)
    {
        //verify if the cookie is in the cookieStringSessionsRecMap.
        std::map<std::string,SantiagoDBTables::SessionsRec>::iterator cookieStringSessionsRecMapIter;
        std::error_code error;
        std::tie(error,cookieStringSessionsRecMapIter) = checkForCookieInMapAndGetSessionsRecIter(cookieString_);
        if(error)
        {
            postCallbackFn(onLogoutAllCookiesCallbackFn_,error);
            return;
        }

        cleanupCookieDataAndUpdateSessionRecordsForAllCookies(cookieStringSessionsRecMapIter->second._userName);
        postCallbackFn(onLogoutAllCookiesCallbackFn_,std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance()));
        return;
    }

    void Controller::changeUserPasswordImpl(const std::string& cookieString_,
                                            const std::string& oldPassword_,
                                            const std::string& newPassword_,
                                            const ErrorCodeCallbackFn& onChangePasswordCallbackFn_)
    {
        std::map<std::string,SantiagoDBTables::SessionsRec>::iterator cookieStringSessionsRecMapIter;
        std::error_code error;
        std::tie(error,cookieStringSessionsRecMapIter) = checkForCookieInMapAndGetSessionsRecIter(cookieString_);
        if(error)
        {
            postCallbackFn(onChangePasswordCallbackFn_,error);
            return;
        }

        std::string userName = cookieStringSessionsRecMapIter->second._userName;
        //verify username-password
        boost::optional<SantiagoDBTables::UsersRec> usersRecOpt;
        std::tie(error,usersRecOpt) =
            verifyUserNamePasswordAndGetUsersRec(cookieStringSessionsRecMapIter->second._userName,oldPassword_);
        if(error)
        {
            postCallbackFn(onChangePasswordCallbackFn_,error);
            return;
        }
        BOOST_ASSERT(usersRecOpt);
            
        //change and update the password
        usersRecOpt->_password = newPassword_;
        _databaseConnection.get().updateUsersRec(*usersRecOpt,error);
        //whether succeed or db error...it will be passed to the onChangePasswordCallbackFn
        postCallbackFn(onChangePasswordCallbackFn_,error);
        return;

    }

    void Controller::changeUserEmailAddressImpl(const std::string& cookieString_,
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
            verifyUserNamePasswordAndGetUsersRec(cookieStringSessionsRecMapIter->second._userName,password_);
        if(error)
        {
            postCallbackFn(onChangeEmailAddressCallbackFn_,error);
            return;
        }
        BOOST_ASSERT(usersRecOpt);

        SantiagoDBTables::UsersRec newUsersRec = *usersRecOpt;

        //verify no other existing user with same email address
        std::tie(error,usersRecOpt) =
            verifyEmailAddressPasswordAndGetUsersRec(newEmailAddress_,"");
        if((ErrorCode::ERR_DATABASE_EXCEPTION == error.value()) ||
           (ErrorCode::ERR_DATABASE_INVALID_USER_INPUT == error.value()))
        {
            postCallbackFn(onChangeEmailAddressCallbackFn_,error);
            return;
        }
        else if(usersRecOpt)
        {
            postCallbackFn(onChangeEmailAddressCallbackFn_,std::error_code(ErrorCode::ERR_EMAIL_ADDRESS_ALREADY_EXISTS,
                                                                           ErrorCategory::GetInstance()));
            return;
        }

        //change and update the password
        newUsersRec._emailAddress = newEmailAddress_;
        _databaseConnection.get().updateUsersRec(newUsersRec,error);
        //whether succeed or db error...it will be passed to the onChangePasswordCallbackFn
        postCallbackFn(onChangeEmailAddressCallbackFn_,error);
        return;

    }

    void Controller::deleteUserImpl(const std::string& cookieString_,
                                    const ErrorCodeCallbackFn& onDeleteUserCallbackFn_)
    {
        //verify if the cookie is in the cookieStringSessionsRecMap.
        std::map<std::string,SantiagoDBTables::SessionsRec>::iterator cookieStringSessionsRecMapIter;
        std::error_code error;
        std::tie(error,cookieStringSessionsRecMapIter) = checkForCookieInMapAndGetSessionsRecIter(cookieString_);
        if(error)
        {
            postCallbackFn(onDeleteUserCallbackFn_,error);
            return;
        }

        //delete from db
        boost::optional<SantiagoDBTables::UsersRec> usersRecOpt;
        std::tie(error,usersRecOpt) =
            verifyUserNamePasswordAndGetUsersRec(cookieStringSessionsRecMapIter->second._userName,"");
        if((ErrorCode::ERR_DATABASE_EXCEPTION == error.value()) ||
           (ErrorCode::ERR_DATABASE_INVALID_USER_INPUT == error.value()))
        {
            postCallbackFn(onDeleteUserCallbackFn_,error);
            return;
        }

        BOOST_ASSERT(usersRecOpt);
        _databaseConnection.get().deleteUsersRec(usersRecOpt->_userName,error);
        if(error)
        {
            postCallbackFn(onDeleteUserCallbackFn_,error);
            return;
        }

        //remove from memory
        cleanupCookieDataAndUpdateSessionRecordsForAllCookies(cookieStringSessionsRecMapIter->second._userName);
        postCallbackFn(onDeleteUserCallbackFn_,std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance()));
        return;
    }


    std::pair<std::error_code,boost::optional<SantiagoDBTables::UsersRec> > 
    Controller::verifyUserNamePasswordAndGetUsersRec(const std::string& userName_, const std::string& password_)
    {
        //get the UsersRec from db
        std::error_code error;
        boost::optional<SantiagoDBTables::UsersRec> usersRecOpt = 
            _databaseConnection.get().getUsersRecForUserName(userName_,error);
        if(error)
        {
            return std::make_pair(error,usersRecOpt);
        }

        //check if the username/password matches
        if(!usersRecOpt || (usersRecOpt->_password != password_))
        {
            ST_LOG_INFO("Wrong username_password. userName:"<<userName_<<std::endl);
            return std::make_pair(std::error_code(ErrorCode::ERR_INVALID_USERNAME_PASSWORD,
                                                  ErrorCategory::GetInstance()),
                                  usersRecOpt);
        }

        ST_LOG_INFO("Username password verified for userName:"<<userName_<<std::endl);
        return std::make_pair(std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance()),usersRecOpt);
    }

    std::pair<std::error_code,boost::optional<SantiagoDBTables::UsersRec> > 
    Controller::verifyEmailAddressPasswordAndGetUsersRec(const std::string& emailAddress_,
                                                                const std::string& password_)
    {
        //get the UsersRec from db
        std::error_code error;
        boost::optional<SantiagoDBTables::UsersRec> usersRecOpt = 
            _databaseConnection.get().getUsersRecForEmailAddress(emailAddress_,error);
        if(error)//TODO
        {
            return std::make_pair(error,usersRecOpt);
        }

        //check if the username/password matches
        if(!usersRecOpt || (usersRecOpt->_password != password_))
        {
            ST_LOG_INFO("Wrong emailaddress_password. emailAddress:"<<emailAddress_<<std::endl);
            return std::make_pair(std::error_code(ErrorCode::ERR_INVALID_EMAIL_ADDRESS_PASSWORD,
                                                  ErrorCategory::GetInstance()),
                                  usersRecOpt);
        }

        ST_LOG_INFO("EmailAddress password verified for emailAddress:"<<emailAddress_<<std::endl);
        return std::make_pair(std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance()),usersRecOpt);
    }


    std::pair<std::error_code,std::map<std::string,Santiago::SantiagoDBTables::SessionsRec>::iterator > 
    Controller::checkForCookieInMapAndGetSessionsRecIter(const std::string& cookieString_)
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
    
    void Controller::cleanupCookieDataAndUpdateSessionRecord(const std::string& cookieString_)
    {
        //internal fn. so when this fn is called the cookie should already be verified.
        std::map<std::string,SantiagoDBTables::SessionsRec>::iterator cookieStringSessionsRecMapIter =
            _cookieStringSessionsRecMap.find(cookieString_);
        BOOST_ASSERT(cookieStringSessionsRecMapIter != _cookieStringSessionsRecMap.end());

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
        BOOST_ASSERT(userNameUserDataMapIter != _userNameUserDataMap.end());
        std::vector<std::string>::iterator cookieListIter =
            std::find(userNameUserDataMapIter->second._cookieList.begin(),
                      userNameUserDataMapIter->second._cookieList.end(),
                      cookieString_);
        BOOST_ASSERT(cookieListIter != userNameUserDataMapIter->second._cookieList.end());
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

    void Controller:: cleanupCookieDataAndUpdateSessionRecordsForAllCookies(const std::string& userName_)
    {
        //internal fn. the username should exist.
        std::map<std::string,UserData >::iterator userNameUserDataMapIter =
            _userNameUserDataMap.find(userName_);
        BOOST_ASSERT(userNameUserDataMapIter != _userNameUserDataMap.end());
        
        while(userNameUserDataMapIter->second._cookieList.size() > 1)
        {
            cleanupCookieDataAndUpdateSessionRecord(userNameUserDataMapIter->second._cookieList[0]);
        }
        //now there is 1 cookie remaining in the cookieList vector
        BOOST_ASSERT(userNameUserDataMapIter->second._cookieList.size() == 1);
        cleanupCookieDataAndUpdateSessionRecord(userNameUserDataMapIter->second._cookieList[0]);
        BOOST_ASSERT(userNameUserDataMapIter == _userNameUserDataMap.end());

        return;
    }

    std::string Controller::generateUniqueCookie()
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

}}}
