#ifndef SANTIAGO_USER_CONTROLLER_H
#define SANTIAGO_USER_CONTROLLER_H

#include <functional>
#include <map>
#include <system_error>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <sstream>

#include <openssl/sha.h>

#include "Santiago/SantiagoDBTables/MariaDBConnection.h"
#include "Santiago/ThreadSpecificVar/ThreadSpecificVar.h"

#include "Santiago/User/ControllerBase.h"
#include "Santiago/Utils/STLog.h"
#define MAX_SESSION_DURATION 1

namespace Santiago{ namespace User{ namespace SingleNode
{

    class Controller:public User::ControllerBase
    {
    public:

        struct UserData
        {
            UserData(const std::string& emailAddress_):
                _emailAddress(emailAddress_),
                _cookieList()
            {}

            std::string                  _emailAddress;
            std::vector<std::string>     _cookieList;
        };

        typedef ThreadSpecificVar::ThreadSpecificVar<SantiagoDBTables::MariaDBConnection> ThreadSpecificDbConnection;

        Controller(ThreadSpecificDbConnection& databaseConnection_,
                   boost::asio::io_service& ioService_,
                   const boost::property_tree::ptree& config_);
	virtual ~Controller()
	{}

        std::string generateSHA256(const std::string str);
        
        virtual void createUser(const std::string& userName_,
                                const std::string& emailAddress_,
                                const std::string& password_,
                                const ErrorCodeCallbackFn& onCreateUserCallbackFn_);

        virtual void loginUser(const std::string& userNameOrEmailAddress_,
                               bool isUserNameNotEmailAddress_,
                               const std::string& password_,
                               const ErrorCodeUserInfoStringPairCallbackFn& onLoginUserCallbackFn_);

        virtual void verifyCookieAndGetUserInfo(const std::string& cookieString_,
                                                const ErrorCodeUserInfoCallbackFn& onVerifyUserCallbackFn_);

        virtual void logoutUserForCookie(const std::string& cookieString_,
                                         const ErrorCodeCallbackFn& onLogoutCookieCallbackFn_);

        virtual void logoutUserForAllCookies(const std::string& currentCookieString_,
                                             const ErrorCodeCallbackFn& onLogoutAllCookiesCallbackFn_);

        virtual void changeUserPassword(const std::string& cookieString_,
                                        const std::string& oldPassword_,
                                        const std::string& newPassword_,
                                        const ErrorCodeCallbackFn& onChangePasswordCallbackFn_);

        virtual void changeUserEmailAddress(const std::string& cookieString_,
                                            const std::string& newEmailAddress_,
                                            const std::string& password_,
                                            const ErrorCodeCallbackFn& onChangeEmailAddressCallbackFn_);

        virtual void deleteUser(const std::string& cookieString_,
                                const ErrorCodeCallbackFn& onDeleteUserCallbackFn_);
        

    protected:

        void postCallbackFn(const ErrorCodeUserInfoCallbackFn& errorCodeUserInfoCallbackFn_,
                            const std::error_code& error_,
                            const boost::optional<UserInfo>& userInfoOpt_);

        void postCallbackFn(const ErrorCodeUserInfoStringPairCallbackFn& errorCodeUserInfoStringPairCallbackFn_,
                            const std::error_code& error_,
                            const boost::optional<std::pair<UserInfo,std::string> >& userInfoStringPair_);

        void postCallbackFn(const ErrorCodeCallbackFn& errorCodeCallbackFn_,const std::error_code& error_);

        //implementation functions to implement in strand
        virtual void createUserImpl(const std::string& userName_,
                                    const std::string& emailAddress_,
                                    const std::string& password_,
                                    const ErrorCodeCallbackFn& onCreateUserCallbackFn_);

        virtual void loginUserImpl(const std::string& userNameOrEmailAddress_,
                                   bool isUserNameNotEmailAddress_,
                                   const std::string& password_,
                                   const ErrorCodeUserInfoStringPairCallbackFn& onLoginUserCallbackFn_);

        virtual void verifyCookieAndGetUserInfoImpl(const std::string& cookieString_,
                                                    const ErrorCodeUserInfoCallbackFn& onVerifyUserCallbackFn_);

        virtual void logoutUserForCookieImpl(const std::string& cookieString_,
                                             const ErrorCodeCallbackFn& onLogoutCookieCallbackFn_);

        virtual void logoutUserForAllCookiesImpl(const std::string& currentCookieString_,
                                                 const ErrorCodeCallbackFn& onLogoutAllCookiesCallbackFn_);

        virtual void changeUserPasswordImpl(const std::string& cookieString_,
                                            const std::string& oldPassword_,
                                            const std::string& newPassword_,
                                            const ErrorCodeCallbackFn& onChangePasswordCallbackFn_);

        virtual void changeUserEmailAddressImpl(const std::string& cookieString_,
                                                const std::string& newEmailAddress_,
                                                const std::string& password_,
                                                const ErrorCodeCallbackFn& onChangeEmailAddressCallbackFn_);

        virtual void deleteUserImpl(const std::string& cookieString_,
                                    const ErrorCodeCallbackFn& onDeleteUserCallbackFn_);

        //helper fns
        std::pair<std::error_code,boost::optional<SantiagoDBTables::UsersRec> > 
        verifyUserNamePasswordAndGetUsersRec(const std::string& userName_, const std::string& password_);
        std::pair<std::error_code,boost::optional<SantiagoDBTables::UsersRec> > 
        verifyEmailAddressPasswordAndGetUsersRec(const std::string& userName_, const std::string& password_);

        std::pair<std::error_code,std::map<std::string,Santiago::SantiagoDBTables::SessionsRec>::iterator > 
        checkForCookieInMapAndGetSessionsRecIter(const std::string& cookieString_);

        void cleanupCookieDataAndUpdateSessionRecord(const std::string& cookieString_);
        void cleanupCookieDataAndUpdateSessionRecordsForAllCookies(const std::string& userName_);
        std::string generateUniqueCookie();

        ThreadSpecificDbConnection                           &_databaseConnection;

        std::map<std::string,SantiagoDBTables::SessionsRec>   _cookieStringSessionsRecMap;
        std::map<std::string,UserData>                        _userNameUserDataMap;
        
    };

}}}

#endif
