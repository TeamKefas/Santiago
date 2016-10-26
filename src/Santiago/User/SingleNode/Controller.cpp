#include <boost/date_time/posix_time/posix_time.hpp>

#include "Controller.h"

namespace Santiago{ namespace User{ namespace SingleNode
{

    Controller::Controller(SantiagoDBTables::MariaDBConnection& databaseConnection_,
                           boost::asio::io_service& ioService_,
                           const boost::property_tree::ptree& config_)
        :User::ControllerBase(ioService_,config_)
        ,_databaseConnection(databaseConnection_)
    {
        srand ( time(NULL) );
    }

    std::string Controller::generateUniqueCookie()
    {
        std::string str;
        static const char alphanum[] =
            "0123456789"
            "!@#$%^*"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
        
        int stringLength = sizeof(alphanum) - 1;
	
        //return alphanum[rand() % stringLength];
	for(unsigned int i = 0; i < 45; ++i)
	{
            str += alphanum[rand() % stringLength];
	}
        return str;
    }

    void Controller::createUserImpl(const std::string& userName_,
                                    const std::string& password_,
                                    const ErrorCodeCallbackFn& onCreateUserCallbackFn_)
    {
        //check for existing accounts with same userName
        boost::optional<SantiagoDBTables::UserProfilesRec> userProfilesRecOpt;
        std::error_code error;
        std::tie(error,userProfilesRecOpt) = verifyUserNamePasswordAndGetUserProfilesRec(userName_,password_);
        if(ErrorCode::ERR_DATABASE_EXCEPTION == error.value())
        {
            onCreateUserCallbackFn_(error);
            return;
        }
        else if(ErrorCode::ERR_INVALID_USERNAME_PASSWORD != error.value())
        {
            onCreateUserCallbackFn_(std::error_code(ErrorCode::ERR_USERNAME_ALREADY_EXISTS,
                                                    ErrorCategory::GetInstance()));
            return;
        }

        //create new userProfilesRec and add to db
        SantiagoDBTables::UserProfilesRec userProfilesRec;
        userProfilesRec._userName = userName_;
        userProfilesRec._password = password_;
        _databaseConnection.addUserProfilesRec(userProfilesRec,error);
        if(error)
        {
            onCreateUserCallbackFn_(error);
            return;
        }

        ST_LOG_INFO("Create user successfull for userName:"<<userName_<<std::endl);
        onCreateUserCallbackFn_(std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance()));
        return;
    }

    void Controller::loginUserImpl(const std::string& userName_,
                                   const std::string& password_,
                                   const ErrorCodeStringCallbackFn& onLoginUserCallbackFn_)
    {
        //verify username-password
        boost::optional<SantiagoDBTables::UserProfilesRec> userProfilesRecOpt;
        std::error_code error;
        std::tie(error,userProfilesRecOpt) = verifyUserNamePasswordAndGetUserProfilesRec(userName_,
                                                                                         password_);
                                                                                         //,userProfilesRecOpt);
        if(error)
        {
            onLoginUserCallbackFn_(error,boost::optional<std::string>());
            return;
        }

        //create new session record and add to db
        SantiagoDBTables::SessionsRec sessionsRec;
        sessionsRec._userName = userName_;
        //  sessionsRec._cookieString = "12345";//TODO: make this unique
        //   srand ( time(NULL) );
        sessionsRec._cookieString = generateUniqueCookie();
        
        sessionsRec._loginTime = boost::posix_time::second_clock::universal_time();
        sessionsRec._lastActiveTime = sessionsRec._loginTime;

        _databaseConnection.addSessionsRec(sessionsRec,error);
        if(error)
        {
            onLoginUserCallbackFn_(error,boost::optional<std::string>());
            return;
        }
        
        //update the maps
        bool isInsertionSuccessfulFlag =
            _cookieStringSessionsRecMap.insert(std::make_pair(sessionsRec._cookieString,sessionsRec)).second;
        BOOST_ASSERT(isInsertionSuccessfulFlag);
        _userNameCookieListMap[userName_].push_back(sessionsRec._cookieString);

        onLoginUserCallbackFn_(std::error_code(ErrorCode::ERR_SUCCESS,
                                               ErrorCategory::GetInstance()),
                               sessionsRec._cookieString);
        return;
    }

    void Controller::verifyCookieAndGetUserNameImpl(const std::string& cookieString_,
                                                    const ErrorCodeStringCallbackFn& onVerifyUserCallbackFn_)
    {
        std::map<std::string,SantiagoDBTables::SessionsRec>::iterator cookieStringSessionsRecMapIter;
        std::error_code error;
        std::tie(error,cookieStringSessionsRecMapIter) = checkForCookieInMapAndGetSessionsRecIter(cookieString_);
        if(error)
        {
            onVerifyUserCallbackFn_(error,boost::optional<std::string>());
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
                                    boost::optional<std::string>());
            return;
        }
        else //set the lastActiveTime to now
        {
            cookieStringSessionsRecMapIter->second._lastActiveTime = boost::posix_time::second_clock::universal_time();
//            std::error_code error; //calling db for every verify is very inefficient. So commenting out
//            _databaseConnection.updateSessionsRec(*cookieStringSessionsRecMapIter->second,error_);
        }

        ST_LOG_INFO("Verify cookie successfull. cookieString:"<<cookieString_<<std::endl);

        onVerifyUserCallbackFn_(std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance()),
                                cookieStringSessionsRecMapIter->second._userName);
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
        _databaseConnection.updateUserProfilesRec(*userProfilesRecOpt,error);
        //whether succeed or db error...it will be passed to the onChangePasswordCallbackFn
        onChangePasswordCallbackFn_(error);
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
        if(ErrorCode::ERR_DATABASE_EXCEPTION == error.value())
        {
            onDeleteUserCallbackFn_(error);
        }

        BOOST_ASSERT(userProfilesRecOpt);
        _databaseConnection.deleteUserProfilesRec(userProfilesRecOpt->_userName,error);
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
            _databaseConnection.getUserProfilesRec(userName_,error);
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
        _databaseConnection.updateSessionsRec(cookieStringSessionsRecMapIter->second,error);
        if(error)
        {
            ST_LOG_INFO("updateSessionsRec failed. Logging out without writing to db. SessionsRec:"
                     <<std::endl << cookieStringSessionsRecMapIter->second <<std::endl);
        }

        //remove from _userNameCookieListMap
        std::map<std::string,std::vector<std::string> >::iterator userNameCookieListMapIter =
            _userNameCookieListMap.find(cookieStringSessionsRecMapIter->second._userName);
        BOOST_ASSERT(userNameCookieListMapIter != _userNameCookieListMap.end());
        std::vector<std::string>::iterator cookieListIter =
            std::find(userNameCookieListMapIter->second.begin(),userNameCookieListMapIter->second.end(),cookieString_);
        BOOST_ASSERT(cookieListIter != userNameCookieListMapIter->second.end());
        userNameCookieListMapIter->second.erase(cookieListIter);

        //remove from _cookieStringSessionsRecMap
        _cookieStringSessionsRecMap.erase(cookieStringSessionsRecMapIter);

        return;
    }

    void Controller:: cleanupCookieDataAndUpdateSessionRecordsForAllCookies(const std::string& userName_)
    {
        //internal fn. the username should exist.
        std::map<std::string,std::vector<std::string> >::iterator userNameCookieListMapIter =
            _userNameCookieListMap.find(userName_);
        BOOST_ASSERT(userNameCookieListMapIter != _userNameCookieListMap.end());

        while(userNameCookieListMapIter->second.begin() != userNameCookieListMapIter->second.end())
        {
            cleanupCookieDataAndUpdateSessionRecord(*userNameCookieListMapIter->second.begin());
        }
        return;
    }

}}}
