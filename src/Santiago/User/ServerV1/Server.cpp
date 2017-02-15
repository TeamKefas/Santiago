
#include "Server.h"

namespace Santiago{ namespace User { namespace Server
{
    Server::Server(const boost::property_tree::ptree& config_):
        _ioService(),
        _config(config_),
        _databaseConnection(std::bind(Santiago::SantiagoDBTables::CreateMariaDBConnection,config_)),
        _connectionServer(_ioService,
                          config_.get<unsigned>("Santiago.UserServer.port"),
                          std::bind(&Server::handleDisconnect,this,std::placeholders::_1),
                          std::bind(&Server::handleRequestNew,this,std::placeholders::_1),
                          std::bind(&Server::handleRequestReply,this,std::placeholders::_1))
    {}

    void Server::start()
    {
        _connectionServer.start();
    }
    
    
    void Server::handleDisconnect(unsigned connectionId_)
    {
        //TODO
    }

    void Server::handleRequestNew(const ServerMessage& message_)
    {
        RequestHandlerBasePtr requestHandlerPtr;
        switch(message_._connectionMessage->_type)
        {
            //Below is a non working example
        case ConnectionMessageType::CR_CREATE_USER:
            requestHandlerPtr.reset(new CreateUserRequestHandler(
                                        _serverData,
                                        _databaseConnection,
                                        std::bind(&ConnectionServer::sendMessage,
                                                  &_connectionServer,
                                                  std::placeholders::_1),
                                        std::bind(&Server::handleRequestCompleted,this,std::placeholders::_1),
                                        message_));
            break;
        case ConnectionMessageType::CR_CHANGE_USER_EMAIL_ADDRESS:
            requestHandlerPtr.reset(new ChangeUserEmailAddressRequestHandler(
                                        _serverData,
                                        _databaseConnection,
                                        std::bind(&ConnectionServer::sendMessage,
                                                  &_connectionServer,
                                                  std::placeholders::_1),
                                        std::bind(&Server::handleRequestCompleted, this, std::placeholders::_1),
                                        message_));
            break;  
        case ConnectionMessageType::CR_CHANGE_USER_PASSWORD:
            requestHandlerPtr.reset(new ChangeUserPasswordRequestHandler(
                                        _serverData,
                                        _databaseConnection,
                                        std::bind(&ConnectionServer::sendMessage,
                                                  &_connectionServer,
                                                  std::placeholders::_1),
                                        std::bind(&Server::handleRequestCompleted, this, std::placeholders::_1),
                                        message_));
            break;
        case ConnectionMessageType::CR_VERIFY_COOKIE_AND_GET_USER_INFO:
             requestHandlerPtr.reset(new VerifyUserForCookieRequestHandler(
                                        _serverData,
                                        _databaseConnection,
                                        std::bind(&ConnectionServer::sendMessage,
                                                  &_connectionServer,
                                                  std::placeholders::_1),
                                        std::bind(&Server::handleRequestCompleted, this, std::placeholders::_1),
                                        message_));
             break;
        case ConnectionMessageType::CR_LOGIN_USER:
             requestHandlerPtr.reset(new LoginUserRequestHandler(
                                        _serverData,
                                        _databaseConnection,
                                        std::bind(&ConnectionServer::sendMessage,
                                                  &_connectionServer,
                                                  std::placeholders::_1),
                                        std::bind(&Server::handleRequestCompleted, this, std::placeholders::_1),
                                        message_));
             break;
        case ConnectionMessageType::CR_LOGOUT_USER_FOR_COOKIE:
            requestHandlerPtr.reset(new LogoutUserForCookieRequestHandler(
                                        _serverData,
                                        _databaseConnection,
                                        std::bind(&ConnectionServer::sendMessage,
                                                  &_connectionServer,
                                                  std::placeholders::_1),
                                        std::bind(&Server::handleRequestCompleted, this, std::placeholders::_1),
                                        message_));
            break;
        case ConnectionMessageType::CR_LOGOUT_USER_FOR_ALL_COOKIES:
            requestHandlerPtr.reset(new LogoutUserForAllCookiesRequestHandler(
                                        _serverData,
                                        _databaseConnection,
                                        std::bind(&ConnectionServer::sendMessage,
                                                  &_connectionServer,
                                                  std::placeholders::_1),
                                        std::bind(&Server::handleRequestCompleted, this, std::placeholders::_1),
                                        message_));
            break;
        case ConnectionMessageType::CR_DELETE_USER:
            requestHandlerPtr.reset(new DeleteUserRequestHandler(
                                        _serverData,
                                        _databaseConnection,
                                        std::bind(&ConnectionServer::sendMessage,
                                                  &_connectionServer,
                                                  std::placeholders::_1),
                                        std::bind(&Server::handleRequestCompleted, this, std::placeholders::_1),
                                        message_));
            break;
        default:
            ST_ASSERT(false);
            break;
            

        }
        
        _activeRequestHandlersList.insert(std::make_pair(message_._requestId, requestHandlerPtr));
        requestHandlerPtr->start();
    }

    void Server::handleRequestReply(const ServerMessage& message_)
    {

        std::map<RequestId,RequestHandlerBasePtr>::iterator iter =
            _activeRequestHandlersList.find(message_._requestId);
        BOOST_ASSERT(iter != _activeRequestHandlersList.end());
        
    }

    void Server::handleRequestCompleted(const RequestId& requestId_)
    {
        std::map<RequestId,RequestHandlerBasePtr>::iterator iter =
            _activeRequestHandlersList.find(requestId_);
        //  _activeRequestHandlersList.find(message_._requestId);
        
        BOOST_ASSERT(iter == _activeRequestHandlersList.end());

        _activeRequestHandlersList.erase(iter);
    }


}}} //closing namespace Santiago::User::Server
