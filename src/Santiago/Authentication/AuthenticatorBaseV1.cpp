#include <ctime>
#include <cjose/jws.h>

#include <boost/asio.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "AuthenticatorBaseV1.h"

namespace Santiago{ namespace Authentication
{
    boost::optional<UserInfo> AuthenticatorBase::
    verifyCookieAndGetUserInfo(const std::string& cookieString_,
                               boost::asio::yield_context yield_,
                               std::error_code& error_)
    {

        //since coroutine is not considred work keeping the ioService from running out of work
        boost::asio::io_service::work work(_ioService);

#if BOOST_VERSION >= 106600
        boost::asio::async_completion<boost::asio::yield_context, void()> completion{ yield_ };
        auto& handler = completion.completion_handler;
        auto& result = completion.result;
#else        
        typename boost::asio::handler_type<boost::asio::yield_context, void()>::type
            handler(yield_);
        boost::asio::async_result<decltype(handler)> result(handler);
#endif
        
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

#if BOOST_VERSION >= 106600
        boost::asio::async_completion<boost::asio::yield_context, void()> completion{ yield_ };
        auto& handler = completion.completion_handler;
        auto& result = completion.result;
#else        
        typename boost::asio::handler_type<boost::asio::yield_context, void()>::type
            handler(yield_);
        boost::asio::async_result<decltype(handler)> result(handler);
#endif
        
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
#if BOOST_VERSION >= 106600
        boost::asio::async_completion<boost::asio::yield_context, void()> completion{ yield_ };
        auto& handler = completion.completion_handler;
        auto& result = completion.result;
#else        
        typename boost::asio::handler_type<boost::asio::yield_context, void()>::type
            handler(yield_);
        boost::asio::async_result<decltype(handler)> result(handler);
#endif        
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

    boost::optional<std::pair<UserInfo,std::string> > AuthenticatorBase::
    loginUserWithOICTokenId(const std::string& oicProviderName_,
                            const std::string& tokenIdString_,
                            boost::asio::yield_context yield_,
                            std::error_code& error_)
    {
        boost::optional<std::pair<JWSPtr,std::string> > tokenIdEmailIdPair =
            verifyTokenIdClaimsAndGetEmailId(tokenIdString_,
                                             oicProviderName_,
                                             error_);
        if(!tokenIdEmailIdPair)
        {
            return boost::none;
        }

#if BOOST_VERSION >= 106600
        boost::asio::async_completion<boost::asio::yield_context, void()> completion{ yield_ };
        auto& handler = completion.completion_handler;
        auto& result = completion.result;
#else        
        typename boost::asio::handler_type<boost::asio::yield_context, void()>::type
            handler(yield_);
        boost::asio::async_result<decltype(handler)> result(handler);
#endif
        boost::optional<std::pair<UserInfo,std::string> > ret;

        std::pair<AuthenticatorImplBasePtr,StrandPtr> authenticatorStrandPair =
            getAuthenticatorAndStrandForString(tokenIdEmailIdPair->second,false);

        boost::asio::spawn(
            *authenticatorStrandPair.second,
            [&error_,&ret,handler,this,authenticatorStrandPair,oicProviderName_,tokenIdEmailIdPair](boost::asio::yield_context yield_)
            {
                std::tie(error_,ret) = authenticatorStrandPair.first->
                    loginUserWithOICTokenId(oicProviderName_,
                                            tokenIdEmailIdPair->second,
                                            tokenIdEmailIdPair->first,
                                            yield_);
                asio_handler_invoke(handler, &handler);
            });
        
        result.get();
        return ret;
    }

    void AuthenticatorBase::loginUserWithOICTokenId(const std::string& oicProviderName_,
                                                    const std::string& tokenIdString_,
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

        std::error_code error;
        boost::optional<std::pair<JWSPtr,std::string> > tokenIdEmailIdPair =
            verifyTokenIdClaimsAndGetEmailId(tokenIdString_,
                                             oicProviderName_,
                                             error);
        if(!tokenIdEmailIdPair)
        {
            postCallbackWrapper(error,boost::none);
        }

        std::pair<AuthenticatorImplBasePtr,StrandPtr> authenticatorStrandPair =
            getAuthenticatorAndStrandForString(tokenIdEmailIdPair->second,false);

        boost::asio::spawn(
            *authenticatorStrandPair.second,
            [authenticatorStrandPair,this,oicProviderName_,tokenIdEmailIdPair,postCallbackWrapper](boost::asio::yield_context yield_)
            {
                std::error_code error;
                boost::optional<std::pair<UserInfo,std::string> > userInfoStringPairOpt;
                
                std::tie(error,userInfoStringPairOpt) = authenticatorStrandPair.first->
                    loginUserWithOICTokenId(oicProviderName_,
                                            tokenIdEmailIdPair->second,
                                            tokenIdEmailIdPair->first,
                                            yield_);
                postCallbackWrapper(error,userInfoStringPairOpt);
            });
    }
    
    void AuthenticatorBase::
    logoutUserForCookie(const std::string& cookieString_,
                        boost::asio::yield_context yield_,
                        std::error_code& error_)
    {
#if BOOST_VERSION >= 106600
        boost::asio::async_completion<boost::asio::yield_context, void()> completion{ yield_ };
        auto& handler = completion.completion_handler;
        auto& result = completion.result;
#else        
        typename boost::asio::handler_type<boost::asio::yield_context, void()>::type
            handler(yield_);
        boost::asio::async_result<decltype(handler)> result(handler);
#endif        

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
#if BOOST_VERSION >= 106600
        boost::asio::async_completion<boost::asio::yield_context, void()> completion{ yield_ };
        auto& handler = completion.completion_handler;
        auto& result = completion.result;
#else        
        typename boost::asio::handler_type<boost::asio::yield_context, void()>::type
            handler(yield_);
        boost::asio::async_result<decltype(handler)> result(handler);
#endif
        
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
#if BOOST_VERSION >= 106600
        boost::asio::async_completion<boost::asio::yield_context, void()> completion{ yield_ };
        auto& handler = completion.completion_handler;
        auto& result = completion.result;
#else        
        typename boost::asio::handler_type<boost::asio::yield_context, void()>::type
            handler(yield_);
        boost::asio::async_result<decltype(handler)> result(handler);
#endif 
       
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
#if BOOST_VERSION >= 106600
        boost::asio::async_completion<boost::asio::yield_context, void()> completion{ yield_ };
        auto& handler = completion.completion_handler;
        auto& result = completion.result;
#else        
        typename boost::asio::handler_type<boost::asio::yield_context, void()>::type
            handler(yield_);
        boost::asio::async_result<decltype(handler)> result(handler);
#endif        
        
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
#if BOOST_VERSION >= 106600
        boost::asio::async_completion<boost::asio::yield_context, void()> completion{ yield_ };
        auto& handler = completion.completion_handler;
        auto& result = completion.result;
#else        
        typename boost::asio::handler_type<boost::asio::yield_context, void()>::type
            handler(yield_);
        boost::asio::async_result<decltype(handler)> result(handler);
#endif

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
#if BOOST_VERSION >= 106600
        boost::asio::async_completion<boost::asio::yield_context, void()> completion{ yield_ };
        auto& handler = completion.completion_handler;
        auto& result = completion.result;
#else        
        typename boost::asio::handler_type<boost::asio::yield_context, void()>::type
            handler(yield_);
        boost::asio::async_result<decltype(handler)> result(handler);
#endif
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
#if BOOST_VERSION >= 106600
        boost::asio::async_completion<boost::asio::yield_context, void()> completion{ yield_ };
        auto& handler = completion.completion_handler;
        auto& result = completion.result;
#else        
        typename boost::asio::handler_type<boost::asio::yield_context, void()>::type
            handler(yield_);
        boost::asio::async_result<decltype(handler)> result(handler);
#endif

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
#if BOOST_VERSION >= 106600
        boost::asio::async_completion<boost::asio::yield_context, void()> completion{ yield_ };
        auto& handler = completion.completion_handler;
        auto& result = completion.result;
#else        
        typename boost::asio::handler_type<boost::asio::yield_context, void()>::type
            handler(yield_);
        boost::asio::async_result<decltype(handler)> result(handler);
#endif
        
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

    boost::optional<std::pair<JWSPtr,std::string> > AuthenticatorBase::
    verifyTokenIdClaimsAndGetEmailId(const std::string& tokenIdString_,
                                     const std::string& oicProviderName_,
                                     std::error_code& error_)
    {
        try
        {
            //get the property_tree from the JWS tokenId
            cjose_err error;
            error.code = CJOSE_ERR_NONE;

            JWSPtr tokenId(cjose_jws_import(tokenIdString_.c_str(),
                                            tokenIdString_.length(),
                                            &error),
                           [](JWS* ptr_)
                           {
                               cjose_jws_release(ptr_); //cjose_jws_release() handles NULL ptr
                           });
            if(!tokenId)
            {
                ST_ASSERT(error.code != CJOSE_ERR_NONE);
                ST_LOG_INFO("TokenId conversion to jws object failed at "<<error.file<<":"<<error.line
                            <<", message: "<< error.message <<std::endl);
                error_ = std::error_code(ErrorCode::ERR_TOKENID_VERIFICATION_FAILED);
                return boost::none;
            }
            
            char *jsonText = NULL; //dont call delete on this. delete when jwsPtr destroyed
            size_t length;
            bool flag = cjose_jws_get_plaintext(tokenId.get(),
                                                reinterpret_cast<uint8_t**>(&jsonText),
                                                &length,
                                                &error);
            if(!flag)
            {
                ST_LOG_INFO("Retrieving json claims for tokenId failed. oicProvider:" << oicProviderName_ <<" at "
                            << error.file<<":"<<error.line
                            <<", message:"<<error.message << std::endl);
                error_ = std::error_code(ErrorCode::ERR_TOKENID_VERIFICATION_FAILED);
                ST_ASSERT(error.code != CJOSE_ERR_NONE);
                return boost::none;
            }

            std::istringstream ss(std::string(jsonText,0,length));
            ST_LOG_INFO("TokenId claims in JSON: "<<ss.str());
            boost::property_tree::ptree tokenIdClaims;
            boost::property_tree::read_json(ss,tokenIdClaims);

            //load the config info to property tree for the oic provider
            boost::property_tree::ptree oicProviderData =
                _config.get_child("Santiago.Authentication.OICProviders").get_child(oicProviderName_);
            //check the issuer
            std::string iss = tokenIdClaims.get<std::string>("iss");
            std::string issExpected = oicProviderData.get<std::string>("iss");
            if( iss != issExpected)
            {
                ST_LOG_INFO( "Wrong issuer for OIC tokenId, oicProvider: "
                             << oicProviderName_
                             << " iss: "<< iss
                             << " iss expected: "<< issExpected << std::endl);
                error_ = std::error_code(ErrorCode::ERR_TOKENID_VERIFICATION_FAILED);
                return boost::none;
            }

            //check the aud
            std::string aud = tokenIdClaims.get<std::string>("aud");
            std::string audExpected = oicProviderData.get<std::string>("aud");
            if( aud != audExpected)
            {
                ST_LOG_INFO( "Wrong audience for OIC tokenId, aud:"
                             << aud
                             << " aud expected:"
                             << audExpected << std::endl);
                error_ = std::error_code(ErrorCode::ERR_TOKENID_VERIFICATION_FAILED);
                return boost::none;
            }

            //check the time stamps
            std::time_t currentTime = std::time(nullptr);
            long iat = tokenIdClaims.get<long>("iat");
            long exp = tokenIdClaims.get<long>("exp");
            boost::optional<long> nbfOpt = tokenIdClaims.get_optional<long>("nbf");
            
            ST_LOG_INFO("Time info: cur_time = " << currentTime
                        <<", iat = " << iat
                        <<", exp = " << exp
                        <<", nbf = " << (nbfOpt? *nbfOpt: 0) << std::endl);

            if( !(currentTime >= iat) ||
                !(currentTime < exp) ||
                (nbfOpt && !(currentTime >= *nbfOpt)) )
            {
                ST_LOG_INFO ("TokenId time checks failed");
                error_ = std::error_code(ErrorCode::ERR_TOKENID_VERIFICATION_FAILED);
                return boost::none;
            }
            
            boost::optional<std::string> emailIdOpt = tokenIdClaims.get_optional<std::string>("email");
            boost::optional<std::string> emailVerifiedOpt = tokenIdClaims.get_optional<std::string>("email_verified");
            if(!emailIdOpt || !emailVerifiedOpt || *emailVerifiedOpt != "true" )
            {
                ST_LOG_INFO("email with email_verfied=true not sent in the tokenId"<<std::endl);
                error_ = std::error_code(ErrorCode::ERR_TOKENID_VERIFICATION_FAILED);
                return boost::none;
            }
            
            error_ = std::error_code(ErrorCode::ERR_SUCCESS);
            return std::make_pair(tokenId,*emailIdOpt);
        }
        catch(std::exception& e)
        {
            error_ = std::error_code(ErrorCode::ERR_TOKENID_VERIFICATION_FAILED);
            ST_LOG_INFO("TokenId claims verification failed. Reason:"<<e.what());
        }
        catch(...)
        {
            error_ = std::error_code(ErrorCode::ERR_TOKENID_VERIFICATION_FAILED);
            ST_ASSERT(false);
        }
        
        return boost::none;
    }
    
}}
        
