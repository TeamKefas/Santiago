#ifndef SANTIAGO_AUTHENTICATION_AUTHENTICATOR_H
#define SANTIAGO_AUTHENTICATION_AUTHENTICATOR_H

/**
 * @file Authenticator.h
 *
 * @section DESCRIPTION
 *
 * Contains the Authenticator class 
 */

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
//#include "Santiago/ThreadSpecificVar/ThreadSpecificVar.h"
#include "Santiago/Thread/ThreadSpecificVar.h"

#include "Santiago/Authentication/AuthenticatorBaseV1.h"
#include "Santiago/Utils/STLog.h"
#define MAX_SESSION_DURATION 1

namespace Santiago{ namespace Authentication{ namespace SingleNode
{

    class Authenticator:public Authentication::AuthenticatorBase
    {
    public:

        struct UserData
        {
          /**
           * The constructor
           * @param emailAddress_- email address of the user.
           */
            UserData(const std::string& emailAddress_):
                _emailAddress(emailAddress_),
                _cookieList()
            {}

            std::string                  _emailAddress;
            std::vector<std::string>     _cookieList;
        };

        typedef Thread::ThreadSpecificVar<SantiagoDBTables::MariaDBConnection> ThreadSpecificDbConnection;
        /**
         * The constructor
         * @param databaseConnection_- Used for multithreaded operations.  
         * @param ioService_- the ioservice to use.
         * @param config_- Used to hold the json data.
         */
        Authenticator(ThreadSpecificDbConnection& databaseConnection_,
                   boost::asio::io_service& ioService_,
                   const boost::property_tree::ptree& config_);
        /**
         * The destructor
         */
	virtual ~Authenticator()
	{}
        /**
         * This function generates the 
         * @param str - str is the string to be encrypted.
         */
        std::string generateSHA256(const std::string str);
    protected:
       

        //implementation functions to implement in strand
        /**
         * Implementation function for createUser.
         * @param userName_ - username of the user.
         * @param emailAddress_ - Email address of the user.
         * @param password_ - Password received from the user.
         * @param onCreateUserCallbackFn_ - Call back function for createUser operations
         */
        virtual void createUserImpl(const std::string& userName_,
                                    const std::string& emailAddress_,
                                    const std::string& password_,
                                    const ErrorCodeCallbackFn& onCreateUserCallbackFn_);
        /**
         * Implementation function for the loginUser function.
         * @param userNameOrEmailAddress_ - username used by the user for login.
         * @param isUserNameNotEmailAddress_ - true for username and false for email address.
         * @param password_ - Password used by the user for login.
         * @param onCreateUserCallbackFn_ - Call back function for login operations
         */
        virtual void loginUserImpl(const std::string& userNameOrEmailAddress_,
                                   bool isUserNameNotEmailAddress_,
                                   const std::string& password_,
                                   const ErrorCodeUserInfoStringPairCallbackFn& onLoginUserCallbackFn_);
        /**
         * Implementation function for the verifyCookieAndGetUserInfo function.
         * @param cookieString_ - cookieString of the user that is used to get user information.
         * @param onVerifyUserCallbackFn_ - Call back function for verify cookie and get user info operations.
         */
        virtual void verifyCookieAndGetUserInfoImpl(const std::string& cookieString_,
                                                    const ErrorCodeUserInfoCallbackFn& onVerifyUserCallbackFn_);
        /**
         * This function is the implementation function for the logoutUserForCookie function.
         * @param cookieString_ - cookieString of the user that is used for logout operation.
         * @param onLogoutCookieCallbackFn_ - Call back function for logout user for cookie operations
         */
        virtual void logoutUserForCookieImpl(const std::string& cookieString_,
                                             const ErrorCodeCallbackFn& onLogoutCookieCallbackFn_);
        /**
         * This function is the implementation function for the logoutUserForAllCookies function.
         * @param userName_ - username of the user.
         * @param onLogoutAllCookiesCallbackFn_ - Call back function for logout for all cookie operations.
         */
        virtual void logoutUserForAllCookiesImpl(const std::string& userName_,
                                                 const ErrorCodeCallbackFn& onLogoutAllCookiesCallbackFn_);
        /**
         * This function is the implementation function for the changeUserPassword function.
         * @param cookieString_ - Cookie from the user.
         * @param oldPassword_ - Old password of the user.
         * @param newPassword_ - New password of the user.
         * @param onChangePasswordCallbackFn_ - Call back function for password change operations.
         */
        virtual void changeUserPasswordImpl(const std::string& cookieString_,
                                            const std::string& oldPassword_,
                                            const std::string& newPassword_,
                                            const ErrorCodeCallbackFn& onChangePasswordCallbackFn_);
        /**
         * This function is the implementation function for the getUserForEmailAddressAndRecoveryString.
         * @param emailAddress_ - Email address  received from the user.
         * @param onGetUserForEmailAddressAndRecoveryStringCallbackFn_ -Call back function for get user for email address operations.
         */
        virtual void getUserForEmailAddressAndRecoveryStringImpl(const std::string& emailAddress_,
                                                                 const std::string& recoveryString_,
                                                                 const ErrorCodeStringCallbackFn& onGetUserForEmailAddressAndRecoveryStringCallbackFn_);
        /**
         * This function is the implementation function for the changeUserPasswordForEmailAddressAndRecoveryString function.
         * @param emailAddress_ - Email address from the user.
         * @param recoveryString_ - Recovery string from the user.
         * @param newPassword_ - New password of the user.
         * @param onChangePasswordForEmailAddressAndRecoveryStringCallbackFn_ - Call back function for password change for email address and recovery string operations.
         */
        virtual void changeUserPasswordForEmailAddressAndRecoveryStringImpl(const std::string& emailAddress_,
                                                                            const std::string& recoverystring_,
                                                                            const std::string& newPassword_,
                                                                            const ErrorCodeCallbackFn& onChangePasswordForEmailAddressAndRecoveryStringCallbackFn_);
        /**
         * This function is the implementation function for the changeUserEmailAddress function. 
         * @param cookieString_ - Cookie from the user.
         * @param newEmailAddress_ - New email address that is to be updated.
         * @param password_ - Password of the user account.
         * @param onChangeEmailAddressCallbackFn_ - Call back operations for email change operations.
         */
        virtual void changeUserEmailAddressImpl(const std::string& cookieString_,
                                                const std::string& newEmailAddress_,
                                                const std::string& password_,
                                                const ErrorCodeCallbackFn& onChangeEmailAddressCallbackFn_);
        /**
         *  This function is the implementation function for the createAndReturnRecoveryString function.
         * @param emailAddress_ - Email Address from the user.
         * @param onCreateAndReturnRecoveryStringCallbackFn_ - Call back function for create ang return recovery string operations.
         */
        virtual void createAndReturnRecoveryStringImpl(const std::string& emailAddress_,
                                                       const ErrorCodeStringCallbackFn& onCreateAndReturnRecoveryStringCallbackFn_);
        /**
         *  This function is the implementation function for the deleteUser function.
         * @param cookieString_ - Cookie from the user.
         * @param onDeleteUserCallbackFn_ - Call back function for delete user operations.
         */
        virtual void deleteUserImpl(const std::string& cookieString_,
                                    const ErrorCodeCallbackFn& onDeleteUserCallbackFn_);

        //helper fns
        /**
         *  This function verifies user credentials based on the userName_ and password_ and returns the pair of error code and user record.  
         * @param userName_ - Username from the user.
         * @param password_ - Password from the user.
         * @returns This function returns a std pair of error code and UserRecord if  error is true otherwise error code alone is send.
         */
        std::pair<std::error_code,boost::optional<SantiagoDBTables::UsersRec> > 
        verifyUserNamePasswordAndGetUsersRec(const std::string& userName_, const std::string& password_);
        //helper fns
        /**
         * This function verifies user credentials based on the emailAddress_ and password_ and returns the pair of error code and user record. 
         * @param userName_ - Email address from the user.
         * @param password_ - Password from the user.
         * @returns This function returns a std pair of error code and UserRecord if  error is true otherwise error code alone is send.
         */
        std::pair<std::error_code,boost::optional<SantiagoDBTables::UsersRec> > 
        verifyEmailAddressPasswordAndGetUsersRec(const std::string& emailAddress_, const std::string& password_);
         //helper fns
        /**
         * This function verifies user credentials based on the emailAddress_ and recoverystring_ and returns the pair of error code and user record. 
         * @param userName_ - Email address from the user.
         * @param recoveryString - Recovery string from the user.
         * @returns This function returns a std pair of error code and UserRecord if  error is true otherwise error code alone is send.
         */
        std::pair<std::error_code,boost::optional<SantiagoDBTables::UsersRec> > 
        verifyEmailAddressRecoveryStringAndGetUsersRec(const std::string& emailAddress_, const std::string& recoveryString_);
        //helper fns
        /**
         * It checks for CookieString_ in map and returns pair of error code and map iterator.
         * @param cookieString_ - Cookie string from the user.
         * @return Returns a pair of error code and a map iterator of cookieString_ and Sessions Record 
         */
        std::pair<std::error_code,std::map<std::string,Santiago::SantiagoDBTables::SessionsRec>::iterator > 
        checkForCookieInMapAndGetSessionsRecIter(const std::string& cookieString_);
        //helper fns
        /**
         * Clean up Cookie based up on the cookieString_ that user given and updates the Session record.
         * @param cookieString_ - Cookie string from the user.
         */
        void cleanupCookieDataAndUpdateSessionRecord(const std::string& cookieString_);
        //helper fns
        /**
         * Clean up all cookie using the username given by the user.
         * @param userName_ - Username of the user.
         */
        void cleanupCookieDataAndUpdateSessionRecordsForAllCookies(const std::string& userName_);
        //helper fns
        /**
         * Generates the unique cookie string.
         * @return Returns the unique cookie string.
         */
        std::string generateUniqueCookie();
        std::string generateUniqueCookieForUserName(std::string userName_);
        std::string generateRecoveryString();
        
        ThreadSpecificDbConnection                           &_databaseConnection;

        // std::map<std::string,SantiagoDBTables::SessionsRec>   _cookieStringSessionsRecMap;
        // std::map<std::string,UserData>                        _userNameUserDataMap;

        struct AuthenticationData
        {
            std::map<std::string,SantiagoDBTables::SessionsRec>   _cookieStringSessionsRecMap;
            std::map<std::string,UserData>                        _userNameUserDataMap;
        };

        std::array<AuthenticationData,26>                         _authenticationDataArray;

        int getAlphabetPosition(char alphabet);
             
    };

}}}

#endif
