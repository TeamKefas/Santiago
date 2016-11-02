#ifndef SANTIAGO_EXAMPLES_SESSIONMANAGER_SIGNUPPAGEHANDLER_H
#define SANTIAGO_EXAMPLES_SESSIONMANAGER_SIGNUPPAGEHANDLER_H

#include <memory>
#include "RequestHandlerBase.h"

namespace SimpleAppServer
{
   class SignupPageHandler:public RequestHandlerBase
   {
   public:

       typedef RequestHandlerBase MyBase;
       typedef std::shared_ptr<SignupPageHandler> Ptr;

       SignupPageHandler(Santiago::User::ControllerBase& userController_):
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
