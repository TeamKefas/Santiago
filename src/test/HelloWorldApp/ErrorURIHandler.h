#ifndef SLABS_APPSERVER_ERRORURIHANDLER_H
#define SLABS_APPSERVER_ERRORURIHANDLER_H

#include "RequestHandlerBase.h"

namespace Test{ namespace AppServer
{
   class ErrorURIHandler:public RequestHandlerBase
   {
   public:

       ErrorURIHandler(boost::asio::io_service& ioService_):
          RequestHandlerBase(ioService_) 
       {}

   protected:

       void handleRequest(const RequestPtr& request_)
       {
           request_->setContentMIMEType(Santiago::MIMEType::TEXT);
           request_->out()<<"Invalid "<<std::endl;
           request_->setAppStatus(0);
           std::error_code error;
           request_->commit(error);
       }
   };

}}

#endif
