#ifndef SANTIAGO_TEST_APPSERVER_LOGINHANDLER_H
#define SANTIAGO_TEST_APPSERVER_LOGINHANDLER_H

#include <memory>

#include "RequestHandlerBase.h"

namespace Test{ namespace AppServer
{
   class LoginUserHandler:public RequestHandlerBase
   {
   public:

       LoginUserHandler(Santiago::Authentication::AuthenticatorBase& userController_):
            RequestHandlerBase(userController_)
       {}

   protected:

       void handleLoginUser(const RequestPtr& request_,
                            const std::string& userName_,
                            std::error_code error_,
                            const boost::optional<std::pair<Santiago::Authentication::UserInfo,std::string> >& userInfoCookieStringPair_);

       virtual void handleVerifiedRequest(const RequestPtr& request_,
                                          boost::asio::yield_context yield_,
                                          const std::string& userName_,
                                          const std::string& emailAddress_,
                                          const std::string& cookieString_);

       virtual void handleNonVerifiedRequest(const RequestPtr& request_,
                                             boost::asio::yield_context yield_);

   };
    }}

#endif
