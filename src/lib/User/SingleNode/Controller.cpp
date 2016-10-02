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
                                    const ErrorCodeCallbackFn& onCreateUserCallbackFn_)
    {
        //check for existing accounts with same userName
        std::optional<SantiagoDBTables::UserProfilesRec> userProfilesRecOpt;
        std::error_code error;
        std::tie(error,userProfilesRecOpt) = verifyUserNamePasswordAndGetUserProfilesRec(userName_,password_);
        if(Error::DATABASE_ERROR == error.value())
        {
            ocCreateUserCallbackFn_(error);
            return;
        }
        else if(Error::INVALID_USERNAME_PASSWORD != error.value())
        {
            onCreateUserCallbackFn_(std::error_code(Error::USERNAME_ALREADY_EXISTS,
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
            ocCreateUserCallbackFn_(error);
            return;
        }

        LOG_INFO("Create user successfull for userName:"<<userName_<<std::endl);
        onCreateUserCallbackFn_(std::error_code(Error::SUCCESS,Error::ErrorCategory::GetInstance()));
        return;
    }

    void Controller::loginUserImpl(const std::string& userName_,
                                   const std::string& password_,
                                   const ErrorCodeStringCallbackFn& onLoginUserCallbackFn_)
    {
        //verify username-password
        std::optional<SantiagoDBTables::UserProfilesRec> userProfilesRecOpt;
        std::error_code error;
        std::tie(error,userProfilesRecOpt) = verifyUserNamePasswordAndGetUserProfilesRec(userName_,
                                                                                         password_,
                                                                                         userProfilesRecOpt);
        if(error)
        {
            onLoginUserCallbackFn_(error,boost::none);
            return;
        }

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
            onLoginUserCallbackFn_(error,boost::none);
            return;
        }
        
        //update the maps
        bool isInsertionSuccessfulFlag =
            _cookieStringSessionsRecMap.insert(std::make_pair(sessionsRec._cookieId,sessionsRec)).first;
        BOOST_ASSERT(isInsertionSuccessfulFlag);
        _userNameCookieListMap[userName_].push_back(sessionsRec._cookieId);

        onLoginUserCallbackFn_(std::error_code(Error::SUCCESS,
                                               Error::ErrorCategory::GetInstance()),
                               sessionsRec._cookieId);
        return;
    }

    void Controller::verifyCookieAndGetUserNameImpl(const std::string& cookieString_,
                                                    const ErrorCodeStringCallbackFn& onVerifyUserCallbackFn_)
    {
        std::map<std::string,SantiagoDBTable::SessionsRec>::iterator cookieStringSessionsRecMapIter;
        std::error error;
        std::tie(error,cookieStringSessionsRecMapIter) = checkForCookieInMapAndGetSessionsRec(cookieString_);
        if(error)
        {
            onVerifyUserCallbackFn_(error,boost::none);
        }

        //check if the lastActiveTime older than MAX_SESSION_DURATION. If yes then logout
        if((boost::posix_time::second_clock::universal_time() -
            cookieStringSessionsRecMapIter->second._lastActiveTime) >
           boost::posix_time::time_duration(MAX_SESSION_DURATION,0,0,0))
        {

            LOG_INFO("Session lastActiveTime older than MAX_SESSION_DURATION. Going to log out. cookieString:"
                     <<cookieString_<<std::endl);            
            cleanupCookieDataAndUpdateSessionRecord(cookieString_);

            onVerifyUserCallbackFn_(std::error_code(Error::INVALID_SESSION_COOKIE,
                                                    Error::ErrorCategory::GetInstance()),
                                    boost::none);
            return;
        }
        else //set the lastActiveTime to now
        {
            cookieStringSessionsRecMapIter->second._lastActiveTime = boost::posix_time::second_clock::universal_time();
        }

        LOG_INFO("Verify cookie successfull. cookieString:"<<cookieString_<<std::endl);

        onVerifyUserCallbackFn_(std::error_code(Error::SUCCESS,Error::ErrorCategory::GetInstance()),
                                cookieStringSessionsRecMapIter->second._userName);
        return;
    }

    void Controller::logoutUserForCookieImpl(const std::string& cookieString_,
                                             const ErrorCodeCallbackFn& onLogoutCookieCallbackFn_)
    {
        std::error error;
        std::tie(error,std::ignore) = checkForCookieInMapAndGetSessionsRec(cookieString_);
        if(error)
        {
            onLogoutCookieCallbackFn_(error);
        }

        cleanupCookieDataAndUpdateSessionRecord(cookieString_);

        onLogoutUserCallbackFn_(std::error_code(Error::SUCCESS,Error::ErrorCategory::GetInstance()));
        return;        
    }

    void Controller::logoutUserForAllCookiesImpl(const std::string& cookieString_,
                                                 const ErrorCodeCallbackFn& onLogoutAllCookiesCallbackFn_)
    {
        //verify if the cookie is in the cookieStringSessionsRecMap.
        std::map<std::string,SantiagoDBTable::SessionsRec>::iterator cookieStringSessionsRecMapIter;
        std::error error;
        std::tie(error,cookieStringSessionsRecMapIter) = checkForCookieInMapAndGetSessionsRec(cookieString_);
        if(error)
        {
            onVerifyUserCallbackFn_(error);
        }

        cleanupCookieDataAndUpdateSessionRecordsForAllCookies(cookieStringSessionsRecMapIter->second._userName);
        onLogoutAllCookiesCallbackFn_(std::error_code(Error::SUCEESS,Error::ErrorCategory::GetInstance()));
        return;
    }

    void Controller::changeUserPasswordImpl(const UserInfo& cookieString_,
                                            const std::string& oldPassword_,
                                            const std::string& newPassword_,
                                            const ErrorCodeCallbackFn& onChangePasswordCallbackFn_)
    {
        //verify username-password
        std::optional<SantiagoDBTables::UserProfilesRec> userProfilesRecOpt;
        std::error_code error;
        std::tie(error,userProfilesRecOpt) = verifyUserNamePasswordAndGetUserProfilesRec(userName_,password_);
        if(error)
        {
            onChangePasswordCallbackFn_(error);
            return;
        }
        BOOST_ASSERT(userProfilesRecOpt);

        //change and update the password
        userProfilesRecOpt->_password = newPassword_;
        error = _databaseConnection.updateUserProfilesRec(*userProfilesRecOpt);
        if(error)
        {
            LOG_DEBUG("Database Error for updateUserProfilesRec(). userName:"<<userName_<<std::endl);
        }

        onChangePasswordCallbackFn_(error);
        return;
    }

    void Controller::deleteUserImpl(const std::string& cookieString_,
                                    const ErrorCodeCallbackFn& onDeleteUserCallbackFn_)
    {
        //verify if the cookie is in the cookieStringSessionsRecMap.
        std::map<std::string,SantiagoDBTable::SessionsRec>::iterator cookieStringSessionsRecMapIter;
        std::error error;
        std::tie(error,cookieStringSessionsRecMapIter) = checkForCookieInMapAndGetSessionsRec(cookieString_);
        if(error)
        {
            onDeleteUserCallbackFn_(error);
        }

        //delete from db
        std::optional<SantiagoDBTables::UserProfilesRec> userProfilesRecOpt;
        std::error_code error;
        std::tie(error,userProfilesRecOpt) =
            verifyUserNamePasswordAndGetUserProfilesRec(cookieStringSessionsRecMapIter->second._userName,"");
        BOOST_ASSER(userProfilesRecOpt);
        error = _databaseConnection.deleteUserProfilesRec(*userProfilesRecOpt);
        if(error)
        {
            onDeleteCallbackFn(error);
        }
        BOOST_ASSERT(0 == userProfilesRecOpt->_id);

        //remove from memory
        cleanupCookieDataAndUpdateSessionRecordsForAllCookies(cookieStringSessionsRecMapIter->second._userName);
        onDeleteUserCallbackFn_(std::error_code(Error::SUCEESS,Error::ErrorCategory::GetInstance()));
        return;
    }

    std::pair<std::error_code,std::optional<SantiagoDBTables::UserProfilesRec> > 
    Controller::verifyUserNamePasswordAndGetUserProfilesRec(const std::string& userName_,
                                                            const std::string& password_)
    {
        std::optional<SantiagoDBTables::UserProfilesRec> userProfilesRecOpt;
        //verify username-password
        std::error_code error = _databaseConnection.getUserProfilesRec(userName_,userProfilesRecOpt);
        if(error)
        {
            LOG_DEBUG("Database Error for getUserProfilesRec(). userName:"<<userName_<<std::endl);
            BOOST_ASSERT(false);
            return std::make_pair(error,userProfilesRecOpt);
        }

        if(!userProfilesRecOpt || (userProfilesRecOpt->_password != password_))
        {
            LOG_INFO("Wrong username_password. userName:"<<userName_<<std::endl);
            return std::make_pair(std::error_code(Error::INVALID_USERNAME_PASSWORD,
                                                  Error::ErrorCategory::GetInstance()),
                                  userProfilesRecOpt);
        }

        LOG_INFO("Username password verified for userName:"<<userName_<<std::endl);
        return std::make_pair(std::error_code(Error::SUCCESS,Error::ErrorCategory::GetInstance(),userProfilesRecOpt));
    }

    std::pair<std::error_code,std::optional<SantiagoDBTables::SessionsRec> > 
    Controller::checkForCookieInMapAndGetSessionsRecIter(const std::string cookieString_)
    {

        //verify if the cookie is in the cookieStringSessionsRecMap.
        std::map<std::string,SantiagoDBTable::SessionsRec>::iterator cookieStringSessionsRecMapIter =
            _cookieStringSessionsRecMap.find(cookieString_);
        if(cookieStringSessionsRecMapIter == _cookieStringSessionsRecMap.end())
        {
            LOG_INFO("Cookied not in _cookieStringSessionsRecMap. cookieString:"
                     <<cookieString_<<std::endl);
            return std::make_pair(std::error_code(Error::INVALID_SESSION_COOKIE,Error::ErrorCategory::GetInstance()),
                                  boost::none);
        }

        return std::make_pair(std::error_code(Error::SUCCESS,Error::ErrorCategory::GetInstance()),
                              cookieStringSessionsRecMapIter);
    }
    
    void Controller::cleanupCookieDataAndUpdateSessionRecord(const std::string& cookieString_)
    {
        //internal fn. so when this fn is called the cookie should already be verified.
        std::map<std::string,SantiagoDBTable::SessionsRec>::iterator cookieStringSessionsRecMapIter =
            _cookieStringSessionsRecMap.find(cookieString_);
        BOOST_ASSERT(cookieStringSessionsRecMapIter != _cookieStringSessionsRecMap.end());
        cookieStringSessionsRecMapIter->second._logoutTime = boost::posix_time::second_clock::universal_time();
        //update the db
        std::error_code error = _databaseConnection.updateSessionsRec(cookieStringSessionsRecMapIter->second);
        if(error)
        {
            LOG_INFO("updateSessionsRec failed. Logging out without writing to db. SessionsRec:"
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

    void cleanupCookieDataAndUpdateSessionRecordsForAllCookies(const std::string& userName_)
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
