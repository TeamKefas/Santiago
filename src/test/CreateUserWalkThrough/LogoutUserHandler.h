#ifndef SANTIAGO_EXAMPLES_SESSIONSERVER_LOGOUTHANDLER_H
#define SANTIAGO_EXAMPLES_SESSIONSERVER_LOGOUTHANDLER_H

#include <memory>

#include "RequestHandlerBase.h"

namespace Test{ namespace AppServer
{
   class LogoutUserHandler:public RequestHandlerBase
   {
   public:

       LogoutUserHandler(Santiago::Authentication::AuthenticatorBase& userController_):
            RequestHandlerBase(userController_)
       {}

   protected:

       void handleLogoutUser(const RequestPtr& request_,
                             std::error_code error_);

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
