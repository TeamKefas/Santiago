#include "AuthenticatorImpl.h"

namespace Santiago{ namespace Authentication{ namespace SingleNode
{
    std::error_code AuthenticatorImpl::createUser(const std::string& userName_,
                                                  const std::string& emailAddress_,
                                                  const std::string& password_,
                                                  boost::asio::yield_context yield_)
    {
        return _controller.createUser(userName_,
                                      emailAddress_,
                                      password_,
                                      yield_);
    }

    std::pair<std::error_code,boost::optional<std::pair<UserInfo,std::string> > >
    AuthenticatorImpl::loginUser(const std::string& userNameOrEmailAddress_,
                                 bool isUserNameNotEmailAddress_,
                                 const std::string& password_,
                                 boost::asio::yield_context yield_)
    {
        return _controller.loginUser(None(),
                                     userNameOrEmailAddress_,
                                     isUserNameNotEmailAddress_,
                                     password_,
                                     yield_);  
    }
        
    std::pair<std::error_code,boost::optional<UserInfo> >
    AuthenticatorImpl::verifyCookieAndGetUserInfo(const std::string& cookieString_,
                                                  boost::asio::yield_context yield_)
    {
        return _controller.verifyCookieAndGetUserInfo(None(),
                                                      cookieString_,
                                                      yield_);
    }

    std::error_code AuthenticatorImpl::logoutUserForCookie(const std::string& cookieString_,
                                                           boost::asio::yield_context yield_)
    {
        return _controller.logoutUserForCookie(cookieString_,
                                               yield_);
    }

    std::error_code AuthenticatorImpl::logoutUserForAllCookies(const std::string& userName_,
                                                               boost::asio::yield_context yield_)
    {
        return _controller.logoutUserForAllCookies(userName_,
                                                   yield_);
    }

    std::error_code AuthenticatorImpl::changeUserPassword(const std::string& cookieString_,
                                                          const std::string& oldPassword_,
                                                          const std::string& newPassword_,
                                                          boost::asio::yield_context yield_)
    {
        return _controller.changeUserPassword(cookieString_,
                                              oldPassword_,
                                              newPassword_,
                                              yield_);
    }

    std::pair<std::error_code,boost::optional<std::string> >
    AuthenticatorImpl::getUserForEmailAddressAndRecoveryString(const std::string& emailAddress_,
                                                               const std::string& recoveryString_,
                                                               boost::asio::yield_context yield_)
    {
        return _controller.getUserForEmailAddressAndRecoveryString(emailAddress_,
                                                                   recoveryString_,
                                                                   yield_);
    }

    std::error_code AuthenticatorImpl::changeUserPasswordForEmailAddressAndRecoveryString(
        const std::string& emailAddress_,
        const std::string& recoveryString_,
        const std::string& newPassword_,
        boost::asio::yield_context yield_)
    {
        return _controller.changeUserPasswordForEmailAddressAndRecoveryString(emailAddress_,
                                                                              recoveryString_,
                                                                              newPassword_,
                                                                              yield_);
    }

    std::error_code AuthenticatorImpl::changeUserEmailAddress(const std::string& cookieString_,
                                                              const std::string& newEmailAddress_,
                                                              const std::string& password_,
                                                              boost::asio::yield_context yield_)
    {
        return _controller.changeUserEmailAddress(cookieString_,
                                                  newEmailAddress_,
                                                  password_,
                                                  yield_);
    }

    std::pair<std::error_code,boost::optional<std::string> >
    AuthenticatorImpl::createAndReturnRecoveryString(const std::string& emailAddress_,
                                                     boost::asio::yield_context yield_)
    {
        return _controller.createAndReturnRecoveryString(emailAddress_,
                                                         yield_);
    }

    std::error_code AuthenticatorImpl::deleteUser(const std::string& cookieString_,
                                                  boost::asio::yield_context yield_)
    {
        return _controller.deleteUser(cookieString_,
                                      yield_);
    }
    
}}}