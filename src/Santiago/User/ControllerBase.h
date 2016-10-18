#ifndef SANTIAGO_USER_CONTROLLERBASE_H
#define SANTIAGO_USER_CONTROLLERBASE_H

#include <functional>
#include <system_error>

#include <boost/asio.hpp>
#include <boost/property_tree/ptree.hpp>

#include <boost/optional.hpp>

namespace Santiago{ namespace User
{

    class ControllerBase
    {
    public:

        typedef std::function<void(const std::error_code&,const boost::optional<std::string>&)>
        ErrorCodeStringCallbackFn;
        typedef std::function<void(const std::error_code&)> ErrorCodeCallbackFn;

        ControllerBase(const ControllerBase&) = delete;
        ControllerBase& operator=(const ControllerBase&) = delete;

        ControllerBase(boost::asio::io_service& ioService_, const boost::property_tree::ptree& config_);

        virtual void createUser(const std::string& userName_,
                                const std::string& password_,
                                const ErrorCodeCallbackFn& onCreateUserCallbackFn_);

        virtual void loginUser(const std::string& userName_,
                               const std::string& passworld_,
                               const ErrorCodeStringCallbackFn& onLoginUserCallbackFn_);

        virtual void verifyCookieAndGetUserName(const std::string& cookieString_,
                                                const ErrorCodeStringCallbackFn& onVerifyUserCallbackFn_);

        virtual void logoutUserForCookie(const std::string& cookieString_,
                                         const ErrorCodeCallbackFn& onLogoutCookieCallbackFn_);

        virtual void logoutUserForAllCookies(const std::string& currentCookieString_,
                                             const ErrorCodeCallbackFn& onLogoutAllCookiesCallbackFn_);

        virtual void changeUserPassword(const std::string& cookieString_,
                                        const std::string& oldPassword_,
                                        const std::string& newPassword_,
                                        const ErrorCodeCallbackFn& onChangePasswordCallbackFn_);

        virtual void deleteUser(const std::string& cookieString_,
                                const ErrorCodeCallbackFn& onDeleteUserCallbackFn_);

    protected:

        virtual void createUserImpl(const std::string& userName_,
                                    const std::string& password_,
                                    const ErrorCodeCallbackFn& onCreateUserCallbackFn_) = 0;

        virtual void loginUserImpl(const std::string& userName_,
                                   const std::string& passworld_,
                                   const ErrorCodeStringCallbackFn& onLoginUserCallbackFn_) = 0;

        virtual void verifyCookieAndGetUserNameImpl(const std::string& cookieString_,
                                                    const ErrorCodeStringCallbackFn& onVerifyUserCallbackFn_) = 0;

        virtual void logoutUserForCookieImpl(const std::string& cookieString_,
                                             const ErrorCodeCallbackFn& onLogoutCookieCallbackFn_) = 0;

        virtual void logoutUserForAllCookiesImpl(const std::string& currentCookieString_,
                                                 const ErrorCodeCallbackFn& onLogoutAllCookiesCallbackFn_) = 0;

        virtual void changeUserPasswordImpl(const std::string& cookieString_,
                                            const std::string& oldPassword_,
                                            const std::string& newPassword_,
                                            const ErrorCodeCallbackFn& onChangePasswordCallbackFn_) = 0;
        
        virtual void deleteUserImpl(const std::string& cookieString_,
                                    const ErrorCodeCallbackFn& onDeleteUserCallbackFn_) = 0;

        boost::asio::io_service         &_ioService;
        boost::asio::strand              _strand;
        boost::property_tree::ptree      _config;

    };

}} 
#endif
