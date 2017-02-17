#ifndef SANTIAGO_TEST_CLIENT_H
#define SANTIAGO_TEST_CLIENT_H


#include "../../Santiago/AppServer/ServerBase.h"
#include "../../Santiago/Utils/STLog.h"
#include "../../Santiago/User/ServerV1/ConnectionMessageSocket.h"

namespace Santiago { namespace Test { namespace Client
{
    class Client:public Santiago::AppServer::ServerBase<boost::asio::ip::tcp>
    {
    public:
         typedef std::function<void(unsigned)> OnDisconnectCallbackFn;
        Client()
            :_connectionMessageSocket(socketPtr_,
                                      std::bind(&Client::handleDisconnect,this),
                                      std::bind(
                                          &Client::handleConnectionMessageSocketMessage,
                                          this,
                                          std::placeholders::_1,
                                          std::placeholders::_2))
             void start();
        
        
    protected:
        virtual RequestHandlerBasePtr route(const std::string& documentURI_);
        void handleDisconnect();
        void handleConnectionMessageSocketMessage(const RequestId& requestId_, const ConnectionMessage& message_);
        
        ConnectionMessageSocket _connectionMessageSocket;
        
        
    }
        
}}}
    
