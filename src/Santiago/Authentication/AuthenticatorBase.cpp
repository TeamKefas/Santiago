#include <boost/asio.hpp>

#include "AuthenticatorBase.h"

namespace Santiago{ namespace Authentication
{
    
    boost::optional<UserInfo> AuthenticatorBase::
    verifyCookieAndGetUserInfo(const std::string& cookieString_,
                               boost::asio::yield_context yield_,
                               std::error_code& error_)
    {
        typename boost::asio::handler_type<boost::asio::yield_context, void()>::type
            handler(std::forward<boost::asio::yield_context>(yield_));
        
        boost::asio::async_result<decltype(handler)> result(handler);
        boost::optional<UserInfo> ret;
        
        verifyCookieAndGetUserInfo(
            cookieString_,
            [&error_,&ret,handler](const std::error_code& ec_,
                                   const boost::optional<UserInfo>& userInfoOpt_)
            {
                error_ = ec_;
                ret = userInfoOpt_;
                asio_handler_invoke(handler, &handler);
            });
        
        result.get();
        return ret;       
    }

    void AuthenticatorBase::asyncCreateUser(const std::string& userName_,
                                             const std::string& emailAddress_,
                                             const std::string& password_,
                                             boost::asio::yield_context yield_,
                                             std::error_code& error_)
    {
        typename boost::asio::handler_type<boost::asio::yield_context, void()>::type
            handler(std::forward<boost::asio::yield_context>(yield_));
        
        boost::asio::async_result<decltype(handler)> result(handler);
        
        createUser(userName_,
                   emailAddress_,
                   password_,
                   [&error_,handler](const std::error_code& ec_)
                   {
                       error_ = ec_;
                       asio_handler_invoke(handler, &handler);
                   });
        
        result.get();
    }
        

    
    boost::optional<std::pair<UserInfo,std::string> >
    AuthenticatorBase::asyncLoginUser(const std::string& userNameOrEmailAddress_,
                                      bool isUserNameNotEmailAddress_,
                                      const std::string& password_,
                                      boost::asio::yield_context yield_,
                                      std::error_code& error_)
    {
        typename boost::asio::handler_type<boost::asio::yield_context, void()>::type
            handler(std::forward<boost::asio::yield_context>(yield_));
        
        boost::asio::async_result<decltype(handler)> result(handler);
        boost::optional<std::pair<UserInfo,std::string> > ret;
        
        loginUser(userNameOrEmailAddress_,
                  isUserNameNotEmailAddress_,
                  password_,
                  [&error_,&ret,handler](const std::error_code& ec_,
                                         const boost::optional<std::pair<UserInfo,std::string> >& userInfoCookiePairOpt_)
                  {
                      error_ = ec_;
                      ret = userInfoCookiePairOpt_;
                      asio_handler_invoke(handler, &handler);
                  });
        result.get();
        return ret;
    }

    void AuthenticatorBase::asyncLogoutUserForCookie(const std::string& cookieString_,
                                                     boost::asio::yield_context yield_,
                                                     std::error_code& error_)
    {
         typename boost::asio::handler_type<boost::asio::yield_context, void()>::type
            handler(std::forward<boost::asio::yield_context>(yield_));
        
        boost::asio::async_result<decltype(handler)> result(handler);
        
        logoutUserForCookie(cookieString_,
                            [&error_,handler](const std::error_code& ec_)
                            {
                                error_ = ec_;
                                asio_handler_invoke(handler, &handler);
                            });
        
        result.get();
    }

    void AuthenticatorBase:: asyncLogoutUserForAllCookies(const std::string& userName_,
                                                          boost::asio::yield_context yield_,
                                                          std::error_code& error_)
    {
        typename boost::asio::handler_type<boost::asio::yield_context, void()>::type
            handler(std::forward<boost::asio::yield_context>(yield_));
        
        boost::asio::async_result<decltype(handler)> result(handler);
        
        logoutUserForAllCookies(userName_,
                                [&error_,handler](const std::error_code& ec_)
                                {
                                    error_ = ec_;
                                    asio_handler_invoke(handler, &handler);
                                });
        
        result.get();
    }

    void AuthenticatorBase::asyncChangeUserPassword(const std::string& cookieString_,
                                                    const std::string& oldPassword_,
                                                    const std::string& newPassword_,
                                                    boost::asio::yield_context yield_,
                                                    std::error_code& error_  )
    {
        typename boost::asio::handler_type<boost::asio::yield_context, void()>::type
            handler(std::forward<boost::asio::yield_context>(yield_));
        
        boost::asio::async_result<decltype(handler)> result(handler);
        
        changeUserPassword(cookieString_,
                           oldPassword_,
                           newPassword_,
                           [&error_,handler](const std::error_code& ec_)
                           {
                               error_ = ec_;
                               asio_handler_invoke(handler, &handler);
                           });
        
        result.get();
    }

    boost::optional<std::string> AuthenticatorBase::asyncGetUserForEmailAddressAndRecoveryString(const std::string& emailAddress_,
                                                                                                 const std::string& recoverystring_,
                                                                                                 boost::asio::yield_context yield_,
                                                                                                 std::error_code& error_)
    {
         typename boost::asio::handler_type<boost::asio::yield_context, void()>::type
            handler(std::forward<boost::asio::yield_context>(yield_));
        
        boost::asio::async_result<decltype(handler)> result(handler);
        boost::optional<std::string> ret;
        
        getUserForEmailAddressAndRecoveryString(
            emailAddress_,
            recoverystring_,
            [&error_,&ret,handler](const std::error_code& ec_,
                                   const boost::optional<std::string>& UserNameOpt_)
            {
                error_ = ec_;
                ret = UserNameOpt_;
                asio_handler_invoke(handler, &handler);
            });
        
        result.get();
        return ret;
    }
    

    void AuthenticatorBase::verifyCookieAndGetUserInfo(const std::string& cookieString_,
                                                       const ErrorCodeUserInfoCallbackFn& onVerifyUserCallbackFn_)
    {
        _strand.post(std::bind(&AuthenticatorBase::verifyCookieAndGetUserInfoImpl,
                               this,
                               cookieString_,
                               onVerifyUserCallbackFn_));
    }
    
    void AuthenticatorBase::createUser(const std::string& userName_,
                                       const std::string& emailAddress_,
                                       const std::string& password_,
                                       const ErrorCodeCallbackFn& onCreateUserCallbackFn_)
    {
        _strand.post(std::bind(&AuthenticatorBase::createUserImpl,
                               this,
                               userName_,
                               emailAddress_,
                               password_,
                               onCreateUserCallbackFn_));
    }
    
    void AuthenticatorBase::loginUser(const std::string& userNameOrEmailAddress_,
                                      bool isUserNameNotEmailAddress_,
                                      const std::string& password_,
                                      const ErrorCodeUserInfoStringPairCallbackFn& onLoginUserCallbackFn_)
    {
        _strand.post(std::bind(&AuthenticatorBase::loginUserImpl,
                               this,
                               userNameOrEmailAddress_,
                               isUserNameNotEmailAddress_,
                               password_,
                               onLoginUserCallbackFn_));
    }

    void AuthenticatorBase::logoutUserForCookie(const std::string& cookieString_,
                                                const ErrorCodeCallbackFn& onLogoutCookieCallbackFn_)
    {
        _strand.post(std::bind(&AuthenticatorBase::logoutUserForCookieImpl,this,cookieString_,onLogoutCookieCallbackFn_));
    }

    void AuthenticatorBase::logoutUserForAllCookies(const std::string& userName_,
                                                    const ErrorCodeCallbackFn& onLogoutAllCookiesCallbackFn_)
    {
        _strand.post(std::bind(&AuthenticatorBase::logoutUserForAllCookiesImpl,
                               this,
                               userName_,
                               onLogoutAllCookiesCallbackFn_));
    }

    void AuthenticatorBase::changeUserPassword(const std::string& cookieString_,
                                               const std::string& oldPassword_,
                                               const std::string& newPassword_,
                                               const ErrorCodeCallbackFn& onChangePasswordCallbackFn_)
    {
        _strand.post(std::bind(&AuthenticatorBase::changeUserPasswordImpl,
                               this,
                               cookieString_,
                               oldPassword_,
                               newPassword_,
                               onChangePasswordCallbackFn_));
    }
    
    void AuthenticatorBase::createAndReturnRecoveryString(const std::string& emailAddress_,
                                                          const ErrorCodeStringCallbackFn& onCreateAndReturnRecoveryStringCallbackFn_)
    {
        _strand.post(std::bind(&AuthenticatorBase::createAndReturnRecoveryStringImpl,
                               this,
                               emailAddress_,
                               onCreateAndReturnRecoveryStringCallbackFn_));
    }

    void AuthenticatorBase::getUserForEmailAddressAndRecoveryString(const std::string& emailAddress_,
                                                                    const std::string& recoveryString_,
                                                                    const ErrorCodeStringCallbackFn& onGetUserForEmailAddressAndRecoveryStringCallbackFn_)
    {
        _strand.post(std::bind(&AuthenticatorBase::getUserForEmailAddressAndRecoveryStringImpl,
                               this,
                               emailAddress_,
                               recoveryString_,
                               onGetUserForEmailAddressAndRecoveryStringCallbackFn_));
    }

    void AuthenticatorBase::changeUserPasswordForEmailAddressAndRecoveryString(const std::string& emailAddress_,
                                                                               const std::string& recoveryString_,
                                                                               const std::string& newPassword_,
                                                                               const ErrorCodeCallbackFn& onChangePasswordForEmailAddressAndRecoveryStringCallbackFn_)
    {
        _strand.post(std::bind(&AuthenticatorBase::changeUserPasswordForEmailAddressAndRecoveryStringImpl,
                               this,
                               emailAddress_,
                               recoveryString_,
                               newPassword_,
                               onChangePasswordForEmailAddressAndRecoveryStringCallbackFn_));
    }

    void AuthenticatorBase::changeUserEmailAddress(const std::string& cookieString_,
                                                   const std::string& newEmailAddress_,
                                                   const std::string& password_,
                                                   const ErrorCodeCallbackFn& onChangeEmailAddressCallbackFn_)
    {
        _strand.post(std::bind(&AuthenticatorBase::changeUserEmailAddressImpl,
                               this,
                               cookieString_,
                               newEmailAddress_,
                               password_,
                               onChangeEmailAddressCallbackFn_));
    }

    
    void AuthenticatorBase::deleteUser(const std::string& cookieString_,
                                       const ErrorCodeCallbackFn& onDeleteUserCallbackFn_)
    {
        _strand.post(std::bind(&AuthenticatorBase::deleteUserImpl,this,cookieString_,onDeleteUserCallbackFn_));
    }

     void AuthenticatorBase::postCallbackFn(const ErrorCodeUserInfoCallbackFn& errorCodeUserInfoCallbackFn_,
                                            const std::error_code& error_,
                                            const boost::optional<UserInfo>& userInfoOpt_)
    {
        std::function<void()> errorCodeUserInfoCallbackFnImpl =
            std::bind(errorCodeUserInfoCallbackFn_,error_, userInfoOpt_);
        _ioService.post(errorCodeUserInfoCallbackFnImpl);
    }

    void AuthenticatorBase::postCallbackFn(const ErrorCodeUserInfoStringPairCallbackFn& errorCodeUserInfoStringPairCallbackFn_,
                                           const std::error_code& error_,
                                           const boost::optional<std::pair<UserInfo,std::string> >& userInfoStringPair_)
    {
        std::function<void()> errorCodeUserInfoStringPairCallbackFnImpl = 
            std::bind(errorCodeUserInfoStringPairCallbackFn_, error_, userInfoStringPair_);
        _ioService.post(errorCodeUserInfoStringPairCallbackFnImpl);
    }

    void AuthenticatorBase::postCallbackFn(const ErrorCodeCallbackFn& errorCodeCallbackFn_,
                                           const std::error_code& error_)
    {
        std::function<void()> errorCodeCallbackFnImpl = std::bind(errorCodeCallbackFn_,error_);
        _ioService.post(errorCodeCallbackFnImpl);
    }

    void AuthenticatorBase::postCallbackFn(const ErrorCodeStringCallbackFn& errorCodeStringCallbackFn_,
                                           const std::error_code& error_,
                                           const boost::optional<std::string>& string_)
    {
        std::function<void()> errorCodeStringCallbackFnImpl =
            std::bind(errorCodeStringCallbackFn_,error_,string_);
        _ioService.post(errorCodeStringCallbackFnImpl);
    }


}}
        
