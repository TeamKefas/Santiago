#ifndef SLABS_APPSERVER_REQUESTHANDLERBASE_H
#define SLABS_APPSERVER_REQUESTHANDLERBASE_H

#include <memory>

#include "../../Santiago/AppServer/RequestHandlerBase.h"



namespace Test{ namespace AppServer
{

    class RequestHandlerBase:public Santiago::AppServer::RequestHandlerBase<boost::asio::ip::tcp>
    {
    public:

        typedef Santiago::AppServer::RequestHandlerBase<boost::asio::ip::tcp> MyBase;
        typedef std::shared_ptr<RequestHandlerBase> Ptr;


        typedef typename MyBase::RequestPtr RequestPtr;
        
        RequestHandlerBase(boost::asio::io_service& ioService_):
            _ioService(ioService_)
        {}

        virtual void handleRequest(const RequestPtr& request_);

    protected:

        void printEcho(const RequestPtr& request_) const;


        boost::asio::io_service                         &_ioService;
        
    };

}}
#endif
