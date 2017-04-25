#ifndef SANTIAGO_TEST_SIMPLEAPPSERVER_SIGNUPHANDLER_H
#define SANTIAGO_TEST_SIMPLEAPPSERVER_SIGNUPHANDLER_H

#include <memory>
#include "RequestHandlerBase.h"

namespace SimpleAppServer
{
   class SignupHandler:public RequestHandlerBase
   {
   public:

       SignupHandler(Santiago::Authentication::ControllerBase& userController_):
            RequestHandlerBase(userController_)
       {}

   protected:

       void handleSignupUser(const RequestPtr& request_,
                             const std::string& userName_,
                             const std::string& emailAddress_,
                             std::error_code error_);

       virtual void handleVerifiedRequest(const RequestPtr& request_,
                                          const std::string& userName_,
                                          const std::string& emailAddress_,
                                          const std::string& cookieString_);

       virtual void handleNonVerifiedRequest(const RequestPtr& request_);

   };
}

#endif
