#ifndef SLABS_APPSERVER_ECHOHANDLER_H
#define SLABS_APPSERVER_ECHOHANDLER_H

#include "RequestHandlerBase.h"

namespace Test{ namespace AppServer
{
    class EchoHandler:public RequestHandlerBase
    {
    public:
             
        EchoHandler(boost::asio::io_service& ioService_):
            RequestHandlerBase(ioService_)
        {}
        
    protected:
        
       void handleRequest(const RequestPtr& request_);
       
        
    };
    
}}

#endif
