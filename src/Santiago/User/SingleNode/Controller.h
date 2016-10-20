#ifndef SANTIAGO_USER_CONTROLLER_H
#define SANTIAGO_USER_CONTROLLER_H

#include <functional>
#include <map>
#include <system_error>
#include <vector>

#include "Santiago/SantiagoDBTables/MariaDBConnectionV1.h"

#include "Santiago/User/ControllerBase.h"
#include "Santiago/Utils/STLog.h"
#define MAX_SESSION_DURATION 1

namespace Santiago{ namespace User{ namespace SingleNode
{

    class Controller:public User::ControllerBase
    {
    public:

        Controller(SantiagoDBTables::MariaDBConnection& databaseConnection_,
                   boost::asio::io_service& ioService_,
                   const boost::property_tree::ptree& config_);

        virtual void createUserImpl(const std::string& userName_,
                                    const std::string& password_,
                                    const ErrorCodeCallbackFn& onCreateUserCallbackFn_);

        virtual void loginUserImpl(const std::string& userName_,
                                   const std::string& password_,
                                   const ErrorCodeStringCallbackFn& onLoginUserCallbackFn_);

        virtual void verifyCookieAndGetUserNameImpl(const std::string& cookieString_,
                                                    const ErrorCodeStringCallbackFn& onVerifyUserCallbackFn_);

        virtual void logoutUserForCookieImpl(const std::string& cookieString_,
                                             const ErrorCodeCallbackFn& onLogoutCookieCallbackFn_);

        virtual void logoutUserForAllCookiesImpl(const std::string& currentCookieString_,
                                                 const ErrorCodeCallbackFn& onLogoutAllCookiesCallbackFn_);

        virtual void changeUserPasswordImpl(const std::string& cookieString_,
                                            const std::string& oldPassword_,
                                            const std::string& newPassword_,
                                            const ErrorCodeCallbackFn& onChangePasswordCallbackFn_);

        virtual void deleteUserImpl(const std::string& cookieString_,
                                    const ErrorCodeCallbackFn& onDeleteUserCallbackFn_);

    protected:

        //helper fns
        std::pair<std::error_code,boost::optional<SantiagoDBTables::UserProfilesRec> > 
        verifyUserNamePasswordAndGetUserProfilesRec(const std::string& userName_, const std::string& password_);
        std::pair<std::error_code,std::map<std::string,Santiago::SantiagoDBTables::SessionsRec>::iterator > 
        checkForCookieInMapAndGetSessionsRecIter(const std::string& cookieString_);

        void cleanupCookieDataAndUpdateSessionRecord(const std::string& cookieString_);
        void cleanupCookieDataAndUpdateSessionRecordsForAllCookies(const std::string& userName_);

        SantiagoDBTables::MariaDBConnection                  &_databaseConnection;

        std::map<std::string,SantiagoDBTables::SessionsRec>   _cookieStringSessionsRecMap;
        std::map<std::string,std::vector<std::string> >       _userNameCookieListMap;
        
    };

}}}

#endif
