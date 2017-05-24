#include <boost/asio.hpp>

#include "AuthenticatorBase.h"

namespace Santiago{ namespace Authentication
{
    boost::optional<std::pair<UserInfo,std::string> >
    AuthenticatorBase::asyncLoginUser(const std::string& userNameOrEmailAddress_,
                                      bool isUserNameNotEmailAddress_,
                                      const std::string& password_,
                                      boost::asio::yield_context&& yield_,
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


    boost::optional<UserInfo> AuthenticatorBase::
    verifyCookieAndGetUserInfo(const std::string& cookieString_,
                               boost::asio::yield_context&& yield_,
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

}}
        
