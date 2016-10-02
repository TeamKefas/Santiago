#include <boost/date_time/posix_time/posix_time.hpp>

#include "Controller.h"

namespace Santiago{ namespace User{ namespace SingleNode
{

    Controller::Controller(SantiagoDBTables::MariaDBConnection& databaseConnection_,
                           boost::asio::io_service& ioService_,
                           const boost::property_tree::ptree& config_)
        :User::ControllerBase(ioService_,config_)
        ,_databaseConnection(databaseConnection_)
    {}

    void Controller::createUserImpl(const std::string& userName_,
                                    const std::string& password_,
                                    const UserInfoErrorCodeCallbackFn& onCreateUserCallbackFn_)
    {
        //check for existing accounts with same userName
        std::optional<SantiagoDBTables::UserProfilesRec> userProfilesRecOpt;
        std::error_code error = _databaseConnection.getUserProfilesRec(userName_,userProfilesRecOpt);
        if(error)
        {
            LOG_DEBUG("Database Error for getUserProfilesRec(). userName:"<<userName_<<std::endl);
            BOOST_ASSERT(false);
            ocCreateUserCallbackFn_(boost::none,error);
            return;
        }

        if(userProfilesRec)
        {
            onCreateUserCallbackFn_(boost::none,std::error_code(Error::USERNAME_ALREADY_EXISTS,
                                                                Error::ErrorCategory::GetInstance()));
            return;
        }

        //create new userProfilesRec and add to db
        SantiagoDBTables::UserProfilesRec userProfilesRec;
        userProfilesRec._userName = userName_;
        userProfilesRec._password = password_;
        error = _databaseConnection.addUserProfilesRec(userProfilesRec);
        if(error)
        {
            LOG_DEBUG("Database Error for addUserProfilesRec(). userName:"<<userName_<<std::endl);
            BOOST_ASSERT(false);
            ocCreateUserCallbackFn_(boost::none,error);
            return;
        }

        LOG_INFO("Create user successfull for userName:"<<userName_<<std::endl);
        onCreateUserCallbackFn_(UserInfo(userName_,boost::none),error); //Note the cookieString is still optional
                                                                        //This means the user is still not logged in
        return;
    }

    void Controller::loginUserImpl(const std::string& userName_,
                                   const std::string& password_,
                                   const UserInfoErrorCodeCallbackFn& onLoginUserCallbackFn_)
    {
        //verify username-password
        std::optional<SantiagoDBTables::UserProfilesRec> userProfilesRecOpt;
        std::error_code error = _databaseConnection.getUserProfilesRec(userName_,userProfilesRecOpt);
        if(error)
        {
            LOG_DEBUG("Database Error for getUserProfilesRec(). userName:"<<userName_<<std::endl);
            BOOST_ASSERT(false);
            ocLoginUserCallbackFn_(boost::none,error);
            return;
        }

        if(!userProfilesRec || (userProfilesRec->_password != password_))
        {
            LOG_INFO("Wrong username_password. userName:"<<userName_<<std::endl);
            onLoginUserCallbackFn_(boost::none,std::error_code(Error::INVALID_USERNAME_PASSWORD,
                                                               Error::ErrorCategory::GetInstance()));
            return;
        }

        LOG_INFO("Username password verified for userName:"<<userName_<<std::endl);
        //create new session record and add to db
        SantiagoDBTables::SessionsRec sessionsRec;
        sessionsRec._userName = userName_;
        sessionsRec._cookieId = "12345";//TODO: make this unique
        sessionsRec._loginTime = boost::posix_time::second_clock::universal_time();
        sessionsRec._lastActiveTime = sessionsRec._loginTime;

        error = _databaseConnection.addSessionsRec(sessionsRec);
        if(error)
        {
            LOG_DEBUG("Add sessionsRec failed. userName:"<<userName_<<std::endl);
            onLoginUserCallbackFn_(boost::none,error);
            return;
        }
        
        //update the maps
        bool isInsertionSuccessfulFlag =
            _cookieStringSessionsRecMap.insert(std::make_pair(sessionsRec._cookieId,sessionsRec)).first;
        BOOST_ASSERT(isInsertionSuccessfulFlag);
        _userNameCookieListMap[userName_].push_back(sessionsRec._cookieId);

        onLoginUserCallbackFn_(UserInfo(userName_,sessionsRec._cookieId),
                               std::error_code(Error::SUCCESS,
                                               Error::ErrorCategory::GetInstance()));
        return;
    }

    void Controller::verifyUserForCookieImpl(const std::string cookieString_,
                                             const UserInfoErrorCodeCallbackFn& onVerifyUserCallbackFn_)
    {

        //if the cookie is not in the cookieStringSessionsRecMap. Then try loading from the db
        std::map<std::string,std::string>::iterator cookieStringSessionsRecMapIter =
            _cookieStringSessionsRecMap.find(cookieString_);
        if(cookieStringSessionsRecMapIter == _cookieStringSessionsRecMap.end())
        {
            std::optional<SantiagoDBTables::SessionsRec> sessionsRecOpt;
            std::error_code error = _databaseConnection.getSessionsRec(cookieString_,sessionsRecOpt);
            if(error)
            {
                LOG_DEBUG("Database Error for getSessionsRec(). cookieString:"<<cookieString_<<std::endl);
                BOOST_ASSERT(false);
                onVerifyUserCallbackFn_(boost::none,error);
                return;
            }
            
            //check for cookie does not exist or is already looged out
            if(!sessionsRecOpt || sessionRecOpt->_logoutTime)
            {
                LOG_INFO("Session does not exist or is already logged out for cookie. cookieString:"
                         <<cookieString_<<std::endl);
                onVerifyUserCallbackFn_(boost::none,std::error_code(Error::INVALID_SESSION_COOKIE,
                                                                    Error::ErrorCategory::GetInstance()));
                return;
            }
            LOG_INFO("Session successfully retrieved for cookie. cookieString:"<<cookieString_<<std::endl);

            //load the cookie sessionRec to the _cookieStringSessionsRecMap and update the _userNameCookieListMap
            bool isInsertionSuccessfulFlag;
            std::tie(isInsertionSuccessfulFlag,cookieStringSessionsRecMapIter) =
                _cookieStringSessionsRecMap.insert(std::make_pair(cookieString_,*sessionRecOpt));
            BOOST_ASSERT(isInsertionSuccessfulFlag);

            _userNameCookieListMap[sessionsRecOpt->_userName].push_back(cookieString);
        }

        //check if the lastActiveTime older than MAX_SESSION_DURATION. If yes then logout
        if((boost::posix_time::second_clock::universal_time() -
            cookieStringSessionsRecMapIter->second._lastActiveTime) >
           boost::posix_time::time_duration(MAX_SESSION_DURATION,0,0,0))
        {

            LOG_INFO("Session lastActiveTime older than MAX_SESSION_DURATION. Going to log out. cookieString:"
                     <<cookieString_<<std::endl);            
            std::error_code::error = cleanupCookieDataAndUpdateSessionRecord(cookieString_);
            BOOST_ASSERT(!error);

            onVerifyUserCallbackFn_(boost::none,std::error_code(Error::INVALID_SESSION_COOKIE,
                                                                Error::ErrorCategory::GetInstance()));
            return 
        }
        else //set the lastActiveTime to now
        {
            cookieStringSessionsRecMapIter->second._lastActiveTime = boost::posix_time::second_clock::universal_time();
        }

        LOG_INFO("Verify cookie successfull. cookieString:"<<cookieString_<<std::endl);
        onVerifyUserCallbackFn_(UserInfo(sessionsRecOpt->_userName,cookieString_),
                                std::error_code(Error::SUCCESS,Error::ErrorCategory::GetInstance()));
        return;
    }

    void Controller::logoutUserForCookieImpl(const std::string& cookieString_,
                                             const ErrorCodeCallbackFn& onLogoutCookieCallbackFn_)
    {
        std::map<std::string,std::string>::iterator cookieStringSessionsRecMapIter =
            _cookieStringSessionsRecMap.find(cookieString_);
        //it is assumed that the user is verified
        BOOST_ASSERT(cookieStringSessionsRecMapIter != _cookieStringSessionsRecMap.end());

        std::error_code::error = cleanupCookieDataAndUpdateSessionRecord(cookieString_);
        BOOST_ASSERT(!error);
        
        onLogoutUserCallbackFn_(boost::none,std::error_code(Error::SUCEESS));
        return 
        
    }

    void Controller::logoutUserForAllCookiesImpl(const std::string& userName_,
                                                 const ErrorCodeCallbackFn& onLogoutAllCookiesCallbackFn_)
    {
    }

    void Controller::changeUserPasswordImpl(const std::string& userName_,
                                            const std::string& oldPassword_,
                                            const std::string& newPassword_,
                                            const ErrorCodeCallbackFn& onChangePasswordCallbackFn_)
    {
    }

    void Controller::deleteUserImpl(const std::string& userName_,
                                    const ErrorCodeCallbackFn& onDeleteUserCallbackFn_)
    {
    }

}}}
