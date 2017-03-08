#ifndef SLABS_APPSERVER_ERRORURIHANDLER_H
#define SLABS_APPSERVER_ERRORURIHANDLER_H

#include "RequestHandlerBase.h"

namespace Test{ namespace AppServer
{
   class ErrorURIHandler:public RequestHandlerBase
   {
   public:

       ErrorURIHandler(Santiago::User::ControllerBase& userController_):
            RequestHandlerBase(userController_)
       {}

   protected:

       virtual void handleVerifiedRequest(const RequestPtr& request_,
                                          const std::string& userName_,
                                          const std::string& emailAddress_,
                                          const std::string& cookieString_);

       virtual void handleNonVerifiedRequest(const RequestPtr& request_);
   };

    void ErrorURIHandler::handleVerifiedRequest(const RequestPtr& request_,
                                                 const std::string& userName_,
                                                 const std::string& emailAddress_,
                                                 const std::string& cookieString_)
    {
        request_->setContentMIMEType(Santiago::MIMEType::TEXT);
        request_->out() << "Page not found... " << std::endl;
        request_->out() << "username ="<<userName_<<std::endl;
        printEcho(request_);
        request_->setAppStatus(0);
        std::error_code error;
        request_->commit(error);
    }

    void ErrorURIHandler::handleNonVerifiedRequest(const RequestPtr& request_)
    {
        request_->setContentMIMEType(Santiago::MIMEType::TEXT);
        request_->out() << "Page not found... " << std::endl;
        printEcho(request_);
        request_->setAppStatus(0);
        std::error_code error;
        request_->commit(error);
    }

}}

#endif
