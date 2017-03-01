#include "ConnectionServer.h"

namespace Santiago{ namespace User { namespace Server
{
    ConnectionServer::ConnectionServer(boost::asio::io_service& ioService_,
                                       unsigned port_,
                                       const OnDisconnectCallbackFn& onDisconnectCallbackFn_,
                                       const OnNewRequestCallbackFn& onNewRequestCallbackFn_,
                                       const OnRequestReplyCallbackFn& onRequestReplyCallbackFn_)
        :_nextConnectionId(1)
        ,_acceptor(ioService_, tcp::endpoint(tcp::v4(),port_))
        ,_onDisconnectCallbackFn(onDisconnectCallbackFn_)
        ,_onNewRequestCallbackFn(onNewRequestCallbackFn_)
        ,_onRequestReplyCallbackFn(onRequestReplyCallbackFn_)
    {}
    
    void ConnectionServer::start()
    {
        ST_LOG_DEBUG("Starting ConnectionServer"<<std::endl);
        ConnectionMessageSocket::MySocketPtr socketPtr
            (new ConnectionMessageSocket::MySocket(_acceptor.get_io_service()));
        _acceptor.async_accept(*socketPtr,
                               boost::bind(&ConnectionServer::handleAccept, this, socketPtr,
                                           boost::asio::placeholders::error));
    }

    void ConnectionServer::sendMessage(const ServerMessage& serverMessage_)
    {
        ST_ASSERT(_idConnectionPtrMap.find(serverMessage_._connectionId) != _idConnectionPtrMap.end());
        _idConnectionPtrMap.find(serverMessage_._connectionId)->second->sendMessage(serverMessage_);
    }

    void ConnectionServer::handleAccept(const ConnectionMessageSocket::MySocketPtr& socketPtr_,
                                        const boost::system::error_code& error_)
    {
        ST_LOG_DEBUG("New connection received"<<std::endl);
        ConnectionRequestsControllerPtr newConnection(new ConnectionRequestsController(
                                                          socketPtr_,
                                                          std::bind(&ConnectionServer::handleDisconnect,
                                                                    this,
                                                                    std::placeholders::_1),
                                                          _onNewRequestCallbackFn,
                                                          _onRequestReplyCallbackFn,
                                                          _nextConnectionId));
        ST_ASSERT(_idConnectionPtrMap.find(_nextConnectionId) == _idConnectionPtrMap.end());
        _idConnectionPtrMap.insert(std::make_pair(_nextConnectionId,newConnection));
        //_idConnectionPtrMap[_nextConnectionId] = newConnection;
        // newConnection->start();
        ++_nextConnectionId;
    }

    void ConnectionServer::handleDisconnect(unsigned connectionId_)
    {
        ST_LOG_DEBUG("in handleDisconnect. connectionId ="<< connectionId_<<std::endl);

        std::map<unsigned,ConnectionRequestsControllerPtr>::iterator iter =
            _idConnectionPtrMap.find(connectionId_);
        
        ST_ASSERT(iter != _idConnectionPtrMap.end());
        _idConnectionPtrMap.erase(iter);
        _onDisconnectCallbackFn(connectionId_);
    }
    
    
}}}
