#ifndef SANTIAGO_AUTHENTICATION_SERVER_CONNECTIONSERVER_H
#define SANTIAGO_AUTHENTICATION_SERVER_CONNECTIONSERVER_H

/**
 * @file ConnectionServer.h
 *
 * @section DESCRIPTION
 *
 * Contains the ConnectionServer class  
 */

#include <boost/asio/socket_acceptor_service.hpp>
#include <boost/asio/error.hpp>

#include "ConnectionRequestsController.h"
#include "Santiago/Authentication/MultiNode/ConnectionMessageSocket.h"
using boost::asio::ip::tcp;

namespace Santiago{ namespace Authentication { namespace MultiNode { namespace Server
{
    class ConnectionServer
    {
    public:

        typedef std::shared_ptr<ConnectionRequestsController> ConnectionRequestsControllerPtr;
        typedef std::function<void(const ConnectionMessage&)> OnNewRequestCallbackFn;
        typedef std::function<void(unsigned)> OnDisconnectCallbackFn;
        typedef std::function<void(const std::error_code&,
                                   const boost::optional<ConnectionMessage>&)> OnReplyMessageCallbackFn;
        
        /**
         * The constructor
         * @param ioService_- 
         * @param port_- ///NEED TO WRITE\\\
         * @param onDisconnectCallbackFn_ -
         * @param onNewRequestCallbackFn_ -
         */
        ConnectionServer(boost::asio::io_service& ioService_,
                         unsigned port_,
                         const OnDisconnectCallbackFn& onDisconnectCallbackFn_,
                         const OnNewRequestCallbackFn& onNewRequestCallbackFn_);
       /**
        * ///Message\\
        */
        void start();
       /**
        * ///Message\\
        */
        void sendMessage(unsigned connectionId_,
                         const ConnectionMessage& message_,
                         bool isReplyExpectingMessage_,
                         const boost::optional<OnReplyMessageCallbackFn>& onReplyMessageCallbackFn_);

        unsigned                                           _nextConnectionId;
        
    protected:
       /**
        * ///Message\\
        * @param socketPtr_ -
        * @param error_ - 
        */
        void handleAccept(const ConnectionMessageSocket::MySocketPtr& socketPtr_,
                          const boost::system::error_code& error_);
       /**
        * ///Message\\
        * @param connectionId_ - 
        */
        void handleDisconnect(unsigned connectionId_);

       
        boost::asio::io_service                           &_ioService;
        tcp::acceptor                                      _acceptor;

        OnDisconnectCallbackFn                             _onDisconnectCallbackFn;
        OnNewRequestCallbackFn                             _onNewRequestCallbackFn;
        OnReplyMessageCallbackFn                           _onReplyMessageCallbackFn;
        
        std::map<unsigned,ConnectionRequestsControllerPtr> _idConnectionPtrMap;

       
    };
}}}}
#endif
