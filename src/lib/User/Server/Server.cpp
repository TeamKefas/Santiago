#include "Server.h"

namespace Santiago{ namespace Authentication
{
    Server::Server(boost::asio::io_service& ioService_,int port_)
        :_acceptor(ioService_, tcp::endpoint(tcp::v4(),port_))
        ,_nextConnectionId(1)
        ,_userController(std::bind(&Server::sendMessageCallbackFn,this,std::placeholders::_1))
    {}

    void Server::start()
    {
        TCPConnection::MySocketPtr socketPtr(new TCPConnection::MySocket(_acceptor.get_io_service()));
        _acceptor.async_accept(*socketPtr,
                               boost::bind(&Server::handleAccept, this, socketPtr,
                                           boost::asio::placeholders::error));
    }
    
    void Server::handleAccept(const TCPConnection::MySocketPtr& socketPtr_,
                              const boost::system::error_code& error_)
    {
        if (!error_)
        {
            std::function<void()> onDisconnectCallbackFn = 
                std::bind(&Server::handleDisconnect,this,_nextConnectionId);
            std::function<void(const ConnectionMessage)> onMessageCallbackFn = 
                std::bind(&Server::handleClientMessage,this,_nextConnectionId,std::placeholders::_1);
                
            TCPConnectionPtr newConnection(new TCPConnection(socketPtr_,onDisconnectCallbackFn,
                                                             onMessageCallbackFn));
            BOOST_ASSERT(_idConnectionPtrMap.find(_nextConnectionId) == _idConnectionPtrMap.end());
            _idConnectionPtrMap[_nextConnectionId] = newConnection;
            newConnection->start();
            ++_nextConnectionId;
        }
        
        start();
    }
    
    void Server::handleDisconnect(unsigned connectionId_)
    {
        _idConnectionPtrMap.erase(connectionId_);
    }    
    
    void Server::handleClientMessage(unsigned connectionId_,
                                     const ConnectionMessage& connectionMessage_)
    {
        //for now assume all messages are USER messages.
        // Later when implementing ResourceController check if they are USER or RESOURCE message
        ServerMessage serverMessage(connectionId_,connectionMessage_);
        _userController.handleClientMessage(serverMessage);
    }    
    
    void Server::sendMessageCallbackFn(const ServerMessage& serverMessage_)
    {
        //TODO:Instead of below assert need to return error code...decide if it will be 1 threaded first
        BOOST_ASSERT(_idConnectionPtrMap.find(serverMessage_._connectionId) != _idConnectionPtrMap.end());
        return _idConnectionPtrMap[serverMessage_._connectionId]->sendMessage(serverMessage_._connectionMessage);
    }

}}// closing Santiago::Authentication
