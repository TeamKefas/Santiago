#include "ServerV1.h"

namespace Santiago{ namespace User { namespace Server
{
    Server::Server(boost::asio::io_service& ioService_,unsigned port_)
        :_ioService(ioService_)
        ,_port(port_)
        ,_connectionMessage(nullptr,0)
        ,_connectionServer(_ioService
                           ,_port
                           ,std::bind(&Server::handleDisconnect,this,std::placeholders::_1)
                           ,std::bind(&Server::handleRequestNew,this,std::placeholders::_1)
                           ,std::bind(&Server::handleRequestReply,this,std::placeholders::_1))
    {}

    void Server::start()
    {
        _connectionServer.start();
    }
    
    
    void Server::handleDisconnect(unsigned connectionId_)
    {
        //TODO
    }

    void Server::handleRequestNew(const ServerMessage& message_)
    {
        RequestHandlerBasePtr requestHandlerPtr;
        switch(message_._connectionMessage->_type)
        {
            //Below is a non working example
        case ConnectionMessageType::CR_CREATE_USER:
            requestHandlerPtr.reset(new CreateUserRequestHandler(
                                        _serverData,
                                        std::bind(&ConnectionServer::sendMessage,
                                                  _connectionServer,
                                                  std::placeholders::_1),
                                        std::bind(&Server::handleRequestCompleted,this,std::placeholders::_1),
                                        message_));
            break;
        default:
            BOOST_ASSERT(false);
            break;

        }
        
        _activeRequestHandlersList.insert(std::make_pair(message_._requestId,requestHandlerPtr));
        requestHandlerPtr->start();
    }

    void Server::handleRequestReply(const ServerMessage& message_)
    {

        std::map<RequestId,RequestHandlerBasePtr>::iterator iter =
            _activeRequestHandlersList.find(message_._requestId);
        BOOST_ASSERT(iter != _activeRequestHandlersList.end());
        
    }

    void Server::handleRequestCompleted(const RequestId& requestId_)
    {
        std::map<RequestId,RequestHandlerBasePtr>::iterator iter =
            _activeRequestHandlersList.find(requestId_);
        //  _activeRequestHandlersList.find(message_._requestId);
        
        BOOST_ASSERT(iter == _activeRequestHandlersList.end());

        _activeRequestHandlersList.erase(iter);
    }


}}} //closing namespace Santiago::User::Server
