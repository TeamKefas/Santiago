#ifndef SANTIAGO_AUTHENTICATION_SERVER_H
#define SANTIAGO_AUTHENTICATION_SERVER_H


#include <boost/asio/socket_acceptor_service.hpp>
#include <boost/asio/error.hpp>
#include <boost/optional.hpp>

#include "ConnectionServer.h"
#include "ServerMessage.h"

#include "CreateUserRequestHandler.h"
#include "LoginUserRequestHandler.h"
#include "VerifyUserForCookieRequestHandler.h"
#include "LogoutUserForCookieRequestHandler.h"
#include "LogoutUserForAllCookiesRequestHandler.h"
#include "ChangeUserPasswordRequestHandler.h"
#include "RemovedCookieFromAppserverRequestHandler.h"
#include "AddResourceRequestHandler.h"





using boost::asio::ip::tcp;

namespace Santiago{ namespace Authentication
{
    class Server
    {
    public:

        typedef std::shared_ptr<RequestHandlerBase> RequestHandlerBasePtr;

        Server(boost::asio::io_service& ioService_,unsigned port_);

        void start();

    protected:
        
        void handleDisconnect(unsigned connectionId_);
        void handleRequestNew(const ServerMessage& message_);
        void handleRequestReply(const ServerMessage& message_);
        void handleRequestCompleted(const RequestId& requestId_);

        std::map<RequestId,RequestHandlerBasePtr>   _activeRequestHandlersList;
        boost::asio::io_service&                    _ioService;
        unsigned                                    _port;
        ConnectionMessage                           _connectionMessage;
        ConnectionServer                            _connectionServer;
    };
}}


#endif
