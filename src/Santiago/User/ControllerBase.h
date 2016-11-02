#ifndef SANTIAGO_USER_CONTROLLERBASE_H
#define SANTIAGO_USER_CONTROLLERBASE_H

#include <functional>
#include <system_error>

#include <boost/asio.hpp>
#include <boost/property_tree/ptree.hpp>

#include <boost/optional.hpp>

namespace Santiago{ namespace User
{
    struct UserInfo
    {
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

        ControllerBase(boost::asio::io_service& ioService_, const boost::property_tree::ptree& config_);
	virtual ~ControllerBase()
	{}

        virtual void createUser(const std::string& userName_,
                                const std::string& emailAddress_,
                                const std::string& password_,
                                const ErrorCodeCallbackFn& onCreateUserCallbackFn_) = 0;

        virtual void loginUser(const std::string& userNameOrEmailAddress_,
                               bool isUserNameNotEmailAddress_,
                               const std::string& password_,
                               const ErrorCodeUserInfoStringPairCallbackFn& onLoginUserCallbackFn_) = 0;

        virtual void verifyCookieAndGetUserInfo(
            const std::string& cookieString_,
            const ErrorCodeUserInfoCallbackFn& onVerifyUserCallbackFn_) = 0;

        virtual void logoutUserForCookie(const std::string& cookieString_,
                                         const ErrorCodeCallbackFn& onLogoutCookieCallbackFn_) = 0;

        virtual void logoutUserForAllCookies(const std::string& currentCookieString_,
                                             const ErrorCodeCallbackFn& onLogoutAllCookiesCallbackFn_) = 0;

        virtual void changeUserPassword(const std::string& cookieString_,
                                        const std::string& oldPassword_,
                                        const std::string& newPassword_,
                                        const ErrorCodeCallbackFn& onChangePasswordCallbackFn_) = 0;

        virtual void deleteUser(const std::string& cookieString_,
                                const ErrorCodeCallbackFn& onDeleteUserCallbackFn_) = 0;

    protected:
        
        boost::asio::io_service         &_ioService;
        boost::asio::strand              _strand;
        boost::property_tree::ptree      _config;

    };

}} 
#endif
