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

    void Controller::createUserImpl(const std::string& userName_,
                                    const std::string& emailAddress_,
                                    const std::string& password_,
                                    const ErrorCodeCallbackFn& onCreateUserCallbackFn_)
    {

        boost::optional<SantiagoDBTables::UserProfilesRec> userProfilesRecOpt;
        std::error_code error;

        //check for existing accounts with same userName
        std::tie(error,userProfilesRecOpt) = verifyUserNamePasswordAndGetUserProfilesRec(userName_,"");
        if((ErrorCode::ERR_DATABASE_EXCEPTION == error.value()) ||
           (ErrorCode::ERR_DATABASE_INVALID_USER_INPUT == error.value()))
        {
            onCreateUserCallbackFn_(error);
            return;
        }
        else if(userProfilesRecOpt)
        {
            onCreateUserCallbackFn_(std::error_code(ErrorCode::ERR_USERNAME_ALREADY_EXISTS,
                                                    ErrorCategory::GetInstance()));
            return;
        }

        //check for existing accounts with same emailAddress
        std::tie(error,userProfilesRecOpt) = verifyEmailAddressPasswordAndGetUserProfilesRec(emailAddress_,"");
        if((ErrorCode::ERR_DATABASE_EXCEPTION == error.value()) ||
           (ErrorCode::ERR_DATABASE_INVALID_USER_INPUT == error.value()))
        {
            onCreateUserCallbackFn_(error);
            return;
        }
        else if(userProfilesRecOpt)
        {
            onCreateUserCallbackFn_(std::error_code(ErrorCode::ERR_EMAIL_ADDRESS_ALREADY_EXISTS,
                                                    ErrorCategory::GetInstance()));
            return;
        }

        //create new userProfilesRec and add to db
        SantiagoDBTables::UserProfilesRec userProfilesRec;
        userProfilesRec._userName = userName_;
        userProfilesRec._emailAddress = emailAddress_;
        userProfilesRec._password = password_;
        _databaseConnection.get().addUserProfilesRec(userProfilesRec,error);
        if(error)
        {
            onCreateUserCallbackFn_(error);
            return;
        }

        ST_LOG_INFO("Create user successfull for userName:"<<userName_<<std::endl);
        onCreateUserCallbackFn_(std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance()));
        return;
    }

    void Controller::loginUserImpl(const std::string& userNameOrEmailAddress_,
                                   bool isUserNameNotEmailAddress_,
                                   const std::string& password_,
                                   const ErrorCodeUserInfoStringPairCallbackFn& onLoginUserCallbackFn_)
    {
        //verify username-password
        boost::optional<SantiagoDBTables::UserProfilesRec> userProfilesRecOpt;
        std::error_code error;

        if(isUserNameNotEmailAddress_)
        {
            std::tie(error,userProfilesRecOpt) =
                verifyUserNamePasswordAndGetUserProfilesRec(userNameOrEmailAddress_,
                                                            password_);
        }
        else
        {
            std::tie(error,userProfilesRecOpt) =
                verifyEmailAddressPasswordAndGetUserProfilesRec(userNameOrEmailAddress_,
                                                                password_);
        }

        if(error)
        {
            onLoginUserCallbackFn_(error,boost::none);
            return;
        }
        BOOST_ASSERT(userProfilesRecOpt);


        //create new session record and add to db
        SantiagoDBTables::SessionsRec sessionsRec;
        sessionsRec._userName = userProfilesRecOpt->_userName;
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
            onLoginUserCallbackFn_(error,boost::none);
            return;
        }
        
        //update the _cookieStringSessionsRecMap;
        bool isInsertionSuccessfulFlag =
            _cookieStringSessionsRecMap.insert(std::make_pair(sessionsRec._cookieString,sessionsRec)).second;
        BOOST_ASSERT(isInsertionSuccessfulFlag);
        
        //update the _userNameUserSessionsInfoMap;
        std::map<std::string,UserData>::iterator userNameUserDataMapIter =
            _userNameUserDataMap.find(userProfilesRecOpt->_userName);
        if(_userNameUserDataMap.end() == userNameUserDataMapIter)
        {
            std::tie(userNameUserDataMapIter,isInsertionSuccessfulFlag) =
                _userNameUserDataMap.insert(std::make_pair(userProfilesRecOpt->_userName,
                                                           UserData(userProfilesRecOpt->_emailAddress)));
            BOOST_ASSERT(isInsertionSuccessfulFlag);
            BOOST_ASSERT(userNameUserDataMapIter != _userNameUserDataMap.end());
        }
        userNameUserDataMapIter->second._cookieList.push_back(sessionsRec._cookieString);


        onLoginUserCallbackFn_(std::error_code(ErrorCode::ERR_SUCCESS,
                                               ErrorCategory::GetInstance()),
                               std::make_pair(UserInfo(userProfilesRecOpt->_userName,
                                                       userProfilesRecOpt->_emailAddress),
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
            onVerifyUserCallbackFn_(error,boost::none);
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
        BOOST_ASSERT(_userNameUserDataMap.end() != userNameUserDataMapIter);


        onVerifyUserCallbackFn_(std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance()),
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
            onLogoutCookieCallbackFn_(error);
        }

        cleanupCookieDataAndUpdateSessionRecord(cookieString_);

        onLogoutCookieCallbackFn_(std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance()));
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
            onLogoutAllCookiesCallbackFn_(error);
        }

        cleanupCookieDataAndUpdateSessionRecordsForAllCookies(cookieStringSessionsRecMapIter->second._userName);
        onLogoutAllCookiesCallbackFn_(std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance()));
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
            onChangePasswordCallbackFn_(error);
        }

        std::string userName = cookieStringSessionsRecMapIter->second._userName;
        //verify username-password
        boost::optional<SantiagoDBTables::UserProfilesRec> userProfilesRecOpt;
        std::tie(error,userProfilesRecOpt) =
            verifyUserNamePasswordAndGetUserProfilesRec(cookieStringSessionsRecMapIter->second._userName,oldPassword_);
        if(error)
        {
            onChangePasswordCallbackFn_(error);
            return;
        }
        BOOST_ASSERT(userProfilesRecOpt);
            
        //change and update the password
        userProfilesRecOpt->_password = newPassword_;
        _databaseConnection.get().updateUserProfilesRec(*userProfilesRecOpt,error);
        //whether succeed or db error...it will be passed to the onChangePasswordCallbackFn
        onChangePasswordCallbackFn_(error);
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
        }

        std::string userName = cookieStringSessionsRecMapIter->second._userName;
        //verify password
        boost::optional<SantiagoDBTables::UserProfilesRec> userProfilesRecOpt;
        std::tie(error,userProfilesRecOpt) =
            verifyUserNamePasswordAndGetUserProfilesRec(cookieStringSessionsRecMapIter->second._userName,password_);
        if(error)
        {
            onChangeEmailAddressCallbackFn_(error);
            return;
        }
        BOOST_ASSERT(userProfilesRecOpt);

        SantiagoDBTables::UserProfilesRec newUserProfilesRec = *userProfilesRecOpt;

        //verify no other existing user with same email address
        std::tie(error,userProfilesRecOpt) =
            verifyEmailAddressPasswordAndGetUserProfilesRec(newEmailAddress_,"");
        if((ErrorCode::ERR_DATABASE_EXCEPTION == error.value()) ||
           (ErrorCode::ERR_DATABASE_INVALID_USER_INPUT == error.value()))
        {
            onChangeEmailAddressCallbackFn_(error);
            return;
        }
        else if(userProfilesRecOpt)
        {
            onChangeEmailAddressCallbackFn_(std::error_code(ErrorCode::ERR_EMAIL_ADDRESS_ALREADY_EXISTS,
                                                            ErrorCategory::GetInstance()));
            return;
        }

        //change and update the password
        newUserProfilesRec._emailAddress = newEmailAddress_;
        _databaseConnection.get().updateUserProfilesRec(newUserProfilesRec,error);
        //whether succeed or db error...it will be passed to the onChangePasswordCallbackFn
        onChangeEmailAddressCallbackFn_(error);
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
            onDeleteUserCallbackFn_(error);
        }

        //delete from db
        boost::optional<SantiagoDBTables::UserProfilesRec> userProfilesRecOpt;
        std::tie(error,userProfilesRecOpt) =
            verifyUserNamePasswordAndGetUserProfilesRec(cookieStringSessionsRecMapIter->second._userName,"");
        if((ErrorCode::ERR_DATABASE_EXCEPTION == error.value()) ||
           (ErrorCode::ERR_DATABASE_INVALID_USER_INPUT == error.value()))
        {
            onDeleteUserCallbackFn_(error);
        }

        BOOST_ASSERT(userProfilesRecOpt);
        _databaseConnection.get().deleteUserProfilesRec(userProfilesRecOpt->_userName,error);
        if(error)
        {
            onDeleteUserCallbackFn_(error);
        }

        //remove from memory
        cleanupCookieDataAndUpdateSessionRecordsForAllCookies(cookieStringSessionsRecMapIter->second._userName);
        onDeleteUserCallbackFn_(std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance()));
        return;
    }


    std::pair<std::error_code,boost::optional<SantiagoDBTables::UserProfilesRec> > 
    Controller::verifyUserNamePasswordAndGetUserProfilesRec(const std::string& userName_, const std::string& password_)
    {
        //get the UserProfilesRec from db
        std::error_code error;
        boost::optional<SantiagoDBTables::UserProfilesRec> userProfilesRecOpt = 
            _databaseConnection.get().getUserProfilesRecForUserName(userName_,error);
        if(error)//TODO
        {
            return std::make_pair(error,userProfilesRecOpt);
        }

        //check if the username/password matches
        if(!userProfilesRecOpt || (userProfilesRecOpt->_password != password_))
        {
            ST_LOG_INFO("Wrong username_password. userName:"<<userName_<<std::endl);
            return std::make_pair(std::error_code(ErrorCode::ERR_INVALID_USERNAME_PASSWORD,
                                                  ErrorCategory::GetInstance()),
                                  userProfilesRecOpt);
        }

        ST_LOG_INFO("Username password verified for userName:"<<userName_<<std::endl);
        return std::make_pair(std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance()),userProfilesRecOpt);
    }

    std::pair<std::error_code,boost::optional<SantiagoDBTables::UserProfilesRec> > 
    Controller::verifyEmailAddressPasswordAndGetUserProfilesRec(const std::string& emailAddress_,
                                                                const std::string& password_)
    {
        //get the UserProfilesRec from db
        std::error_code error;
        boost::optional<SantiagoDBTables::UserProfilesRec> userProfilesRecOpt = 
            _databaseConnection.get().getUserProfilesRecForEmailAddress(emailAddress_,error);
        if(error)//TODO
        {
            return std::make_pair(error,userProfilesRecOpt);
        }

        //check if the username/password matches
        if(!userProfilesRecOpt || (userProfilesRecOpt->_password != password_))
        {
            ST_LOG_INFO("Wrong emailaddress_password. emailAddress:"<<emailAddress_<<std::endl);
            return std::make_pair(std::error_code(ErrorCode::ERR_INVALID_EMAIL_ADDRESS_PASSWORD,
                                                  ErrorCategory::GetInstance()),
                                  userProfilesRecOpt);
        }

        ST_LOG_INFO("EmailAddress password verified for emailAddress:"<<emailAddress_<<std::endl);
        return std::make_pair(std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance()),userProfilesRecOpt);
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
            ST_LOG_INFO("updateSessionsRec failed. Logging out without writing to db. SessionsRec:"
                     <<std::endl << cookieStringSessionsRecMapIter->second <<std::endl);
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
