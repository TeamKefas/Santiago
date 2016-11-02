#ifndef SANTIAGO_TEST_SESSIOMANAGER_SIMPLEPAGEHANDLER_H
#define SANTIAGO_TEST_SESSIOMANAGER_SIMPLEPAGEHANDLER_H

#include "RequestHandlerBase.h"

namespace SimpleAppServer
{
   class SimplePageHandler:public RequestHandlerBase
   {
   public:

       SimplePageHandler(Santiago::User::ControllerBase& userController_,
                         const std::string& documentURI_):
           RequestHandlerBase(userController_),
           _documentURI(documentURI_)
       {}

   protected:

       virtual void handleVerifiedRequest(const RequestPtr& request_,
                                          const std::string& userName_,
                                          const std::string& emailAddress_,
                                          const std::string& cookieString_);

       virtual void handleNonVerifiedRequest(const RequestPtr& request_);

       std::string                           _documentURI;
   };

}

#endif
