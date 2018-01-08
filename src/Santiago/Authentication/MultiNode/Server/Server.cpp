#include "CreateUserRequestHandler.h"
#include "LoginUserRequestHandler.h"
#include "VerifyUserForCookieRequestHandler.h"
#include "LogoutUserForCookieRequestHandler.h"

#include "LogoutUserForAllCookiesRequestHandler.h"
#include "ChangeUserPasswordRequestHandler.h"
#include "RemovedCookieFromAppserverRequestHandler.h"
#include "AddResourceRequestHandler.h"

#include "ServerV1.h"

namespace Santiago{ namespace User { namespace Server
{
    Server::Server(boost::asio::io_service& ioService_,unsigned port_)
        :_ioService(ioService_)
        ,_port(port_)
        ,_connectionServer(_ioService
                           ,_port
                           ,std::bind(&Server::handleDisconnect,this,std::placeholders::_1)
                           ,std::bind(&Server::handleRequestNew,this,std::placeholders::_1)
                           ,std::bind(&Server::handleRequestReply,this,std::placeholders::_1))
        ,_serverImplStrandPairArray()//TODO
    {}

    void Server::start()
    {
        _connectionServer.start();
    }
    
    
    void Server::handleDisconnect(unsigned connectionId_)
    {}

    void Server::handleRequestNew(const ConnectionMessage& message_)
    {
        std::pair<ServerImplPtr,StrandPtr> serverImplStrandPair =
            getServerImplAndStrandForString(message_._parameters[0][0], true);
        boost::asio::spawn(*serverImplStrandPair.second,
                           std::bind(&ServerImpl::handleRequest,
                                     serverImplStrandPair.first,
                                     message_,
                                     std::placeholders::_1));
    }

}}} //closing namespace Santiago::User::Server
