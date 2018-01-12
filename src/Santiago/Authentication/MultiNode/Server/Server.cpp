/*#include "CreateUserRequestHandler.h"
#include "LoginUserRequestHandler.h"
#include "VerifyUserForCookieRequestHandler.h"
#include "LogoutUserForCookieRequestHandler.h"
#include "LogoutUserForAllCookiesRequestHandler.h"
#include "ChangeUserPasswordRequestHandler.h"*/

#include "Server.h"

namespace Santiago{ namespace Authentication { namespace MultiNode { namespace Server
{
    Server::Server(boost::asio::io_service& ioService_,unsigned port_,
                   ThreadSpecificDbConnection& databaseConnection_)
        :_ioService(ioService_),
         _port(port_),
         _connectionServer(_ioService,
                               _port,
                               std::bind(&Server::handleConnectionDisconnect,this,std::placeholders::_1),
                           std::bind(&Server::handleNewRequest,this,std::placeholders::_1)),
         _databaseConnection(databaseConnection_)
         //_serverImplStrandPairArray())//TODO
    {}

    void Server::start()
    {
        _connectionServer.start();
    }
    
    
    void Server::handleConnectionDisconnect(unsigned connectionId_)
    {}

    void Server::handleNewRequest(const ConnectionMessage& message_)
    {
std::pair<Server::ServerImplPtr,Server::StrandPtr> serverImplStrandPair =
            getServerImplAndStrandForString(message_._parameters[0], true);
        boost::asio::spawn(*serverImplStrandPair.second,
                           std::bind(&ServerImpl::processRequest,
                                     serverImplStrandPair.first,
                                     message_,
                                     std::placeholders::_1));
    }

    std::pair<Server::ServerImplPtr,Server::StrandPtr> Server::getServerImplAndStrandForString(const std::string& string_,
                                                                                                   bool isNotEmailAddress_)
    {
        std::string userName = string_;
        std::error_code error;
        if(!isNotEmailAddress_)
        {
            boost::optional<SantiagoDBTables::UsersRec> usersRec = _databaseConnection.get().getUsersRecForEmailAddress(string_,error);
            if(usersRec)
            {
                userName = usersRec->_userName;
            }
        }
        return _serverImplStrandPairArray[static_cast<int>(toupper(userName.at(0)))-65];
    }
        
}}}} //closing namespace Santiago::Authentication::MultiNode::Server
