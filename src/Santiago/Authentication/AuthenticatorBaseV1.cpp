#include <boost/asio.hpp>

#include "AuthenticatorBaseV1.h"

namespace Santiago{ namespace Authentication
{
    template<typename Controller>
    boost::optional<UserInfo> AuthenticatorBase<Controller>::
    verifyCookieAndGetUserInfo(const std::string& cookieString_,
                               boost::asio::yield_context yield_,
                               std::error_code& error_)
    {
        typename boost::asio::handler_type<boost::asio::yield_context, void()>::type
            handler(std::forward<boost::asio::yield_context>(yield_));
        
        boost::asio::async_result<decltype(handler)> result(handler);
        boost::optional<UserInfo> ret;
        
        std::pair<ControllerPtr,StrandPtr> controllerStrandPair = getControllerAndStrandForString(cookieString_,true);

        boost::asio::spawn(
            *controllerStrandPair.second,
            [&error_,&ret,handler,controllerStrandPair,/*_clientId,*/cookieString_](boost::asio::yield_context yield_)
            //NOTE:This yield_ is not same as above yield_
            {
                std::tie(error_,ret) = controllerStrandPair.first->
                    verifyCookieAndGetUserInfo(/*_clientId,*/
                                               cookieString_,
                                               yield_);
                asio_handler_invoke(handler, &handler);
            });

        result.get();
        return ret;
    }

    template<typename Controller>
    void AuthenticatorBase<Controller>::
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

        std::pair<ControllerPtr,StrandPtr> controllerStrandPair = getControllerAndStrandForString(cookieString_,true);

        boost::asio::spawn(
            *controllerStrandPair.second,
            [controllerStrandPair,/*_clientId,*/cookieString_,postCallbackWrapper](boost::asio::yield_context yield_)
            {
                std::error_code error;
                boost::optional<UserInfo> userInfoOpt;
                
                std::tie(error,userInfoOpt) = controllerStrandPair.first->
                    verifyCookieAndGetUserInfo(/*_clientId,*/
                                               cookieString_,
                                               yield_);
                postCallbackWrapper(error,userInfoOpt);
            });        
    }

    template<typename Controller>
    void AuthenticatorBase<Controller>::createUser(const std::string& userName_,
                                                   const std::string& emailAddress_,
                                                   const std::string& password_,
                                                   boost::asio::yield_context yield_,
                                                   std::error_code& error_)
    {
        typename boost::asio::handler_type<boost::asio::yield_context, void()>::type
            handler(std::forward<boost::asio::yield_context>(yield_));
        
        boost::asio::async_result<decltype(handler)> result(handler);

        std::pair<ControllerPtr,StrandPtr> controllerStrandPair = getControllerAndStrandForString(userName_,true);

        controllerStrandPair.second->post(
            [&error_,handler,controllerStrandPair,userName_,emailAddress_,password_](boost::asio::yield_context yield_)
            //NOTE: This yield_ is not same as above yield_
            {
                error_ = controllerStrandPair.first->createUser(userName_,emailAddress_,password_,yield_);
                asio_handler_invoke(handler, &handler);
            });
        
        result.get();
    }

    template<typename Controller>
    void AuthenticatorBase<Controller>::createUser(const std::string& userName_,
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
         std::pair<ControllerPtr,StrandPtr> controllerStrandPair = getControllerAndStrandForString(userName_,true);
         boost::asio::spawn(
             *controllerStrandPair->second,
             [controllerStrandPair,userName_,emailAddress_,password_,postCallbackWrapper]
             (boost::asio::yield_context yield_)
             //NOTE: This yield_ is not same as above yield_
             {
                 std::error_code error = controllerStrandPair->first->createUser(
                     userName_,
                     emailAddress_,
                     password_,
                     yield_);
                 postCallbackWrapper(error);
             });
    }

    template<typename Controller>
    boost::optional<std::pair<UserInfo,std::string> > AuthenticatorBase<Controller>::
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

        std::pair<ControllerPtr,StrandPtr> controllerStrandPair = getControllerAndStrandForString(userNameOrEmailAddress_,isUserNameNotEmailAddress_);

        boost::asio::spawn(
            *controllerStrandPair.second,
            [&error_,&ret,handler,controllerStrandPair,/*_clientId,*/userNameOrEmailAddress_,isUserNameNotEmailAddress_,password_](boost::asio::yield_context yield_)
            //NOTE:This yield_ is not same as above yield_
            {
                std::tie(error_,ret) = controllerStrandPair.first->
                    loginUser(/*_clientId,*/
                              userNameOrEmailAddress_,
                              isUserNameNotEmailAddress_,
                              password_,
                              yield_);
                asio_handler_invoke(handler, &handler);
            });
        
        result.get();
        return ret;
    }

    template<typename Controller>
    void AuthenticatorBase<Controller>::loginUser(const std::string& userNameOrEmailAddress_,
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

        std::pair<ControllerPtr,StrandPtr> controllerStrandPair = getControllerAndStrandForString(userNameOrEmailAddress_,isUserNameNotEmailAddress_);

        boost::asio::spawn(
            *controllerStrandPair.second,
            [controllerStrandPair,/*_clientId,*/userNameOrEmailAddress_,isUserNameNotEmailAddress_,password_,postCallbackWrapper](boost::asio::yield_context yield_)
            {
                std::error_code error;
                boost::optional<std::pair<UserInfo,std::string> > userInfoStringPairOpt;
                
                std::tie(error,userInfoStringPairOpt) = controllerStrandPair.first->
                    loginUser(/*_clientId,*/
                              userNameOrEmailAddress_,
                              isUserNameNotEmailAddress_,
                              password_,
                              yield_);
                postCallbackWrapper(error,userInfoStringPairOpt);
            });        
    }
/*
    void AuthenticatorBase::logoutUserForCookie(const std::string& cookieString_,
                                                boost::asio::yield_context yield_,
                                                std::error_code& error_)
    {
        typename boost::asio::handler_type<boost::asio::yield_context, void()>::type
            handler(std::forward<boost::asio::yield_context>(yield_));
        
        boost::asio::async_result<decltype(handler)> result(handler);
        
        logoutUserForCookieImpl(cookieString_,
                                [&error_,handler](const std::error_code& ec_)
                                {
                                    error_ = ec_;
                                    asio_handler_invoke(handler, &handler);
                                });
        
        result.get();
    }

    void AuthenticatorBase::logoutUserForAllCookies(const std::string& userName_,
                                                     boost::asio::yield_context yield_,
                                                     std::error_code& error_)
    {
        typename boost::asio::handler_type<boost::asio::yield_context, void()>::type
            handler(std::forward<boost::asio::yield_context>(yield_));
        
        boost::asio::async_result<decltype(handler)> result(handler);
        
        logoutUserForAllCookiesImpl(userName_,
                                    [&error_,handler](const std::error_code& ec_)
                                    {
                                        error_ = ec_;
                                        asio_handler_invoke(handler, &handler);
                                    });
        
        result.get();
    }

    void AuthenticatorBase::changeUserPassword(const std::string& cookieString_,
                                               const std::string& oldPassword_,
                                               const std::string& newPassword_,
                                               boost::asio::yield_context yield_,
                                               std::error_code& error_  )
    {
        typename boost::asio::handler_type<boost::asio::yield_context, void()>::type
            handler(std::forward<boost::asio::yield_context>(yield_));
        
        boost::asio::async_result<decltype(handler)> result(handler);
        
        changeUserPasswordImpl(cookieString_,
                               oldPassword_,
                               newPassword_,
                               [&error_,handler](const std::error_code& ec_)
                               {
                                   error_ = ec_;
                                   asio_handler_invoke(handler, &handler);
                               });
        
        result.get();
    }

    void AuthenticatorBase::changeUserEmailAddress(const std::string& cookieString_,
                                                   const std::string& newEmailAddress_,
                                                   const std::string& password_,
                                                   boost::asio::yield_context yield_,
                                                   std::error_code& error_)
    {
        typename boost::asio::handler_type<boost::asio::yield_context, void()>::type
            handler(std::forward<boost::asio::yield_context>(yield_));
        
        boost::asio::async_result<decltype(handler)> result(handler);
        
        changeUserEmailAddressImpl(cookieString_,
                                   newEmailAddress_,
                                   password_,
                                   [&error_,handler](const std::error_code& ec_)
                                   {
                                       error_ = ec_;
                                       asio_handler_invoke(handler, &handler);
                                   });
        
        result.get();
    }

    boost::optional<std::string> AuthenticatorBase::createAndReturnRecoveryString(const std::string& emailAddress_,
                                                                                  boost::asio::yield_context yield_,
                                                                                  std::error_code& error_)
    {
        typename boost::asio::handler_type<boost::asio::yield_context, void()>::type
            handler(std::forward<boost::asio::yield_context>(yield_));
        
        boost::asio::async_result<decltype(handler)> result(handler);
        boost::optional<std::string> ret;
        
        createAndReturnRecoveryStringImpl(
            emailAddress_,
            [&error_,&ret,handler](const std::error_code& ec_,
                                   const boost::optional<std::string>& recoveryStringOpt_)
            {
                error_ = ec_;
                ret = recoveryStringOpt_;
                asio_handler_invoke(handler, &handler);
            });
        
        result.get();
        return ret;
    }
         

    boost::optional<std::string> AuthenticatorBase::getUserForEmailAddressAndRecoveryString(
        const std::string& emailAddress_,
        const std::string& recoverystring_,
        boost::asio::yield_context yield_,
        std::error_code& error_)
    {
        typename boost::asio::handler_type<boost::asio::yield_context, void()>::type
            handler(std::forward<boost::asio::yield_context>(yield_));
        
        boost::asio::async_result<decltype(handler)> result(handler);
        boost::optional<std::string> ret;
        
        getUserForEmailAddressAndRecoveryStringImpl(
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

    void AuthenticatorBase::changeUserPasswordForEmailAddressAndRecoveryString(const std::string& emailAddress_,
                                                                               const std::string& recoveryString_,
                                                                               const std::string& newPassword_,
                                                                               boost::asio::yield_context yield_,
                                                                               std::error_code& error_)
    {
        typename boost::asio::handler_type<boost::asio::yield_context, void()>::type
            handler(std::forward<boost::asio::yield_context>(yield_));
        
        boost::asio::async_result<decltype(handler)> result(handler);
        
        changeUserPasswordForEmailAddressAndRecoveryStringImpl(emailAddress_,
                                                               recoveryString_,
                                                               newPassword_,
                                                               [&error_,handler](const std::error_code& ec_)
                                                               {
                                                                   error_ = ec_;
                                                                   asio_handler_invoke(handler, &handler);
                                                               });
        
        result.get();
    }
        
    
    void AuthenticatorBase::deleteUser(const std::string& cookieString_,
                                       boost::asio::yield_context yield_,
                                       std::error_code& error_)
    {
        typename boost::asio::handler_type<boost::asio::yield_context, void()>::type
            handler(std::forward<boost::asio::yield_context>(yield_));
        
        boost::asio::async_result<decltype(handler)> result(handler);
        
        deleteUserImpl(cookieString_,
                       [&error_,handler](const std::error_code& ec_)
                       {
                           error_ = ec_;
                           asio_handler_invoke(handler, &handler);
                       });
        
        result.get();
    }
     
    void AuthenticatorBase::loginUser(const std::string& userNameOrEmailAddress_,
                                      bool isUserNameNotEmailAddress_,
                                      const std::string& password_,
                                      const ErrorCodeUserInfoStringPairCallbackFn& onLoginUserCallbackFn_)
    {
        ErrorCodeUserInfoStringPairCallbackFn postCallbackWrapper(std::bind(static_cast<void(AuthenticatorBase::*)
                                                                            (const ErrorCodeUserInfoStringPairCallbackFn&,
                                                                             const std::error_code&,
                                                                             const boost::optional<std::pair<UserInfo,std::string> >&)>
                                                                            (&AuthenticatorBase::postCallbackFn),
                                                                            this,
                                                                            onLoginUserCallbackFn_,
                                                                            std::placeholders::_1,
                                                                            std::placeholders::_2));
        
        _strand.post(std::bind(&AuthenticatorBase::loginUserImpl,
                               this,
                               userNameOrEmailAddress_,
                               isUserNameNotEmailAddress_,
                               password_,
                               postCallbackWrapper));
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
        
        _strand.post(std::bind(&AuthenticatorBase::logoutUserForCookieImpl,
                               this,
                               cookieString_,
                               postCallbackWrapper));
    }

    void AuthenticatorBase::logoutUserForAllCookies(const std::string& userName_,
                                                    const ErrorCodeCallbackFn& onLogoutAllCookiesCallbackFn_)
    {
        ErrorCodeCallbackFn postCallbackWrapper(std::bind(static_cast<void(AuthenticatorBase::*)
                                                          (const ErrorCodeCallbackFn&, const std::error_code&)>
                                                          (&AuthenticatorBase::postCallbackFn),
                                                          this,
                                                          onLogoutAllCookiesCallbackFn_,
                                                          std::placeholders::_1));
        
        _strand.post(std::bind(&AuthenticatorBase::logoutUserForAllCookiesImpl,
                               this,
                               userName_,
                               postCallbackWrapper));
    }

    void AuthenticatorBase::changeUserPassword(const std::string& cookieString_,
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
        
        _strand.post(std::bind(&AuthenticatorBase::changeUserPasswordImpl,
                               this,
                               cookieString_,
                               oldPassword_,
                               newPassword_,
                               postCallbackWrapper));
    }

     void AuthenticatorBase::changeUserEmailAddress(const std::string& cookieString_,
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
         
         _strand.post(std::bind(&AuthenticatorBase::changeUserEmailAddressImpl,
                                this,
                                cookieString_,
                                newEmailAddress_,
                                password_,
                                postCallbackWrapper));
     }
    
    void AuthenticatorBase::createAndReturnRecoveryString(const std::string& emailAddress_,
                                                          const ErrorCodeStringCallbackFn& onCreateAndReturnRecoveryStringCallbackFn_)
    {
        ErrorCodeStringCallbackFn postCallbackWrapper(std::bind(static_cast<void(AuthenticatorBase::*)
                                                                (const ErrorCodeStringCallbackFn&,
                                                                 const std::error_code&,
                                                                 const boost::optional<std::string>& )>
                                                                (&AuthenticatorBase::postCallbackFn),
                                                                this,
                                                                onCreateAndReturnRecoveryStringCallbackFn_,
                                                                std::placeholders::_1,
                                                                std::placeholders::_2));
        
        _strand.post(std::bind(&AuthenticatorBase::createAndReturnRecoveryStringImpl,
                               this,
                               emailAddress_,
                               postCallbackWrapper));
    }

    void AuthenticatorBase::getUserForEmailAddressAndRecoveryString(const std::string& emailAddress_,
                                                                    const std::string& recoveryString_,
                                                                    const ErrorCodeStringCallbackFn& onGetUserForEmailAddressAndRecoveryStringCallbackFn_)
    {
        ErrorCodeStringCallbackFn postCallbackWrapper(std::bind(static_cast<void(AuthenticatorBase::*)
                                                                (const ErrorCodeStringCallbackFn&,
                                                                 const std::error_code&,
                                                                 const boost::optional<std::string>& )>
                                                                (&AuthenticatorBase::postCallbackFn),
                                                                this,
                                                                onGetUserForEmailAddressAndRecoveryStringCallbackFn_,
                                                                std::placeholders::_1,
                                                                std::placeholders::_2));
         
        _strand.post(std::bind(&AuthenticatorBase::getUserForEmailAddressAndRecoveryStringImpl,
                               this,
                               emailAddress_,
                               recoveryString_,
                               postCallbackWrapper));
    }

    void AuthenticatorBase::changeUserPasswordForEmailAddressAndRecoveryString(const std::string& emailAddress_,
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
        
        _strand.post(std::bind(&AuthenticatorBase::changeUserPasswordForEmailAddressAndRecoveryStringImpl,
                               this,
                               emailAddress_,
                               recoveryString_,
                               newPassword_,
                               postCallbackWrapper));
    }

    
    void AuthenticatorBase::deleteUser(const std::string& cookieString_,
                                       const ErrorCodeCallbackFn& onDeleteUserCallbackFn_)
    {
        ErrorCodeCallbackFn postCallbackWrapper(std::bind(static_cast<void(AuthenticatorBase::*)
                                                          (const ErrorCodeCallbackFn&, const std::error_code&)>
                                                          (&AuthenticatorBase::postCallbackFn),
                                                          this,
                                                          onDeleteUserCallbackFn_,
                                                          std::placeholders::_1));
         
        _strand.post(std::bind(&AuthenticatorBase::deleteUserImpl,
                               this,
                               cookieString_,
                               postCallbackWrapper));
    }
*/
    template<typename Controller>
    void AuthenticatorBase<Controller>::postCallbackFn(const ErrorCodeUserInfoCallbackFn& errorCodeUserInfoCallbackFn_,
                                                       const std::error_code& error_,
                                                       const boost::optional<UserInfo>& userInfoOpt_)
    {
        std::function<void()> errorCodeUserInfoCallbackFnImpl =
            std::bind(errorCodeUserInfoCallbackFn_,error_, userInfoOpt_);
        _ioService.post(errorCodeUserInfoCallbackFnImpl);
    }

    template<typename Controller>
    void AuthenticatorBase<Controller>::
    postCallbackFn(const ErrorCodeUserInfoStringPairCallbackFn& errorCodeUserInfoStringPairCallbackFn_,
                   const std::error_code& error_,
                   const boost::optional<std::pair<UserInfo,std::string> >& userInfoStringPair_)
    {
        std::function<void()> errorCodeUserInfoStringPairCallbackFnImpl = 
            std::bind(errorCodeUserInfoStringPairCallbackFn_, error_, userInfoStringPair_);
        _ioService.post(errorCodeUserInfoStringPairCallbackFnImpl);
    }

    template<typename Controller>
    void AuthenticatorBase<Controller>::
    postCallbackFn(const ErrorCodeCallbackFn& errorCodeCallbackFn_,
                   const std::error_code& error_)
    {
        std::function<void()> errorCodeCallbackFnImpl = std::bind(errorCodeCallbackFn_,error_);
        _ioService.post(errorCodeCallbackFnImpl);
    }

    template<typename Controller>
    void AuthenticatorBase<Controller>::
    postCallbackFn(const ErrorCodeStringCallbackFn& errorCodeStringCallbackFn_,
                   const std::error_code& error_,
                   const boost::optional<std::string>& string_)
    {
        std::function<void()> errorCodeStringCallbackFnImpl =
            std::bind(errorCodeStringCallbackFn_,error_,string_);
        _ioService.post(errorCodeStringCallbackFnImpl);
    }
    
}}
        
