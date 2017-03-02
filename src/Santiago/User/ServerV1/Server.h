#ifndef SANTIAGO_USER_SERVER_SERVER_H
#define SANTIAGO_USER_SERVER_SERVER_H

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
#include <boost/property_tree/ptree.hpp>

#include "../../ThreadSpecificVar/ThreadSpecificVar.h"

#include "ConnectionServer.h"
#include "RequestHandlerBase.h"
#include "ServerData.h"
#include "ServerMessage.h"

#include "../../SantiagoDBTables/MariaDBConnection.h"

using boost::asio::ip::tcp;

namespace Santiago{ namespace User { namespace Server
{
    class Server
    {
    public:

        typedef ThreadSpecificVar::ThreadSpecificVar<Santiago::SantiagoDBTables::MariaDBConnection> SantiagoDBConnection;
        typedef std::shared_ptr<RequestHandlerBase> RequestHandlerBasePtr;
        
        /**
         * The constructor
         * @param ioService_- 
         * @param port_ -
         */
        Server(const boost::property_tree::ptree& config_);
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
        void handleNewRequest(const ServerMessage& message_);
       /**
        * ///Message\\
        * @param message_- 
        */
        void handleReplyRequest(const ServerMessage& message_);
       /**
        * ///Message\\
        * @param requestId_- 
        */
        void handleRequestCompleted(const RequestId& requestId_);

        boost::asio::io_service                     _ioService;
        boost::property_tree::ptree                 _config;
        SantiagoDBConnection                        _databaseConnection;

        std::map<RequestId,RequestHandlerBasePtr>   _activeRequestHandlersList;
        ServerData                                  _serverData;
        ConnectionServer                            _connectionServer;
    };
}}}


#endif
