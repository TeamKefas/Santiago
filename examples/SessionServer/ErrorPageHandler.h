#ifndef SANTIAGO_EXAMPLES_SESSIONSERVER_ERRORPAGEHANDLER_H
#define SANTIAGO_EXAMPLES_SESSIONSERVER_ERRORPAGEHANDLER_H

#include "RequestHandlerBase.h"

namespace SessionServer
{
   class ErrorPageHandler:public RequestHandlerBase
   {
   public:

       ErrorPageHandler(Santiago::User::ControllerBase& userController_):
            RequestHandlerBase(userController_)
       {}

   protected:

       virtual void handleVerifiedRequest(const RequestPtr& request_,
                                          const std::string& userName_,
                                          const std::string& emailAddress_,
                                          const std::string& cookieString_);

       virtual void handleNonVerifiedRequest(const RequestPtr& request_);
   };

    void ErrorPageHandler::handleVerifiedRequest(const RequestPtr& request_,
                                                 const std::string& userName_,
                                                 const std::string& emailAddress_,
                                                 const std::string& cookieString_)
    {
        request_->setContentMIMEType(Santiago::MIMEType::TEXT);
        request_->out() << "Page not found... " << std::endl;
        request_->out() << "username ="<<userName_<<std::endl;
        printEcho();
        request_->setAppStatus(0);
        std::error_code error;
        request_->commit(error);
    }

    void ErrorPageHandler::handleNonVerifiedRequest(const RequestPtr& request_)
    {
        request_->setContentMIMEType(Santiago::MIMEType::TEXT);
        request_->out() << "Page not found... " << std::endl;
        printEcho();
        request_->setAppStatus(0);
        std::error_code error;
        request_->commit(error);
    }
}

#endif
