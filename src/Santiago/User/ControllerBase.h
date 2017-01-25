#ifndef SANTIAGO_USER_CONTROLLERBASE_H
#define SANTIAGO_USER_CONTROLLERBASE_H

/**
 * @file ControllerBase.h
 *
 * @section DESCRIPTION
 *
 * Contains the ControllerBase class and UserInfo Structure.
 */

#include <functional>
#include <system_error>

#include <boost/asio.hpp>
#include <boost/property_tree/ptree.hpp>

#include <boost/optional.hpp>

namespace Santiago{ namespace User
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

    class ControllerBase
    {
    public:

        typedef std::function<void(const std::error_code&,const boost::optional<UserInfo>&)>
        ErrorCodeUserInfoCallbackFn;

        typedef std::function<void(const std::error_code&,const boost::optional<std::pair<UserInfo,std::string> >&)>
        ErrorCodeUserInfoStringPairCallbackFn;


        typedef std::function<void(const std::error_code&)> ErrorCodeCallbackFn;

        ControllerBase(const ControllerBase&) = delete;
        ControllerBase& operator=(const ControllerBase&) = delete;
        
        /**
         * The constructor
         * @param ioService_- the ioservice to use
         * @param config_- Used to hold the json data.   
         */
        ControllerBase(boost::asio::io_service& ioService_, const boost::property_tree::ptree& config_):
            _ioService(ioService_),
            _strand(_ioService),
            _config(config_)
        {}

	virtual ~ControllerBase()
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
        /**
         * This function is used for the getting user info details for the given cookie string.
         * @param cookieString_ - Cookie string received from the user.
         * @param onVerifyUserCallbackFn_ - Handler function for the get user info operations.
         */
        virtual void verifyCookieAndGetUserInfo(
            const std::string& cookieString_,
            const ErrorCodeUserInfoCallbackFn& onVerifyUserCallbackFn_) = 0;
         /**
         * This function is used for logging out user account for the  given cookie string.
         * @param cookieString_ - Cookie string received from the user.
         * @param onLogoutCookieCallbackFn_ - Handler function for logout operations.
         */
        virtual void logoutUserForCookie(const std::string& cookieString_,
                                         const ErrorCodeCallbackFn& onLogoutCookieCallbackFn_) = 0;
        /**
         * This function is used for loging out user account for all cookies.
         * @param currentCookieString_ - Cookie string received from the user.
         * @param onLogoutAllCookiesCallbackFn_ - Handler function for the logout operations.
         */
        virtual void logoutUserForAllCookies(const std::string& currentCookieString_,
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

    protected:
        
        boost::asio::io_service         &_ioService;
        boost::asio::strand              _strand;
        boost::property_tree::ptree      _config;

    };

}} 
#endif
