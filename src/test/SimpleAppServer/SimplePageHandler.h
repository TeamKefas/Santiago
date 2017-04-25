#ifndef SANTIAGO_TEST_SIMPLEAPPSERVER_SIMPLEPAGEHANDLER_H
#define SANTIAGO_TEST_SIMPLEAPPSERVER_SIMPLEPAGEHANDLER_H

#include "RequestHandlerBase.h"

namespace SimpleAppServer
{
   class SimplePageHandler:public RequestHandlerBase
   {
   public:

       SimplePageHandler(Santiago::Authentication::ControllerBase& userController_):
            RequestHandlerBase(userController_)
       {}

   protected:

       virtual void handleVerifiedRequest(const RequestPtr& request_,
                                          const std::string& userName_,
                                          const std::string& emailAddress_,
                                          const std::string& cookieString_);

       virtual void handleNonVerifiedRequest(const RequestPtr& request_);

   };

}

#endif
