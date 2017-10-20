#ifndef SANTIAGO_AUTHENTICATION_AUTHENTICATORBASEV1_H
#define SANTIAGO_AUTHENTICATION_AUTHENTICATORBASEV1_H

/**
 * @file AuthenticatorBase.h
 *
 * @section DESCRIPTION
 *
 * Contains the AuthenticatorBase class and UserInfo Structure.
 */

#include <functional>
#include <system_error>

#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/property_tree/ptree.hpp>

#include <boost/optional.hpp>

#include "AuthenticatorImplBase.h"
#include "ControllerDataBase.h"

namespace Santiago{ namespace Authentication
{
    
    class AuthenticatorBase
    {
    public:

        typedef std::function<void(const std::error_code&,const boost::optional<UserInfo>&)>
        ErrorCodeUserInfoCallbackFn;
	
	typedef std::function<void(const std::error_code&,const boost::optional<std::string>&)>
        ErrorCodeStringCallbackFn;

        typedef std::function<void(const std::error_code&,const boost::optional<std::pair<UserInfo,std::string> >&)>
        ErrorCodeUserInfoStringPairCallbackFn;

        typedef std::shared_ptr<AuthenticatorImplBase> AuthenticatorImplBasePtr;
        typedef std::shared_ptr<boost::asio::strand> StrandPtr;

        typedef std::function<void(const std::error_code&)> ErrorCodeCallbackFn;

        AuthenticatorBase(const AuthenticatorBase&) = delete;
        AuthenticatorBase& operator=(const AuthenticatorBase&) = delete;
        
        /**
         * The constructor
         * @param ioService_- the ioservice to use
         * @param config_- Used to hold the json data.   
         */
        AuthenticatorBase(boost::asio::io_service& ioService_,
                          const boost::property_tree::ptree& config_):
            _ioService(ioService_),
            _config(config_)
        {}

	virtual ~AuthenticatorBase()
	{}

        /**
         * This function is used for the getting user info details for the given cookie string.
         * @param cookieString_ - Cookie string received from the user.
         * @param onVerifyUserCallbackFn_ - Handler function for the get user info operations.
         */
        virtual void verifyCookieAndGetUserInfo(
            const std::string& cookieString_,
            const ErrorCodeUserInfoCallbackFn& onVerifyUserCallbackFn_);
        
        boost::optional<UserInfo> verifyCookieAndGetUserInfo(
            const std::string& cookieString_,
            boost::asio::yield_context yield_,
            std::error_code& error_);

        /**
         * This function is used to create a new user using the user credentials given by the user.
         * @param userName_ - User name provided by the user.
         * @param emailAddress_ - Email address provided by the user. 
         * @param password_ - Password provided by the user.
         * @param onCreateUserCallbackFn_ - Handler function provided for create user operations.
         */
        virtual void createUser(const std::string& userName_,
                                const std::string& emailAddress_,
                                const std::string& password_,
                                const ErrorCodeCallbackFn& onCreateUserCallbackFn_);
        
        void createUser(const std::string& userName_,
                        const std::string& emailAddress_,
                        const std::string& password_,
                        boost::asio::yield_context yield_,
                        std::error_code& error_);
        
        
        /**
         * ///Message\\
         * @param userNameOrEmailAddress_ - Username/Email address used by the user for login.
         * @param isUserNameNotEmailAddress_ -  true for username and false for email address.
         * @param password_ - Password recevied from the user.
         * @param onLoginUserCallbackFn_ - Call back function for Login operations.
         */
        virtual void loginUser(const std::string& userNameOrEmailAddress_,
                               bool isUserNameNotEmailAddress_,
                               const std::string& password_,
                               const ErrorCodeUserInfoStringPairCallbackFn& onLoginUserCallbackFn_);

        boost::optional<std::pair<UserInfo,std::string> > loginUser(const std::string& userNameOrEmailAddress_,
                                                                    bool isUserNameNotEmailAddress_,
                                                                    const std::string& password_,
                                                                    boost::asio::yield_context yield_,
                                                                    std::error_code& error_);
        
        /**
         * This function is used for logging out user account for the  given cookie string.
         * @param cookieString_ - Cookie string received from the user.
         * @param onLogoutCookieCallbackFn_ - Handler function for logout operations.
         */
        virtual void logoutUserForCookie(const std::string& cookieString_,
                                         const ErrorCodeCallbackFn& onLogoutCookieCallbackFn_);
        
        void logoutUserForCookie(const std::string& cookieString_,
                                 boost::asio::yield_context yield_,
                                 std::error_code& error_);
        /**
         * This function is used for loging out user account for all cookies.
         * @param userName_ - username of the user.
         * @param onLogoutAllCookiesCallbackFn_ - Handler function for the logout operations.
         */
        virtual void logoutUserForAllCookies(const std::string& userName_,
                                             const ErrorCodeCallbackFn& onLogoutAllCookiesCallbackFn_);

        void logoutUserForAllCookies(const std::string& userName_,
                                     boost::asio::yield_context yield_,
                                     std::error_code& error_);
         /**
         * This function is used for updating the user password using the new password provided by the user.
         * @param cookieString_ -Cookie string received from the user.
         * @param oldPassword_ - Old password of the user account.
         * @param newPassword_ - New password for the user account.
         * @param onChangePasswordCallbackFn_ - Handler function for the password change operations.
         */
        virtual void changeUserPassword(const std::string& cookieString_,
                                        const std::string& oldPassword_,
                                        const std::string& newPassword_,
                                        const ErrorCodeCallbackFn& onChangePasswordCallbackFn_);
        
        void changeUserPassword(const std::string& cookieString_,
                                const std::string& oldPassword_,
                                const std::string& newPassword_,
                                boost::asio::yield_context yield_,
                                std::error_code& error_  );
         /**
         * This function is used to update the email address using the email address provided by the user.
         * @param - cookieString_ - Cookie string received from the user.
         * @param - newEmailAddress_ - Email address received from the user.
         * @param - password_ - Password from the user.
         * @param - onChangeEmailAddressCallbackFn_ - Handler function for email change operations.
         */
        virtual void changeUserEmailAddress(const std::string& cookieString_,
                                            const std::string& newEmailAddress_,
                                            const std::string& password_,
                                            const ErrorCodeCallbackFn& onChangeEmailAddressCallbackFn_);
        
        void changeUserEmailAddress(const std::string& cookieString_,
                                    const std::string& newEmailAddress_,
                                    const std::string& password_,
                                    boost::asio::yield_context yield_,
                                    std::error_code& error_);

         /**
         * This function is used to create and return a recovery string for the  user using the given email address and calls the call back function onCreateAndReturnRecoveryStringCallbackFn.
         * @param emailAddress_ - EmailAddress received from the user.
         * @param onCreateAndReturnRecoveryStringCallbackFn_ - Handler function for create and return recovery string operations.
         */
        virtual void createAndReturnRecoveryString(const std::string& emailAddress_,
                                                   const ErrorCodeStringCallbackFn& onCreateAndReturnRecoveryStringCallbackFn_);
        boost::optional<std::string> createAndReturnRecoveryString(const std::string& emailAddress_,
                                                                   boost::asio::yield_context yield_,
                                                                   std::error_code& error_);
        /**
         * This function is used to get user for the given email adress and calls the call back function onGetUserForEmailAddressAndRecoveryStringCallbackFn.
         * @param emailAddress_ - Email address  received from the user.
         * @param onGetUserForEmailAddressAndRecoveryStringCallbackFn_ -Handler function for delete operations.
         */
        virtual void getUserForEmailAddressAndRecoveryString(const std::string& emailAddress_,
                                                             const std::string& recoveryString_,
                                                             const ErrorCodeStringCallbackFn& onGetUserForEmailAddressAndRecoveryStringCallbackFn_);
        
        boost::optional<std::string> getUserForEmailAddressAndRecoveryString(const std::string& emailAddress_,
                                                                             const std::string& recoveryString_,
                                                                             boost::asio::yield_context yield_,
                                                                             std::error_code& error_);  
        
       

        
        /**
         * This function is used for updating the user password using the emailaddress and recovery string provided by the user.
         * @param emailAddress_ - Email Address received from the user.
         * @param recoveryString_ - Recovery string received from the user.
         * @param onChangePasswordForEmailAddressAndRecoveryStringCallbackFn_ - Handler function for the password change using email address and recovery string operations.
         */
        virtual void changeUserPasswordForEmailAddressAndRecoveryString(const std::string& emailAddress_,
                                                                        const std::string& recoveryString_,
                                                                        const std::string& newPassword_,
                                                                        const ErrorCodeCallbackFn& onChangePasswordForEmailAddressAndRecoveryStringCallbackFn_);
        void changeUserPasswordForEmailAddressAndRecoveryString(const std::string& emailAddress_,
                                                                const std::string& recoveryString_,
                                                                const std::string& newPassword_,
                                                                boost::asio::yield_context yield_,
                                                                std::error_code& error_);
        /**
         * This function is used to delete a user using the given cookie string and calls the call back function onDeleteUserCallbackFn.
         * @param cookieString_ - Cookie string received from the user.
         * @param onDeleteUserCallbackFn_ - Handler function for delete operations.
         */
        virtual void deleteUser(const std::string& cookieString_,
                                const ErrorCodeCallbackFn& onDeleteUserCallbackFn_);

	void deleteUser(const std::string& cookieString_,
			boost::asio::yield_context yield_,
                        std::error_code& error_);

    protected:

        virtual std::pair<AuthenticatorImplBasePtr,StrandPtr> getAuthenticatorAndStrandForString(
            const std::string& string_,
            bool isNotEmailAddress_) = 0;

         /**
         * This function is used for calling call back functions.
         * @param errorCodeUserInfoCallbackFn_ - Call back function for error code and userinfo.
         * @param error_ - Error code for representing errors.
         * @param userInfoOpt_ - For holding userInfo details.
         */
        void postCallbackFn(const ErrorCodeUserInfoCallbackFn& errorCodeUserInfoCallbackFn_,
                            const std::error_code& error_,
                            const boost::optional<UserInfo>& userInfoOpt_);
        /**
         * This function is used for calling call back functions
         * @param errorCodeUserInfoStringPairCallbackFn_ - Call back function for error code and userinfo.
         * @param error_ - Error code for representing errors.
         * @param userInfoStringPair_ - For holding userInfo details.
         */
        void postCallbackFn(const ErrorCodeUserInfoStringPairCallbackFn& errorCodeUserInfoStringPairCallbackFn_,
                            const std::error_code& error_,
                            const boost::optional<std::pair<UserInfo,std::string> >& userInfoStringPair_);
        /**
         * This function is used for calling call back functions
         * @param errorCodeCallbackFn_ - Call back function 
         * @param error_ - Error code for representing errors.
         */
        void postCallbackFn(const ErrorCodeCallbackFn& errorCodeCallbackFn_,const std::error_code& error_);
        
         /**
         * This function is used for calling call back functions
         * @param errorCodeStringCallbackFn_ - Call back function for error code and recoverySrting.
         * @param error_ - Error code for representing errors.
         * @param recoveryString_ - For holding password recovery string.
         */
        void postCallbackFn(const ErrorCodeStringCallbackFn& errorCodeStringCallbackFn_,
                            const std::error_code& error_,
                            const boost::optional<std::string>& recoveryString_);

        
        boost::asio::io_service         &_ioService;
        boost::property_tree::ptree      _config;
//        ClientIdType                     _clientId;

    };

}} 

#endif
