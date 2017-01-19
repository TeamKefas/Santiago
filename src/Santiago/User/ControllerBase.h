#ifndef SANTIAGO_USER_CONTROLLERBASE_H
#define SANTIAGO_USER_CONTROLLERBASE_H

/**
 * @file ControllerBase.h
 *
 * @section DESCRIPTION
 *
 * Contains the ControllerBase class 
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
         * @param userName_- 
         * @param configemailAddress_- ///NEED TO WRITE\\\
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
         * @param config_- ///NEED TO WRITE\\\
         */
        ControllerBase(boost::asio::io_service& ioService_, const boost::property_tree::ptree& config_):
            _ioService(ioService_),
            _strand(_ioService),
            _config(config_)
        {}

	virtual ~ControllerBase()
	{}

        /**
         * ///Message\\
         * @param userName_ -
         * @param emailAddress_ -
         * @param password_ -
         * @param onCreateUserCallbackFn_ -
         */
        virtual void createUser(const std::string& userName_,
                                const std::string& emailAddress_,
                                const std::string& password_,
                                const ErrorCodeCallbackFn& onCreateUserCallbackFn_) = 0;
        
        /**
         * ///Message\\
         * @param userNameOrEmailAddress_ -
         * @param isUserNameNotEmailAddress_ -
         * @param password_ -
         * @param onLoginUserCallbackFn_ -
         */
        virtual void loginUser(const std::string& userNameOrEmailAddress_,
                               bool isUserNameNotEmailAddress_,
                               const std::string& password_,
                               const ErrorCodeUserInfoStringPairCallbackFn& onLoginUserCallbackFn_) = 0;
        /**
         * ///Message\\
         * @param cookieString_ -
         * @param onVerifyUserCallbackFn_ - 
         */
        virtual void verifyCookieAndGetUserInfo(
            const std::string& cookieString_,
            const ErrorCodeUserInfoCallbackFn& onVerifyUserCallbackFn_) = 0;
         /**
         * ///Message\\
         * @param cookieString_ - 
         * @param onLogoutCookieCallbackFn_ -
         */
        virtual void logoutUserForCookie(const std::string& cookieString_,
                                         const ErrorCodeCallbackFn& onLogoutCookieCallbackFn_) = 0;
        /**
         * ///Message\\
         * @param currentCookieString_ - 
         * @param onLogoutAllCookiesCallbackFn_ - 
         */
        virtual void logoutUserForAllCookies(const std::string& currentCookieString_,
                                             const ErrorCodeCallbackFn& onLogoutAllCookiesCallbackFn_) = 0;
         /**
         * ///Message\\
         * @param cookieString_ -
         * @param oldPassword_ - 
         * @param newPassword_ - 
         * @param onChangePasswordCallbackFn_ - 
         */
        virtual void changeUserPassword(const std::string& cookieString_,
                                        const std::string& oldPassword_,
                                        const std::string& newPassword_,
                                        const ErrorCodeCallbackFn& onChangePasswordCallbackFn_) = 0;
        /**
         * ///Message\\
         * @param - cookieString_
         * @param - newEmailAddress_
         * @param - password_
         * @param - onChangeEmailAddressCallbackFn_
         */
        virtual void changeUserEmailAddress(const std::string& cookieString_,
                                            const std::string& newEmailAddress_,
                                            const std::string& password_,
                                            const ErrorCodeCallbackFn& onChangeEmailAddressCallbackFn_) = 0;
        /**
         * ///Message\\
         * @param cookieString_ -
         * @param onDeleteUserCallbackFn_ - 
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
