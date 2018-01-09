#include "ConnectionServer.h"

namespace Santiago{ namespace Authentication { namespace Server
{
    ConnectionServer::ConnectionServer(boost::asio::io_service& ioService_,
                                       unsigned port_,
                                       const OnDisconnectCallbackFn& onDisconnectCallbackFn_,
                                       const OnNewRequestCallbackFn& onNewRequestCallbackFn_)
        :_nextConnectionId(1)
        ,_acceptor(ioService_, tcp::endpoint(tcp::v4(),port_))
        ,_onDisconnectCallbackFn(onDisconnectCallbackFn_)
        ,_onNewRequestCallbackFn(onNewRequestCallbackFn_)
    {}
    
    void ConnectionServer::start()
    {
        ConnectionMessageSocket::MySocketPtr socketPtr
            (new ConnectionMessageSocket::MySocket(_acceptor.get_io_service()));
        _acceptor.async_accept(*socketPtr,
                               boost::bind(&ConnectionServer::handleAccept, this, socketPtr,
                                           boost::asio::placeholders::error));
    }

    void ConnectionServer::sendMessage(unsigned connectionId_,
                                       const ConnectionMessage& message_,
                                       bool isReplyExpectingMessage_,
                                       const boost::optional<OnReplyMessageCallbackFn>& onReplyMessageCallbackFn_)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        std::map<unsigned,ConnectionRequestsControllerPtr>::iterator iter =
            _idConnectionPtrMap.find(connectionId_);
        
        if(iter == _idConnectionPtrMap.end())
            return;
        
        iter->second->sendMessage(message_,true,onReplyMessageCallbackFn_);
        
    }

    void ConnectionServer::handleAccept(const ConnectionMessageSocket::MySocketPtr& socketPtr_,
                                        const boost::system::error_code& error_)
    {
        //TODO: See if an easier way to write exists.
        boost::asio::streambuf outputBuffer;
        std::ostream outStream(&outputBuffer);
        outStream.write(reinterpret_cast<const char*>(&_nextConnectionId),
                        sizeof(_nextConnectionId));
        
        boost::system::error_code errorCode;
        BOOST_ASSERT(socketPtr_);
        unsigned size = boost::asio::write(*socketPtr_,outputBuffer,errorCode);
        if(!errorCode)
        {
            return;
        }
        
        ConnectionRequestsControllerPtr newConnection(
            new ConnectionRequestsController(_ioService_,
                                             _socketPtr,
                                             _nextConnectionId,
                                             std::bind(ConnectionServer::handleDisconnect,this
                                                       _nextConnectionId),
                                             _onNewRequestCallbackFn));
        
        std::lock_guard<std::mutex> lock(_mutex);
        BOOST_ASSERT(_idConnectionPtrMap.find(_nextConnectionId) == _idConnectionPtrMap.end());
        _idConnectionPtrMap.insert(std::make_pair(_nextConnectionId,newConnection));
        //_idConnectionPtrMap[_nextConnectionId] = newConnection;
        // newConnection->start();
        ++_nextConnectionId;
    }

    void ConnectionServer::handleDisconnect(unsigned connectionId_)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        std::map<unsigned,ConnectionRequestsControllerPtr>::iterator iter =
            _idConnectionPtrMap.find(connectionId_);
        
        BOOST_ASSERT(iter == _idConnectionPtrMap.end());
        _idConnectionPtrMap.erase(iter);
        _onDisconnectCallbackFn(connectionId_);
    }
    
    
}}}
