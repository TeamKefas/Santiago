#ifndef SLABS_APPSERVER_SERVER_H
#define SLABS_APPSERVER_SERVER_H

#include <boost/asio.hpp>

#include "../../Santiago/AppServer/ServerBase.h"

#include "RequestHandlerBase.h"
#include "EchoHandler.h"
#include "ErrorURIHandler.h"


namespace Test{ namespace AppServer
{
    class Server:public Santiago::AppServer::ServerBase<boost::asio::ip::tcp>
    {
    public:
        
        typedef Santiago::AppServer::ServerBase<boost::asio::ip::tcp> MyBase;

         Server(Santiago::LocalEndpoint<boost::asio::ip::tcp> listenEndpoint_):
            MyBase(listenEndpoint_)
        {
            
        }

        ~Server()
        {}
      
    protected:

        virtual RequestHandlerBasePtr route(const std::string& documentURI_)
        {
            RequestHandlerBasePtr ret;
            
            if(documentURI_ == "/helloworld.fcgi")
            {
                ret.reset(new EchoHandler(_ioService));
            }
            
            else
            {
                ret.reset(new ErrorURIHandler(_ioService));
            }
            return ret;
        }       

    };

}}

#endif
