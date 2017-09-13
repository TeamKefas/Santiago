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
            [&error_,&ret,handler,controllerStrandPair,userNameOrEmailAddress_,isUserNameNotEmailAddress_,password_](boost::asio::yield_context yield_)
            //NOTE:This yield_ is not same as above yield_
            {
                std::tie(error_,ret) = controllerStrandPair.first->
                    loginUser(userNameOrEmailAddress_,
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
            [controllerStrandPair,userNameOrEmailAddress_,isUserNameNotEmailAddress_,password_,postCallbackWrapper](boost::asio::yield_context yield_)
            {
                std::error_code error;
                boost::optional<std::pair<UserInfo,std::string> > userInfoStringPairOpt;
                
                std::tie(error,userInfoStringPairOpt) = controllerStrandPair.first->
                    loginUser(userNameOrEmailAddress_,
                              isUserNameNotEmailAddress_,
                              password_,
                              yield_);
                postCallbackWrapper(error,userInfoStringPairOpt);
            });        
    }

    template<typename Controller>
    void AuthenticatorBase<Controller>::
    logoutUserForCookie(const std::string& cookieString_,
                        boost::asio::yield_context yield_,
                        std::error_code& error_)
    {
        typename boost::asio::handler_type<boost::asio::yield_context, void()>::type
            handler(std::forward<boost::asio::yield_context>(yield_));
        
        boost::asio::async_result<decltype(handler)> result(handler);

        std::pair<ControllerPtr,StrandPtr> controllerStrandPair = getControllerAndStrandForString(cookieString_,true);
        
        controllerStrandPair.second->post(
            [&error_,handler,controllerStrandPair,cookieString_](boost::asio::yield_context yield_)
            //NOTE: This yield_ is not same as above yield_
            {
                error_ = controllerStrandPair.first->logoutUserForCookie(cookieString_,yield_);
                asio_handler_invoke(handler, &handler);
            });
        
        result.get();
    }

    template<typename Controller>
    void AuthenticatorBase<Controller>::logoutUserForCookie(const std::string& cookieString_,
                                                            const ErrorCodeCallbackFn& onLogoutCookieCallbackFn_)
    {
        ErrorCodeCallbackFn postCallbackWrapper(std::bind(static_cast<void(AuthenticatorBase::*)
                                                          (const ErrorCodeCallbackFn&, const std::error_code&)>
                                                          (&AuthenticatorBase::postCallbackFn),
                                                          this,
                                                          onLogoutCookieCallbackFn_,
                                                          std::placeholders::_1));
         std::pair<ControllerPtr,StrandPtr> controllerStrandPair = getControllerAndStrandForString(cookieString_,true);
         boost::asio::spawn(
             *controllerStrandPair->second,
             [controllerStrandPair,cookieString_,postCallbackWrapper]
             (boost::asio::yield_context yield_)
             //NOTE: This yield_ is not same as above yield_
             {
                 std::error_code error = controllerStrandPair->first->logoutUserForCookie(
                     cookieString_,
                     yield_);
                 postCallbackWrapper(error);
             });
    }

    template<typename Controller>
    void AuthenticatorBase<Controller>::
    logoutUserForAllCookies(const std::string& userName_,
                            boost::asio::yield_context yield_,
                            std::error_code& error_)
    {
        typename boost::asio::handler_type<boost::asio::yield_context, void()>::type
            handler(std::forward<boost::asio::yield_context>(yield_));
        
        boost::asio::async_result<decltype(handler)> result(handler);
        
        std::pair<ControllerPtr,StrandPtr> controllerStrandPair = getControllerAndStrandForString(userName_,true);
        
        controllerStrandPair.second->post(
            [&error_,handler,controllerStrandPair,userName_](boost::asio::yield_context yield_)
            //NOTE: This yield_ is not same as above yield_
            {
                error_ = controllerStrandPair.first->logoutUserForAllCookies(userName_,yield_);
                asio_handler_invoke(handler, &handler);
            });
        
        result.get();
    }

    template<typename Controller>
    void AuthenticatorBase<Controller>::
    logoutUserForAllCookies(const std::string& userName_,
                            const ErrorCodeCallbackFn& onLogoutAllCookiesCallbackFn_)
    {
        ErrorCodeCallbackFn postCallbackWrapper(std::bind(static_cast<void(AuthenticatorBase::*)
                                                          (const ErrorCodeCallbackFn&, const std::error_code&)>
                                                          (&AuthenticatorBase::postCallbackFn),
                                                          this,
                                                          onLogoutAllCookiesCallbackFn_,
                                                          std::placeholders::_1));
         std::pair<ControllerPtr,StrandPtr> controllerStrandPair = getControllerAndStrandForString(userName_,true);
         boost::asio::spawn(
             *controllerStrandPair->second,
             [controllerStrandPair,userName_,postCallbackWrapper]
             (boost::asio::yield_context yield_)
             //NOTE: This yield_ is not same as above yield_
             {
                 std::error_code error = controllerStrandPair->first->logoutUserForAllCookies(userName_,
                                                                                              yield_);
                 postCallbackWrapper(error);
             });
    }

    template<typename Controller>
    void AuthenticatorBase<Controller>::
    changeUserPassword(const std::string& cookieString_,
                       const std::string& oldPassword_,
                       const std::string& newPassword_,
                       boost::asio::yield_context yield_,
                       std::error_code& error_)
    {
        typename boost::asio::handler_type<boost::asio::yield_context, void()>::type
            handler(std::forward<boost::asio::yield_context>(yield_));
        
        boost::asio::async_result<decltype(handler)> result(handler);
        
        std::pair<ControllerPtr,StrandPtr> controllerStrandPair = getControllerAndStrandForString(cookieString_,true);
        
        controllerStrandPair.second->post(
            [&error_,handler,controllerStrandPair,cookieString_,oldPassword_,newPassword_](boost::asio::yield_context yield_)
            //NOTE: This yield_ is not same as above yield_
            {
                error_ = controllerStrandPair.first->changeUserPassword(cookieString_,oldPassword_,newPassword_,yield_);
                asio_handler_invoke(handler, &handler);
            });
        
        result.get();
    }

    template<typename Controller>
    void AuthenticatorBase<Controller>::
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
         std::pair<ControllerPtr,StrandPtr> controllerStrandPair = getControllerAndStrandForString(cookieString_,true);
         boost::asio::spawn(
             *controllerStrandPair->second,
             [controllerStrandPair,cookieString_,oldPassword_,newPassword_,postCallbackWrapper]
             (boost::asio::yield_context yield_)
             //NOTE: This yield_ is not same as above yield_
             {
                 std::error_code error = controllerStrandPair->first->changeUserPassword(
                     cookieString_,
                     oldPassword_,
                     newPassword_,
                     yield_);
                 postCallbackWrapper(error);
             });
    }
    
    template<typename Controller>
    void AuthenticatorBase<Controller>::
    changeUserEmailAddress(const std::string& cookieString_,
                           const std::string& newEmailAddress_,
                           const std::string& password_,
                           boost::asio::yield_context yield_,
                           std::error_code& error_)
    {
        typename boost::asio::handler_type<boost::asio::yield_context, void()>::type
            handler(std::forward<boost::asio::yield_context>(yield_));
        
        boost::asio::async_result<decltype(handler)> result(handler);
        
        std::pair<ControllerPtr,StrandPtr> controllerStrandPair = getControllerAndStrandForString(cookieString_,true);
        
        controllerStrandPair.second->post(
            [&error_,handler,controllerStrandPair,cookieString_,newEmailAddress_,password_](boost::asio::yield_context yield_)
            //NOTE: This yield_ is not same as above yield_
            {
                error_ = controllerStrandPair.first->changeUserEmailAddress(cookieString_,newEmailAddress_,password_,yield_);
                asio_handler_invoke(handler, &handler);
            });
        
        result.get();
    }

    template<typename Controller>
    void AuthenticatorBase<Controller>::
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
         std::pair<ControllerPtr,StrandPtr> controllerStrandPair = getControllerAndStrandForString(cookieString_,true);
         
         boost::asio::spawn(
             *controllerStrandPair->second,
             [controllerStrandPair,cookieString_,newEmailAddress_,password_,postCallbackWrapper]
             (boost::asio::yield_context yield_)
             //NOTE: This yield_ is not same as above yield_
             {
                 std::error_code error = controllerStrandPair->first->changeUserEmailAddress(
                     cookieString_,
                     newEmailAddress_,
                     password_,
                     yield_);
                 postCallbackWrapper(error);
             });
    }

    template<typename Controller>
    boost::optional<std::string> AuthenticatorBase<Controller>::
    createAndReturnRecoveryString(const std::string& emailAddress_,
                                  boost::asio::yield_context yield_,
                                  std::error_code& error_)
    {
        typename boost::asio::handler_type<boost::asio::yield_context, void()>::type
            handler(std::forward<boost::asio::yield_context>(yield_));
        
        boost::asio::async_result<decltype(handler)> result(handler);
        boost::optional<std::string> ret;

        std::pair<ControllerPtr,StrandPtr> controllerStrandPair = getControllerAndStrandForString(emailAddress_,false);
        
        boost::asio::spawn(
            *controllerStrandPair.second,
            [&error_,&ret,handler,controllerStrandPair,emailAddress_](boost::asio::yield_context yield_)
            //NOTE:This yield_ is not same as above yield_
            {
                std::tie(error_,ret) = controllerStrandPair.first->
                    createAndReturnRecoveryString(emailAddress_,
                                                  yield_);
                asio_handler_invoke(handler, &handler);
            });
        
        result.get();
        return ret;
    }

    template<typename Controller>
    void AuthenticatorBase<Controller>::createAndReturnRecoveryString(const std::string& emailAddress_,
                                                                      const ErrorCodeStringCallbackFn& onCreateAndReturnRecoveryStringCallbackFn_)
    {
        ErrorCodeStringCallbackFn postCallbackWrapper(std::bind(static_cast<void(AuthenticatorBase::*)
                                                                (const ErrorCodeStringCallbackFn&, const std::error_code&)>
                                                                (&AuthenticatorBase::postCallbackFn),
                                                                this,
                                                                onCreateAndReturnRecoveryStringCallbackFn_,
                                                                std::placeholders::_1,
                                                                std::placeholders::_2));
         std::pair<ControllerPtr,StrandPtr> controllerStrandPair = getControllerAndStrandForString(emailAddress_,false);
         boost::asio::spawn(
             *controllerStrandPair->second,
             [controllerStrandPair,emailAddress_,postCallbackWrapper]
             (boost::asio::yield_context yield_)
             //NOTE: This yield_ is not same as above yield_
             {
                 std::error_code error;
                 boost::optional<std::string> stringOpt;
                 std::tie(error,stringOpt) = controllerStrandPair->first->createAndReturnRecoveryString(
                     emailAddress_,
                     yield_);
                 postCallbackWrapper(error,stringOpt);
             });
    }
         
    template<typename Controller>
    boost::optional<std::string> AuthenticatorBase<Controller>::
    getUserForEmailAddressAndRecoveryString(const std::string& emailAddress_,
                                            const std::string& recoveryString_,
                                            boost::asio::yield_context yield_,
                                            std::error_code& error_)
    {
        typename boost::asio::handler_type<boost::asio::yield_context, void()>::type
            handler(std::forward<boost::asio::yield_context>(yield_));
        
        boost::asio::async_result<decltype(handler)> result(handler);
        boost::optional<std::string> ret;
        
         std::pair<ControllerPtr,StrandPtr> controllerStrandPair = getControllerAndStrandForString(emailAddress_,false);
        
        boost::asio::spawn(
            *controllerStrandPair.second,
            [&error_,&ret,handler,controllerStrandPair,emailAddress_,recoveryString_](boost::asio::yield_context yield_)
            //NOTE:This yield_ is not same as above yield_
            {
                std::tie(error_,ret) = controllerStrandPair.first->
                    getUserForEmailAddressAndRecoveryString(emailAddress_,
                                                            recoveryString_,
                                                            yield_);
                asio_handler_invoke(handler, &handler);
            });
        
        result.get();
        return ret;
    }

    template<typename Controller>
    void AuthenticatorBase<Controller>::getUserForEmailAddressAndRecoveryString(const std::string& emailAddress_,
                                                                                const std::string& recoveryString_,
                                                                                const ErrorCodeStringCallbackFn& onGetUserForEmailAddressAndRecoveryStringCallbackFn_)
    {
        ErrorCodeStringCallbackFn postCallbackWrapper(std::bind(static_cast<void(AuthenticatorBase::*)
                                                                (const ErrorCodeStringCallbackFn&, const std::error_code&)>
                                                                (&AuthenticatorBase::postCallbackFn),
                                                                this,
                                                                onGetUserForEmailAddressAndRecoveryStringCallbackFn_,
                                                                std::placeholders::_1,
                                                                std::placeholders::_2));
         std::pair<ControllerPtr,StrandPtr> controllerStrandPair = getControllerAndStrandForString(emailAddress_,false);
         boost::asio::spawn(
             *controllerStrandPair->second,
             [controllerStrandPair,emailAddress_,recoveryString_,postCallbackWrapper]
             (boost::asio::yield_context yield_)
             //NOTE: This yield_ is not same as above yield_
             {
                 std::error_code error;
                 boost::optional<std::string> stringOpt;
                 std::tie(error,stringOpt) = controllerStrandPair->first->getUserForEmailAddressAndRecoveryString(
                     emailAddress_,
                     recoveryString_,
                     yield_);
                 postCallbackWrapper(error,stringOpt);
             });
    }
/*
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
        
