#ifndef SANTIAGO_AUTHENTICATION_AUTHENTICATORBASE_H
#define SANTIAGO_AUTHENTICATION_AUTHENTICATORBASE_H

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

namespace Santiago{ namespace Authentication
{
    struct UserInfo
    {
        /**
         * The constructor
         * @param userName_- Username from the user. 
         * @param emailAddress_- Email address from the user.
         */
        UserInfo(const std::string& userName_,const std::string& emailAddress_):
            _userName(userName_),
            _emailAddress(emailAddress_)
        {}

        std::string  _userName;
        std::string  _emailAddress;
    };

    class AuthenticatorBase
    {
    public:

        typedef std::function<void(const std::error_code&,const boost::optional<UserInfo>&)>
        ErrorCodeUserInfoCallbackFn;
	
	typedef std::function<void(const std::error_code&,const boost::optional<std::string>&)>
        ErrorCodeStringCallbackFn;

        typedef std::function<void(const std::error_code&,const boost::optional<std::pair<UserInfo,std::string> >&)>
        ErrorCodeUserInfoStringPairCallbackFn;


        typedef std::function<void(const std::error_code&)> ErrorCodeCallbackFn;

        AuthenticatorBase(const AuthenticatorBase&) = delete;
        AuthenticatorBase& operator=(const AuthenticatorBase&) = delete;
        
        /**
         * The constructor
         * @param ioService_- the ioservice to use
         * @param config_- Used to hold the json data.   
         */
        AuthenticatorBase(boost::asio::io_service& ioService_, const boost::property_tree::ptree& config_):
            _ioService(ioService_),
            _strand(_ioService),
            _config(config_)
        {}

	virtual ~AuthenticatorBase()
	{}

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
                                const ErrorCodeCallbackFn& onCreateUserCallbackFn_) = 0;
        
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
                               const ErrorCodeUserInfoStringPairCallbackFn& onLoginUserCallbackFn_) = 0;

        boost::optional<std::pair<UserInfo,std::string> > asyncLoginUser(const std::string& userNameOrEmailAddress_,
                                                                         bool isUserNameNotEmailAddress_,
                                                                         const std::string& password_,
                                                                         boost::asio::yield_context yield_,
                                                                         std::error_code& error_);

                
        /**
         * This function is used for the getting user info details for the given cookie string.
         * @param cookieString_ - Cookie string received from the user.
         * @param onVerifyUserCallbackFn_ - Handler function for the get user info operations.
         */
        virtual void verifyCookieAndGetUserInfo(
            const std::string& cookieString_,
            const ErrorCodeUserInfoCallbackFn& onVerifyUserCallbackFn_) = 0;

        boost::optional<UserInfo> verifyCookieAndGetUserInfo(
            const std::string& cookieString_,
            boost::asio::yield_context yield_,
            std::error_code& error_);

        /**
         * This function is used for logging out user account for the  given cookie string.
         * @param cookieString_ - Cookie string received from the user.
         * @param onLogoutCookieCallbackFn_ - Handler function for logout operations.
         */
        virtual void logoutUserForCookie(const std::string& cookieString_,
                                         const ErrorCodeCallbackFn& onLogoutCookieCallbackFn_) = 0;
        /**
         * This function is used for loging out user account for all cookies.
         * @param userName_ - username of the user.
         * @param onLogoutAllCookiesCallbackFn_ - Handler function for the logout operations.
         */
        virtual void logoutUserForAllCookies(const std::string& userName_,
                                             const ErrorCodeCallbackFn& onLogoutAllCookiesCallbackFn_) = 0;
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
                                        const ErrorCodeCallbackFn& onChangePasswordCallbackFn_) = 0;
        /**
         * This function is used to get user for the given email adress and calls the call back function onGetUserForEmailAddressAndRecoveryStringCallbackFn.
         * @param emailAddress_ - Email address  received from the user.
         * @param onGetUserForEmailAddressAndRecoveryStringCallbackFn_ -Handler function for delete operations.
         */
        virtual void getUserForEmailAddressAndRecoveryString(const std::string& emailAddress_,
                                                             const std::string& recoverystring_,
                                                             const ErrorCodeStringCallbackFn& onGetUserForEmailAddressAndRecoveryStringCallbackFn_) = 0;
        
        /**
         * This function is used to create and return a recovery string for the  user using the given email address and calls the call back function onCreateAndReturnRecoveryStringCallbackFn.
         * @param emailAddress_ - EmailAddress received from the user.
         * @param onCreateAndReturnRecoveryStringCallbackFn_ - Handler function for create and return recovery string operations.
         */
        virtual void createAndReturnRecoveryString(const std::string& emailAddress_,
                                                   const ErrorCodeStringCallbackFn& onCreateAndReturnRecoveryStringCallbackFn_) = 0;

        
        /**
         * This function is used for updating the user password using the emailaddress and recovery string provided by the user.
         * @param emailAddress_ - Email Address received from the user.
         * @param recoveryString_ - Recovery string received from the user.
         * @param onChangePasswordForEmailAddressAndRecoveryStringCallbackFn_ - Handler function for the password change using email address and recovery string operations.
         */
        virtual void changeUserPasswordForEmailAddressAndRecoveryString(const std::string& emailAddress_,
                                                                        const std::string& recoveryString_,
                                                                        const std::string& newPassword_,
                                                                        const ErrorCodeCallbackFn& onChangePasswordForEmailAddressAndRecoveryStringCallbackFn_) = 0;
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
                                            const ErrorCodeCallbackFn& onChangeEmailAddressCallbackFn_) = 0;
        /**
         * This function is used to delete a user using the given cookie string and calls the call back function onDeleteUserCallbackFn.
         * @param cookieString_ - Cookie string received from the user.
         * @param onDeleteUserCallbackFn_ - Handler function for delete operations.
         */
        virtual void deleteUser(const std::string& cookieString_,
                                const ErrorCodeCallbackFn& onDeleteUserCallbackFn_) = 0;

        //implementation functions
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
                                    const ErrorCodeCallbackFn& onCreateUserCallbackFn_) = 0;
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
                                   const ErrorCodeUserInfoStringPairCallbackFn& onLoginUserCallbackFn_) = 0;
        /**
         * Implementation function for the verifyCookieAndGetUserInfo function.
         * @param cookieString_ - cookieString of the user that is used to get user information.
         * @param onVerifyUserCallbackFn_ - Call back function for verify cookie and get user info operations.
         */
        virtual void verifyCookieAndGetUserInfoImpl(const std::string& cookieString_,
                                                    const ErrorCodeUserInfoCallbackFn& onVerifyUserCallbackFn_) = 0;
        /**
         * This function is the implementation function for the logoutUserForCookie function.
         * @param cookieString_ - cookieString of the user that is used for logout operation.
         * @param onLogoutCookieCallbackFn_ - Call back function for logout user for cookie operations
         */
        virtual void logoutUserForCookieImpl(const std::string& cookieString_,
                                             const ErrorCodeCallbackFn& onLogoutCookieCallbackFn_) = 0;
        /**
         * This function is the implementation function for the logoutUserForAllCookies function.
         * @param userName_ - username of the user.
         * @param onLogoutAllCookiesCallbackFn_ - Call back function for logout for all cookie operations.
         */
        virtual void logoutUserForAllCookiesImpl(const std::string& userName_,
                                                 const ErrorCodeCallbackFn& onLogoutAllCookiesCallbackFn_) = 0;
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
                                            const ErrorCodeCallbackFn& onChangePasswordCallbackFn_) = 0;
        /**
         * This function is the implementation function for the getUserForEmailAddressAndRecoveryString.
         * @param emailAddress_ - Email address  received from the user.
         * @param onGetUserForEmailAddressAndRecoveryStringCallbackFn_ -Call back function for get user for email address operations.
         */
        virtual void getUserForEmailAddressAndRecoveryStringImpl(const std::string& emailAddress_,
                                                                 const std::string& recoveryString_,
                                                                 const ErrorCodeStringCallbackFn& onGetUserForEmailAddressAndRecoveryStringCallbackFn_) = 0;
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
                                                                            const ErrorCodeCallbackFn& onChangePasswordForEmailAddressAndRecoveryStringCallbackFn_) = 0;
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
                                                const ErrorCodeCallbackFn& onChangeEmailAddressCallbackFn_) = 0;
        /**
         *  This function is the implementation function for the createAndReturnRecoveryString function.
         * @param emailAddress_ - Email Address from the user.
         * @param onCreateAndReturnRecoveryStringCallbackFn_ - Call back function for create ang return recovery string operations.
         */
        virtual void createAndReturnRecoveryStringImpl(const std::string& emailAddress_,
                                                       const ErrorCodeStringCallbackFn& onCreateAndReturnRecoveryStringCallbackFn_) = 0;
        /**
         *  This function is the implementation function for the deleteUser function.
         * @param cookieString_ - Cookie from the user.
         * @param onDeleteUserCallbackFn_ - Call back function for delete user operations.
         */
        virtual void deleteUserImpl(const std::string& cookieString_,
                                    const ErrorCodeCallbackFn& onDeleteUserCallbackFn_) = 0;

    protected:
        
        boost::asio::io_service         &_ioService;
        boost::asio::strand              _strand;
        boost::property_tree::ptree      _config;

    };

}} 
#endif
