#ifndef SANTIAGO_AUTHENTICATION_SERVER_SERVER_H
#define SANTIAGO_AUTHENTICATION_SERVER_SERVER_H

/**
 * @file ServerV1.h
 *
 * @section DESCRIPTION
 *
 * Contains the Server class  
 */

#include <boost/asio/socket_acceptor_service.hpp>
#include <boost/asio/error.hpp>
#include <boost/optional.hpp>

#include "ConnectionServer.h"
#include "ServerImpl.h"

using boost::asio::ip::tcp;

namespace Santiago{ namespace Authentication{ namespace MultiNode{ namespace Server
{
    class Server
    {
    public:

        typedef std::shared_ptr<RequestHandlerBase> RequestHandlerBasePtr;
        
        /**
         * The constructor
         * @param ioService_- 
         * @param port_ -
         */
        Server(boost::asio::io_service& ioService_,unsigned port_);
       /**
        * ///Message\\
        */
        void start();

    protected:
       /**
        * ///Message\\
        * @param connectionId - 
        */
        void handleConnectionDisconnect(unsigned connectionId_);
       /**
        * ///Message\\
        * @param message_- 
        */
        void handleNewRequest(const ConnectionMessage& connectionMessage_);

        std::pair<ServerImplPtr,StrandPtr> getServerImplAndStrandForString(const std::string& string_,
                                                                           bool isNotEmailAddress_);

        boost::asio::io_service                          &_ioService;
        unsigned                                          _port;
        ConnectionServer                                  _connectionServer;
        std::array<std::pair<ServerImplPtr,StrandPtr>,26> _serverImplStrandPairArray;
    };
}}}}

#endif
