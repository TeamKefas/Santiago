#include "ControllerBase.h"

namespace Santiago{ namespace User
{

    ControllerBase::ControllerBase(boost::asio::io_service& ioService_, const boost::property_tree::ptree& config_)
        :_ioService(ioService_)
        ,_strand(_ioService)
        ,_config(config_)
    {}

    void ControllerBase::createUser(const std::string& userName_,
                                    const std::string& password_,
                                    const ErrorCodeCallbackFn& onCreateUserCallbackFn_)
    {
        _strand.post(std::bind(&ControllerBase::createUserImpl,this,userName_,password_,onCreateUserCallbackFn_));
    }

    void ControllerBase::loginUser(const std::string& userName_,
                                   const std::string& password_,
                                   const ErrorCodeStringCallbackFn& onLoginUserCallbackFn_)
    {
        _strand.post(std::bind(&ControllerBase::loginUserImpl,this,userName_,password_,onLoginUserCallbackFn_));
    }

    void ControllerBase::verifyCookieAndGetUserName(const std::string& cookieString_,
                                                    const ErrorCodeStringCallbackFn& onVerifyUserCallbackFn_)
    {
        _strand.post(std::bind(&ControllerBase::verifyCookieAndGetUserNameImpl,
                               this,
                               cookieString_,
                               onVerifyUserCallbackFn_));
    }

    void ControllerBase::logoutUserForCookie(const std::string& cookieString_,
                                             const ErrorCodeCallbackFn& onLogoutCookieCallbackFn_)
    {
        _strand.post(std::bind(&ControllerBase::logoutUserForCookieImpl,this,cookieString_,onLogoutCookieCallbackFn_));
    }

    void ControllerBase::logoutUserForAllCookies(const std::string& ccurrentCookieString_,
                                                 const ErrorCodeCallbackFn& onLogoutAllCookiesCallbackFn_)
    {
        _strand.post(std::bind(&ControllerBase::logoutUserForAllCookiesImpl,
                               this,
                               currentCookieString_,
                               onLogoutAllCookiesCallbackFn_));
    }

    void ControllerBase::changeUserPassword(const std::string& cookieString_,
                                            const std::string& oldPassword_,
                                            const std::string& newPassword_,
                                            const ErrorCodeCallbackFn& onChangePasswordCallbackFn_)
    {
        _strand.post(std::bind(&ControllerBase::changeUserPasswordImpl,
                               this,
                               cookieString__,
                               oldPassword_,
                               newPassword_,
                               onChangePasswordCallbackFn_));
    }

    void ControllerBase::deleteUser(const std::string& cookieString_,
                                    const ErrorCodeCallbackFn& onDeleteUserCallbackFn_)
    {
        _strand.post(std::bind(&ControllerBase::deleteUserImpl,this,cookieString_,onDeleteUserCallbackFn_));
    }

}}
