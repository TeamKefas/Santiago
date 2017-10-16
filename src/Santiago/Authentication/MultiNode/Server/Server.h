#ifndef SANTIAGO_AUTHENTICATION_SERVER_SERVER_H
#define SANTIAGO_AUTHENTICATION_SERVER_SERVER_H

/**
 * @file Server.h
 *
 * @section DESCRIPTION
 *
 * Contains the Server class  
 */

//#include <boost/asio/acceptor.hpp>
#include <boost/asio/socket_acceptor_service.hpp>
#include <boost/asio/error.hpp>

#include "TCPConnection.h"
#include "UserController.h"
using boost::asio::ip::tcp;

namespace Santiago{ namespace Authentication { namespace Server
{
    class Server
    {
    public:

        typedef TCPConnection::Ptr TCPConnectionPtr;
        /**
         * The constructor
         * @param ioService_- 
         * @param port_ -
         */
        Server(boost::asio::io_service& ioService_,int port_);
       /**
        * ///Message\\
        */
        void start();

    protected:
       /**
        * ///Message\\
        * @param socketPtr - 
        * @param error_ -
        */
        void handleAccept(const TCPConnection::MySocketPtr& socketPtr_,
                          const boost::system::error_code& error_);
       /**
        * ///Message\\
        * @param connectionId - 
        */
        void handleDisconnect(unsigned connectionId_);
       /**
        * ///Message\\
        * @param connectionId - 
        * @param connectionMessage_ -
        */
        void handleClientMessage(unsigned connectionId_,
                                 const ConnectionMessage& connectionMessage_);
       /**
        * ///Message\\
        * @param serverMessage - 
        */
        void sendMessageCallbackFn(const ServerMessage& serverMessage_);
       
        
        tcp::acceptor                           _acceptor;
        std::map<unsigned,TCPConnectionPtr>     _idConnectionPtrMap;
        unsigned                                _nextConnectionId;
        UserController                          _userController;
        
    };
}}}


#endif
