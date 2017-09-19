#include <boost/asio.hpp>

#include "AuthenticatorBaseV1.h"

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
        
        std::pair<AuthenticatorImplBasePtr,StrandPtr> authenticatorStrandPair = getAuthenticatorAndStrandForString(cookieString_,true);

        boost::asio::spawn(
            *authenticatorStrandPair.second,
            [&error_,&ret,handler,authenticatorStrandPair,this,cookieString_](boost::asio::yield_context yield_)
            //NOTE:This yield_ is not same as above yield_
            {
                std::tie(error_,ret) = authenticatorStrandPair.first->
                    verifyCookieAndGetUserInfo(cookieString_,
                                               yield_);
                asio_handler_invoke(handler, &handler);
            });

        result.get();
        return ret;
    }

    void AuthenticatorBase::
    verifyCookieAndGetUserInfo(const std::string& cookieString_,
                               const ErrorCodeUserInfoCallbackFn& onVerifyUserCallbackFn_)
    {
        ErrorCodeUserInfoCallbackFn postCallbackWrapper(std::bind(static_cast<void(AuthenticatorBase::*)
                                                                  (const ErrorCodeUserInfoCallbackFn&, const std::error_code&,
                                                                   const boost::optional<UserInfo>&)>
                                                                  (&AuthenticatorBase::postCallbackFn),
                                                                  this,
                                                                  onVerifyUserCallbackFn_,
                                                                  std::placeholders::_1,
                                                                  std::placeholders::_2));

        std::pair<AuthenticatorImplBasePtr,StrandPtr> authenticatorStrandPair = getAuthenticatorAndStrandForString(cookieString_,true);

        boost::asio::spawn(
            *authenticatorStrandPair.second,
            [authenticatorStrandPair,this,cookieString_,postCallbackWrapper](boost::asio::yield_context yield_)
            {
                std::error_code error;
                boost::optional<UserInfo> userInfoOpt;
                
                std::tie(error,userInfoOpt) = authenticatorStrandPair.first->
                    verifyCookieAndGetUserInfo(cookieString_,
                                               yield_);
                postCallbackWrapper(error,userInfoOpt);
            });        
    }

    void AuthenticatorBase::createUser(const std::string& userName_,
                                       const std::string& emailAddress_,
                                       const std::string& password_,
                                       boost::asio::yield_context yield_,
                                       std::error_code& error_)
    {
        typename boost::asio::handler_type<boost::asio::yield_context, void()>::type
            handler(std::forward<boost::asio::yield_context>(yield_));
        
        boost::asio::async_result<decltype(handler)> result(handler);

        std::pair<AuthenticatorImplBasePtr,StrandPtr> authenticatorStrandPair = getAuthenticatorAndStrandForString(userName_,true);

        boost::asio::spawn(
            *authenticatorStrandPair.second,
            [&error_,handler,authenticatorStrandPair,userName_,emailAddress_,password_](boost::asio::yield_context yield_)
            //NOTE: This yield_ is not same as above yield_
            {
                error_ = authenticatorStrandPair.first->createUser(userName_,emailAddress_,password_,yield_);
                asio_handler_invoke(handler, &handler);
            });
        
        result.get();
    }

    void AuthenticatorBase::createUser(const std::string& userName_,
                                       const std::string& emailAddress_,
                                       const std::string& password_,
                                       const ErrorCodeCallbackFn& onCreateUserCallbackFn_)
    {
        ErrorCodeCallbackFn postCallbackWrapper(std::bind(static_cast<void(AuthenticatorBase::*)
                                                          (const ErrorCodeCallbackFn&, const std::error_code&)>
                                                          (&AuthenticatorBase::postCallbackFn),
                                                          this,
                                                          onCreateUserCallbackFn_,
                                                          std::placeholders::_1));
         std::pair<AuthenticatorImplBasePtr,StrandPtr> authenticatorStrandPair = getAuthenticatorAndStrandForString(userName_,true);
         boost::asio::spawn(
             *authenticatorStrandPair.second,
             [authenticatorStrandPair,userName_,emailAddress_,password_,postCallbackWrapper]
             (boost::asio::yield_context yield_)
             //NOTE: This yield_ is not same as above yield_
             {
                 std::error_code error = authenticatorStrandPair.first->createUser(
                     userName_,
                     emailAddress_,
                     password_,
                     yield_);
                 postCallbackWrapper(error);
             });
    }

    boost::optional<std::pair<UserInfo,std::string> > AuthenticatorBase::
    loginUser(const std::string& userNameOrEmailAddress_,
              bool isUserNameNotEmailAddress_,
              const std::string& password_,
              boost::asio::yield_context yield_,
              std::error_code& error_)
    {
        typename boost::asio::handler_type<boost::asio::yield_context, void()>::type
            handler(std::forward<boost::asio::yield_context>(yield_));
        
        boost::asio::async_result<decltype(handler)> result(handler);
        boost::optional<std::pair<UserInfo,std::string> > ret;

        std::pair<AuthenticatorImplBasePtr,StrandPtr> authenticatorStrandPair = getAuthenticatorAndStrandForString(userNameOrEmailAddress_,isUserNameNotEmailAddress_);

        boost::asio::spawn(
            *authenticatorStrandPair.second,
            [&error_,&ret,handler,this,authenticatorStrandPair,userNameOrEmailAddress_,isUserNameNotEmailAddress_,password_](boost::asio::yield_context yield_)
            {
                std::tie(error_,ret) = authenticatorStrandPair.first->
                    loginUser(userNameOrEmailAddress_,
                              isUserNameNotEmailAddress_,
                              password_,
                              yield_);
                asio_handler_invoke(handler, &handler);
            });
        
        result.get();
        return ret;
    }

    void AuthenticatorBase::loginUser(const std::string& userNameOrEmailAddress_,
                                      bool isUserNameNotEmailAddress_,
                                      const std::string& password_,
                                      const ErrorCodeUserInfoStringPairCallbackFn& onLoginUserCallbackFn_)
    {
        ErrorCodeUserInfoStringPairCallbackFn postCallbackWrapper(std::bind(static_cast<void(AuthenticatorBase::*)
                                                                            (const ErrorCodeUserInfoStringPairCallbackFn&, const std::error_code&,
                                                                             const boost::optional<std::pair<UserInfo,std::string> >&)>
                                                                            (&AuthenticatorBase::postCallbackFn),
                                                                            this,
                                                                            onLoginUserCallbackFn_,
                                                                            std::placeholders::_1,
                                                                            std::placeholders::_2));

        std::pair<AuthenticatorImplBasePtr,StrandPtr> authenticatorStrandPair = getAuthenticatorAndStrandForString(userNameOrEmailAddress_,isUserNameNotEmailAddress_);

        boost::asio::spawn(
            *authenticatorStrandPair.second,
            [authenticatorStrandPair,this,userNameOrEmailAddress_,isUserNameNotEmailAddress_,password_,postCallbackWrapper](boost::asio::yield_context yield_)
            {
                std::error_code error;
                boost::optional<std::pair<UserInfo,std::string> > userInfoStringPairOpt;
                
                std::tie(error,userInfoStringPairOpt) = authenticatorStrandPair.first->
                    loginUser(userNameOrEmailAddress_,
                              isUserNameNotEmailAddress_,
                              password_,
                              yield_);
                postCallbackWrapper(error,userInfoStringPairOpt);
            });        
    }
    
    void AuthenticatorBase::
    logoutUserForCookie(const std::string& cookieString_,
                        boost::asio::yield_context yield_,
                        std::error_code& error_)
    {
        typename boost::asio::handler_type<boost::asio::yield_context, void()>::type
            handler(std::forward<boost::asio::yield_context>(yield_));
        
        boost::asio::async_result<decltype(handler)> result(handler);

        std::pair<AuthenticatorImplBasePtr,StrandPtr> authenticatorStrandPair = getAuthenticatorAndStrandForString(cookieString_,true);

        boost::asio::spawn(
            *authenticatorStrandPair.second,
            [&error_,handler,authenticatorStrandPair,cookieString_](boost::asio::yield_context yield_)
            {
                error_ = authenticatorStrandPair.first->logoutUserForCookie(cookieString_,yield_);
                asio_handler_invoke(handler, &handler);
            });
        
        result.get();
    }

    void AuthenticatorBase::logoutUserForCookie(const std::string& cookieString_,
                                                            const ErrorCodeCallbackFn& onLogoutCookieCallbackFn_)
    {
        ErrorCodeCallbackFn postCallbackWrapper(std::bind(static_cast<void(AuthenticatorBase::*)
                                                          (const ErrorCodeCallbackFn&, const std::error_code&)>
                                                          (&AuthenticatorBase::postCallbackFn),
                                                          this,
                                                          onLogoutCookieCallbackFn_,
                                                          std::placeholders::_1));
         std::pair<AuthenticatorImplBasePtr,StrandPtr> authenticatorStrandPair = getAuthenticatorAndStrandForString(cookieString_,true);
         boost::asio::spawn(
             *authenticatorStrandPair.second,
             [authenticatorStrandPair,cookieString_,postCallbackWrapper]
             (boost::asio::yield_context yield_)
             {
                 std::error_code error = authenticatorStrandPair.first->logoutUserForCookie(
                     cookieString_,
                     yield_);
                 postCallbackWrapper(error);
             });
    }

    void AuthenticatorBase::
    logoutUserForAllCookies(const std::string& userName_,
                            boost::asio::yield_context yield_,
                            std::error_code& error_)
    {
        typename boost::asio::handler_type<boost::asio::yield_context, void()>::type
            handler(std::forward<boost::asio::yield_context>(yield_));
        
        boost::asio::async_result<decltype(handler)> result(handler);
        
        std::pair<AuthenticatorImplBasePtr,StrandPtr> authenticatorStrandPair = getAuthenticatorAndStrandForString(userName_,true);
        
         boost::asio::spawn(
            *authenticatorStrandPair.second,
            [&error_,handler,authenticatorStrandPair,userName_](boost::asio::yield_context yield_)
            {
                error_ = authenticatorStrandPair.first->logoutUserForAllCookies(userName_,yield_);
                asio_handler_invoke(handler, &handler);
            });
        
        result.get();
    }

    void AuthenticatorBase::
    logoutUserForAllCookies(const std::string& userName_,
                            const ErrorCodeCallbackFn& onLogoutAllCookiesCallbackFn_)
    {
        ErrorCodeCallbackFn postCallbackWrapper(std::bind(static_cast<void(AuthenticatorBase::*)
                                                          (const ErrorCodeCallbackFn&, const std::error_code&)>
                                                          (&AuthenticatorBase::postCallbackFn),
                                                          this,
                                                          onLogoutAllCookiesCallbackFn_,
                                                          std::placeholders::_1));
         std::pair<AuthenticatorImplBasePtr,StrandPtr> authenticatorStrandPair = getAuthenticatorAndStrandForString(userName_,true);
         boost::asio::spawn(
             *authenticatorStrandPair.second,
             [authenticatorStrandPair,userName_,postCallbackWrapper]
             (boost::asio::yield_context yield_)
             {
                 std::error_code error = authenticatorStrandPair.first->logoutUserForAllCookies(userName_,
                                                                                              yield_);
                 postCallbackWrapper(error);
             });
    }

    void AuthenticatorBase::
    changeUserPassword(const std::string& cookieString_,
                       const std::string& oldPassword_,
                       const std::string& newPassword_,
                       boost::asio::yield_context yield_,
                       std::error_code& error_)
    {
        typename boost::asio::handler_type<boost::asio::yield_context, void()>::type
            handler(std::forward<boost::asio::yield_context>(yield_));
        
        boost::asio::async_result<decltype(handler)> result(handler);
        
        std::pair<AuthenticatorImplBasePtr,StrandPtr> authenticatorStrandPair = getAuthenticatorAndStrandForString(cookieString_,true);
        
         boost::asio::spawn(
            *authenticatorStrandPair.second,
            [&error_,handler,authenticatorStrandPair,cookieString_,oldPassword_,newPassword_](boost::asio::yield_context yield_)
            {
                error_ = authenticatorStrandPair.first->changeUserPassword(cookieString_,oldPassword_,newPassword_,yield_);
                asio_handler_invoke(handler, &handler);
            });
        
        result.get();
    }

    void AuthenticatorBase::
    changeUserPassword(const std::string& cookieString_,
                       const std::string& oldPassword_,
                       const std::string& newPassword_,
                       const ErrorCodeCallbackFn& onChangePasswordCallbackFn_)
    {
        ErrorCodeCallbackFn postCallbackWrapper(std::bind(static_cast<void(AuthenticatorBase::*)
                                                          (const ErrorCodeCallbackFn&, const std::error_code&)>
                                                          (&AuthenticatorBase::postCallbackFn),
                                                          this,
                                                          onChangePasswordCallbackFn_,
                                                          std::placeholders::_1));
         std::pair<AuthenticatorImplBasePtr,StrandPtr> authenticatorStrandPair = getAuthenticatorAndStrandForString(cookieString_,true);
         boost::asio::spawn(
             *authenticatorStrandPair.second,
             [authenticatorStrandPair,cookieString_,oldPassword_,newPassword_,postCallbackWrapper]
             (boost::asio::yield_context yield_)
             {
                 std::error_code error = authenticatorStrandPair.first->changeUserPassword(
                     cookieString_,
                     oldPassword_,
                     newPassword_,
                     yield_);
                 postCallbackWrapper(error);
             });
    }
    
    void AuthenticatorBase::
    changeUserEmailAddress(const std::string& cookieString_,
                           const std::string& newEmailAddress_,
                           const std::string& password_,
                           boost::asio::yield_context yield_,
                           std::error_code& error_)
    {
        typename boost::asio::handler_type<boost::asio::yield_context, void()>::type
            handler(std::forward<boost::asio::yield_context>(yield_));
        
        boost::asio::async_result<decltype(handler)> result(handler);
        
        std::pair<AuthenticatorImplBasePtr,StrandPtr> authenticatorStrandPair = getAuthenticatorAndStrandForString(cookieString_,true);
        
         boost::asio::spawn(
            *authenticatorStrandPair.second,
            [&error_,handler,authenticatorStrandPair,cookieString_,newEmailAddress_,password_](boost::asio::yield_context yield_)
            {
                error_ = authenticatorStrandPair.first->changeUserEmailAddress(cookieString_,newEmailAddress_,password_,yield_);
                asio_handler_invoke(handler, &handler);
            });
        
        result.get();
    }

    void AuthenticatorBase::
    changeUserEmailAddress(const std::string& cookieString_,
                           const std::string& newEmailAddress_,
                           const std::string& password_,
                           const ErrorCodeCallbackFn& onChangeEmailAddressCallbackFn_)
    {
        ErrorCodeCallbackFn postCallbackWrapper(std::bind(static_cast<void(AuthenticatorBase::*)
                                                          (const ErrorCodeCallbackFn&, const std::error_code&)>
                                                          (&AuthenticatorBase::postCallbackFn),
                                                          this,
                                                          onChangeEmailAddressCallbackFn_,
                                                          std::placeholders::_1));
         std::pair<AuthenticatorImplBasePtr,StrandPtr> authenticatorStrandPair = getAuthenticatorAndStrandForString(cookieString_,true);
         
         boost::asio::spawn(
             *authenticatorStrandPair.second,
             [authenticatorStrandPair,cookieString_,newEmailAddress_,password_,postCallbackWrapper]
             (boost::asio::yield_context yield_)
             {
                 std::error_code error = authenticatorStrandPair.first->changeUserEmailAddress(
                     cookieString_,
                     newEmailAddress_,
                     password_,
                     yield_);
                 postCallbackWrapper(error);
             });
    }

    boost::optional<std::string> AuthenticatorBase::
    createAndReturnRecoveryString(const std::string& emailAddress_,
                                  boost::asio::yield_context yield_,
                                  std::error_code& error_)
    {
        typename boost::asio::handler_type<boost::asio::yield_context, void()>::type
            handler(std::forward<boost::asio::yield_context>(yield_));
        
        boost::asio::async_result<decltype(handler)> result(handler);
        boost::optional<std::string> ret;

        std::pair<AuthenticatorImplBasePtr,StrandPtr> authenticatorStrandPair = getAuthenticatorAndStrandForString(emailAddress_,false);
        
        boost::asio::spawn(
            *authenticatorStrandPair.second,
            [&error_,&ret,handler,authenticatorStrandPair,emailAddress_](boost::asio::yield_context yield_)
            {
                std::tie(error_,ret) = authenticatorStrandPair.first->
                    createAndReturnRecoveryString(emailAddress_,
                                                  yield_);
                asio_handler_invoke(handler, &handler);
            });
        
        result.get();
        return ret;
    }

    void AuthenticatorBase::createAndReturnRecoveryString(const std::string& emailAddress_,
                                                                      const ErrorCodeStringCallbackFn& onCreateAndReturnRecoveryStringCallbackFn_)
    {
        ErrorCodeStringCallbackFn postCallbackWrapper(std::bind(static_cast<void(AuthenticatorBase::*)
                                                                (const ErrorCodeStringCallbackFn&, const std::error_code&,
                                                                 const boost::optional<std::string>&)>
                                                                (&AuthenticatorBase::postCallbackFn),
                                                                this,
                                                                onCreateAndReturnRecoveryStringCallbackFn_,
                                                                std::placeholders::_1,
                                                                std::placeholders::_2));
         std::pair<AuthenticatorImplBasePtr,StrandPtr> authenticatorStrandPair = getAuthenticatorAndStrandForString(emailAddress_,false);
         boost::asio::spawn(
             *authenticatorStrandPair.second,
             [authenticatorStrandPair,emailAddress_,postCallbackWrapper]
             (boost::asio::yield_context yield_)
             {
                 std::error_code error;
                 boost::optional<std::string> stringOpt;
                 std::tie(error,stringOpt) = authenticatorStrandPair.first->createAndReturnRecoveryString(
                     emailAddress_,
                     yield_);
                 postCallbackWrapper(error,stringOpt);
             });
    }
         
    boost::optional<std::string> AuthenticatorBase::
    getUserForEmailAddressAndRecoveryString(const std::string& emailAddress_,
                                            const std::string& recoveryString_,
                                            boost::asio::yield_context yield_,
                                            std::error_code& error_)
    {
        typename boost::asio::handler_type<boost::asio::yield_context, void()>::type
            handler(std::forward<boost::asio::yield_context>(yield_));
        
        boost::asio::async_result<decltype(handler)> result(handler);
        boost::optional<std::string> ret;
        
         std::pair<AuthenticatorImplBasePtr,StrandPtr> authenticatorStrandPair = getAuthenticatorAndStrandForString(emailAddress_,false);
        
        boost::asio::spawn(
            *authenticatorStrandPair.second,
            [&error_,&ret,handler,authenticatorStrandPair,emailAddress_,recoveryString_](boost::asio::yield_context yield_)
            {
                std::tie(error_,ret) = authenticatorStrandPair.first->
                    getUserForEmailAddressAndRecoveryString(emailAddress_,
                                                            recoveryString_,
                                                            yield_);
                asio_handler_invoke(handler, &handler);
            });
        
        result.get();
        return ret;
    }

    void AuthenticatorBase::getUserForEmailAddressAndRecoveryString(const std::string& emailAddress_,
                                                                                const std::string& recoveryString_,
                                                                                const ErrorCodeStringCallbackFn& onGetUserForEmailAddressAndRecoveryStringCallbackFn_)
    {
        ErrorCodeStringCallbackFn postCallbackWrapper(std::bind(static_cast<void(AuthenticatorBase::*)
                                                                (const ErrorCodeStringCallbackFn&, const std::error_code&,
                                                                 const boost::optional<std::string>&)>
                                                                (&AuthenticatorBase::postCallbackFn),
                                                                this,
                                                                onGetUserForEmailAddressAndRecoveryStringCallbackFn_,
                                                                std::placeholders::_1,
                                                                std::placeholders::_2));
         std::pair<AuthenticatorImplBasePtr,StrandPtr> authenticatorStrandPair = getAuthenticatorAndStrandForString(emailAddress_,false);
         boost::asio::spawn(
             *authenticatorStrandPair.second,
             [authenticatorStrandPair,emailAddress_,recoveryString_,postCallbackWrapper]
             (boost::asio::yield_context yield_)
             {
                 std::error_code error;
                 boost::optional<std::string> stringOpt;
                 std::tie(error,stringOpt) = authenticatorStrandPair.first->getUserForEmailAddressAndRecoveryString(
                     emailAddress_,
                     recoveryString_,
                     yield_);
                 postCallbackWrapper(error,stringOpt);
             });
    }

    void AuthenticatorBase::
    changeUserPasswordForEmailAddressAndRecoveryString(const std::string& emailAddress_,
                                                       const std::string& recoveryString_,
                                                       const std::string& newPassword_,
                                                       boost::asio::yield_context yield_,
                                                       std::error_code& error_)
    {
        typename boost::asio::handler_type<boost::asio::yield_context, void()>::type
            handler(std::forward<boost::asio::yield_context>(yield_));
        
        boost::asio::async_result<decltype(handler)> result(handler);

        std::pair<AuthenticatorImplBasePtr,StrandPtr> authenticatorStrandPair = getAuthenticatorAndStrandForString(emailAddress_,false);
        
         boost::asio::spawn(
            *authenticatorStrandPair.second,
            [&error_,handler,authenticatorStrandPair,emailAddress_,recoveryString_,newPassword_](boost::asio::yield_context yield_)
            {
                error_ = authenticatorStrandPair.first->changeUserPasswordForEmailAddressAndRecoveryString(emailAddress_,recoveryString_,newPassword_,yield_);
                asio_handler_invoke(handler, &handler);
            });
        
        result.get();
    }

    void AuthenticatorBase::
    changeUserPasswordForEmailAddressAndRecoveryString(const std::string& emailAddress_,
                                                       const std::string& recoveryString_,
                                                       const std::string& newPassword_,
                                                       const ErrorCodeCallbackFn& onChangePasswordForEmailAddressAndRecoveryStringCallbackFn_)
    {
        ErrorCodeCallbackFn postCallbackWrapper(std::bind(static_cast<void(AuthenticatorBase::*)
                                                          (const ErrorCodeCallbackFn&, const std::error_code&)>
                                                          (&AuthenticatorBase::postCallbackFn),
                                                          this,
                                                          onChangePasswordForEmailAddressAndRecoveryStringCallbackFn_,
                                                          std::placeholders::_1));
         std::pair<AuthenticatorImplBasePtr,StrandPtr> authenticatorStrandPair = getAuthenticatorAndStrandForString(emailAddress_,false);
         
         boost::asio::spawn(
             *authenticatorStrandPair.second,
             [authenticatorStrandPair,emailAddress_,recoveryString_,newPassword_,postCallbackWrapper]
             (boost::asio::yield_context yield_)
             {
                 std::error_code error = authenticatorStrandPair.first->changeUserPasswordForEmailAddressAndRecoveryString(
                     emailAddress_,
                     recoveryString_,
                     newPassword_,
                     yield_);
                 postCallbackWrapper(error);
             });
    }
      
    void AuthenticatorBase::
    deleteUser(const std::string& cookieString_,
               boost::asio::yield_context yield_,
               std::error_code& error_)
    {
        typename boost::asio::handler_type<boost::asio::yield_context, void()>::type
            handler(std::forward<boost::asio::yield_context>(yield_));
        
        boost::asio::async_result<decltype(handler)> result(handler);
        
        std::pair<AuthenticatorImplBasePtr,StrandPtr> authenticatorStrandPair = getAuthenticatorAndStrandForString(cookieString_,true);
        
        boost::asio::spawn(
            *authenticatorStrandPair.second,
            [&error_,handler,authenticatorStrandPair,cookieString_](boost::asio::yield_context yield_)
            {
                error_ = authenticatorStrandPair.first->deleteUser(cookieString_,
                                                                yield_);
                asio_handler_invoke(handler, &handler);
            });
        
        result.get();
    }

    void AuthenticatorBase::
    deleteUser(const std::string& cookieString_,
               const ErrorCodeCallbackFn& onDeleteUserCallbackFn_)
    {
        ErrorCodeCallbackFn postCallbackWrapper(std::bind(static_cast<void(AuthenticatorBase::*)
                                                          (const ErrorCodeCallbackFn&, const std::error_code&)>
                                                          (&AuthenticatorBase::postCallbackFn),
                                                          this,
                                                          onDeleteUserCallbackFn_,
                                                          std::placeholders::_1));
         std::pair<AuthenticatorImplBasePtr,StrandPtr> authenticatorStrandPair = getAuthenticatorAndStrandForString(cookieString_,true);
         
         boost::asio::spawn(
             *authenticatorStrandPair.second,
             [authenticatorStrandPair,cookieString_,postCallbackWrapper]
             (boost::asio::yield_context yield_)
             {
                 std::error_code error = authenticatorStrandPair.first->deleteUser(
                     cookieString_,
                     yield_);
                 postCallbackWrapper(error);
             });
    }

    void AuthenticatorBase::postCallbackFn(const ErrorCodeUserInfoCallbackFn& errorCodeUserInfoCallbackFn_,
                                                       const std::error_code& error_,
                                                       const boost::optional<UserInfo>& userInfoOpt_)
    {
        std::function<void()> errorCodeUserInfoCallbackFnImpl =
            std::bind(errorCodeUserInfoCallbackFn_,error_, userInfoOpt_);
        _ioService.post(errorCodeUserInfoCallbackFnImpl);
    }

    void AuthenticatorBase::
    postCallbackFn(const ErrorCodeUserInfoStringPairCallbackFn& errorCodeUserInfoStringPairCallbackFn_,
                   const std::error_code& error_,
                   const boost::optional<std::pair<UserInfo,std::string> >& userInfoStringPair_)
    {
        std::function<void()> errorCodeUserInfoStringPairCallbackFnImpl = 
            std::bind(errorCodeUserInfoStringPairCallbackFn_, error_, userInfoStringPair_);
        _ioService.post(errorCodeUserInfoStringPairCallbackFnImpl);
    }

    void AuthenticatorBase::
    postCallbackFn(const ErrorCodeCallbackFn& errorCodeCallbackFn_,
                   const std::error_code& error_)
    {
        std::function<void()> errorCodeCallbackFnImpl = std::bind(errorCodeCallbackFn_,error_);
        _ioService.post(errorCodeCallbackFnImpl);
    }

    void AuthenticatorBase::
    postCallbackFn(const ErrorCodeStringCallbackFn& errorCodeStringCallbackFn_,
                   const std::error_code& error_,
                   const boost::optional<std::string>& string_)
    {
        std::function<void()> errorCodeStringCallbackFnImpl =
            std::bind(errorCodeStringCallbackFn_,error_,string_);
        _ioService.post(errorCodeStringCallbackFnImpl);
    }
    
}}
        
