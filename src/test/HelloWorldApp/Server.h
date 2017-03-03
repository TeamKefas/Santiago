#ifndef SLABS_APPSERVER_SERVER_H
#define SLABS_APPSERVER_SERVER_H

#include <boost/asio.hpp>

#include <boost/property_tree/ptree.hpp>
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

        Server(const boost::property_tree::ptree& config_):
            MyBase(getServerLocalEndpoint(config_)),
            _config(config_)
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

       
        
        Santiago::LocalEndpoint<boost::asio::ip::tcp> 
        getServerLocalEndpoint(const boost::property_tree::ptree& config_) const 
        {
            //unsigned portNo = config_.get<unsigned>("Santiago.AppServer.port");

            return Santiago::LocalEndpoint<typename boost::asio::ip::tcp>
                (config_.get<unsigned>("Santiago.AppServer.port"));
        }

        boost::property_tree::ptree                         _config;

    };

}}

#endif
