#ifndef SANTIAGO_TEST_APPSERVER_ASYNCLOGOUTFORALLCOOKIEHANDLER_H
#define SANTIAGO_TEST_APPSERVER_ASYNCLOGOUTFORALLCOOKIEHANDLER_H

#include <memory>

#include "RequestHandlerBase.h"

namespace Test{ namespace AppServer
{
   class AsyncLogoutUserForAllCookieHandler:public RequestHandlerBase
   {
   public:
       
       AsyncLogoutUserForAllCookieHandler(Santiago::Authentication::AuthenticatorBase& userController_):
           RequestHandlerBase(userController_)
       {}

   protected:
       
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
