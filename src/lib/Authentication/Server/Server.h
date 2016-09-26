#ifndef SANTIAGO_AUTHENTICATION_SERVER_H
#define SANTIAGO_AUTHENTICATION_SERVER_H

//#include <boost/asio/acceptor.hpp>
#include <boost/asio/socket_acceptor_service.hpp>
#include <boost/asio/error.hpp>

#include "TCPConnection.h"
#include "UserController.h"
using boost::asio::ip::tcp;

namespace Santiago{ namespace Authentication
{
    class Server
    {
    public:

        typedef TCPConnection::Ptr TCPConnectionPtr;

        Server(boost::asio::io_service& ioService_,int port_);

        void start();

    protected:
        
        void handleAccept(const TCPConnection::MySocketPtr& socketPtr_,
                          const boost::system::error_code& error_);
        void handleDisconnect(unsigned connectionId_);
        void handleClientMessage(unsigned connectionId_,
                                 const ConnectionMessage& connectionMessage_);     
        void sendMessageCallbackFn(const ServerMessage& serverMessage_);
       
        
        tcp::acceptor                           _acceptor;
        std::map<unsigned,TCPConnectionPtr>     _idConnectionPtrMap;
        unsigned                                _nextConnectionId;
        UserController                          _userController;
        
    };
}}


#endif
