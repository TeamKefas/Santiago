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
#include "ConnectionMessageSocket.h"
using boost::asio::ip::tcp;

namespace Santiago{ namespace Authentication { namespace Server
{
    class ConnectionServer
    {
    public:

        typedef ConnectionRequestsController::Ptr ConnectionRequestsControllerPtr;
        typedef std::function<void(const ServerMessage&)> OnNewRequestCallbackFn;
        typedef std::function<void(const ServerMessage&)> OnRequestReplyCallbackFn;
        typedef std::function<void(unsigned)> OnDisconnectCallbackFn;
        /**
         * The constructor
         * @param ioService_- 
         * @param port_- ///NEED TO WRITE\\\
         * @param onDisconnectCallbackFn_ -
         * @param onNewRequestCallbackFn_ -
         * @param onRequestReplyCallbackFn_ -
         */
        ConnectionServer(boost::asio::io_service& ioService_,
                         unsigned port_,
                         const OnDisconnectCallbackFn& onDisconnectCallbackFn_,
                         const OnNewRequestCallbackFn& onNewRequestCallbackFn_,
                         const OnRequestReplyCallbackFn& onRequestReplyCallbackFn_);
       /**
        * ///Message\\
        */
        void start();
       /**
        * ///Message\\
        * @param serverMessage_ - 
        */
        void sendMessage(const ServerMessage& serverMessage_);

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

       
        
        tcp::acceptor                                      _acceptor;

        OnDisconnectCallbackFn                             _onDisconnectCallbackFn;
        OnNewRequestCallbackFn                             _onNewRequestCallbackFn;
        OnRequestReplyCallbackFn                           _onRequestReplyCallbackFn;

        
        std::map<unsigned,ConnectionRequestsControllerPtr> _idConnectionPtrMap;

       
    };
}}}
#endif
