#ifndef SANTIAGO_TEST_SIMPLEAPPSERVER_LOGINPAGEHANDLER_H
#define SANTIAGO_TEST_SIMPLEAPPSERVER_LOGINPAGEHANDLER_H

#include <memory>

#include "RequestHandlerBase.h"

namespace SimpleAppServer
{
   class LoginPageHandler:public RequestHandlerBase
   {
   public:

       LoginPageHandler(Santiago::User::ControllerBase& userController_):
            RequestHandlerBase(userController_)
       {}

   protected:

       void handleLoginUser(const RequestPtr& request_,
                            const std::string& userName_,
                            std::error_code error_,
                            const std::string& cookieString_);

       virtual void handleVerifiedRequest(const RequestPtr& request_,
                                          const std::string& userName_,
                                          const std::string& cookieString_);

       virtual void handleNonVerifiedRequest(const RequestPtr& request_);

   };

}

#endif
