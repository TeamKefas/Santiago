#include <boost/asio.hpp>

#include "AuthenticatorBase.h"

namespace Santiago{ namespace Authentication
{
    boost::optional<std::pair<UserInfo,std::string> >
    AuthenticatorBase::asyncLoginUser(const std::string& userNameOrEmailAddress_,
                                      bool isUserNameNotEmailAddress_,
                                      const std::string& password_,
                                      boost::asio::yield_context&& yield_)
    {
        typename boost::asio::handler_type<boost::asio::yield_context, void()>::type
            handler(std::forward<boost::asio::yield_context>(yield_));
        
        boost::asio::async_result<decltype(handler)> result(handler);
        boost::optional<std::pair<UserInfo,std::string> > ret;
        std::error_code error;
        
        loginUser(userNameOrEmailAddress_,
                  isUserNameNotEmailAddress_,
                  password_,
                  [&error,&ret,handler](const std::error_code& error_,
                                        const boost::optional<std::pair<UserInfo,std::string> >& userInfo_)
                  {
                      error = error_;
                      ret = userInfo_;
                      asio_handler_invoke(handler, &handler);
                  });
        result.get();
        return ret;
    }


}}
        
