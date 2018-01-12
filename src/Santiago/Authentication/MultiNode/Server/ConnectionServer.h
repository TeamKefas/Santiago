#ifndef SANTIAGO_AUTHENTICATION_SERVER_CONNECTIONSERVER_H
#define SANTIAGO_AUTHENTICATION_SERVER_CONNECTIONSERVER_H

/**
 * @file ConnectionServer.h
 *
 * @section DESCRIPTION
 *
 * Contains the ConnectionServer class  
 */

#include <mutex>

#include <boost/asio/socket_acceptor_service.hpp>
#include <boost/asio/error.hpp>

#include "ConnectionRequestsController.h"
#include "../ConnectionMessageSocket.h"
using boost::asio::ip::tcp;

namespace Santiago{ namespace Authentication { namespace MultiNode { namespace Server
{
    class ConnectionServer
    {
    public:

        typedef std::shared_ptr<Authentication::MultiNode::Server::ConnectionRequestsController> ConnectionRequestsControllerPtr;
        typedef std::function<void(const Authentication::MultiNode::ConnectionMessage&)> OnNewRequestCallbackFn;
        typedef std::function<void(unsigned)> OnDisconnectCallbackFn;
        typedef std::function<void(const std::error_code&,
                                   const boost::optional<Santiago::Authentication::MultiNode::ConnectionMessage>&)> OnReplyMessageCallbackFn;
        
        ConnectionServer(boost::asio::io_service& ioService_,
                         unsigned port_,
                         const OnDisconnectCallbackFn& onDisconnectCallbackFn_,
                         const OnNewRequestCallbackFn& onNewRequestCallbackFn_);

        void start();

        void sendMessage(unsigned connectionId_,
                         const Authentication::MultiNode::ConnectionMessage& message_,
                         bool isReplyExpectingMessage_,
                         const boost::optional<OnReplyMessageCallbackFn>& onReplyMessageCallbackFn_);

        unsigned                                           _nextConnectionId;
        
    protected:

        void handleAccept(const Authentication::MultiNode::ConnectionMessageSocket::MySocketPtr& socketPtr_,
                          const boost::system::error_code& error_);

        void handleDisconnect(unsigned connectionId_);

       
        boost::asio::io_service                           &_ioService;
        tcp::acceptor                                      _acceptor;

        OnDisconnectCallbackFn                             _onDisconnectCallbackFn;
        OnNewRequestCallbackFn                             _onNewRequestCallbackFn;
        
        std::map<unsigned,ConnectionRequestsControllerPtr> _idConnectionPtrMap;
        std::mutex                                         _mutex; //for using _idConnectionPtrMap

       
    };
}}}}
#endif
