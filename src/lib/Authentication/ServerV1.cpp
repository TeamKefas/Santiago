#include "ServerV1.h"

namespace Santiago{ namespace Authentication
{
    Server::Server(boost::asio::io_service& ioService_,unsigned port_)
        :_ioService(ioService_)
        ,_port(port_)
        ,_connectionMessage(nullptr,0)
        ,_connectionServer(_ioService
                           ,_port
                           ,std::bind(&Server::handleDisconnect,this,std::placeholders::_1)
                           ,std::bind(&Server::handleRequestNew,this,std::placeholders::_1)
                           ,std::bind(&Server::handleRequestReply,this,std::placeholders::_1))
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
            
        case ConnectionMessageType::CR_CREATE_USER:
            requestHandlerPtr.reset(new CreateUserRequestHandler
                                    (_connectionServer
                                     ,std::bind(&Server::handleRequestCompleted
                                                ,this,std::placeholders::_1)
                                     ,message_));
            
            break;
        case ConnectionMessageType::CR_LOGIN_USER:
            requestHandlerPtr.reset(new LoginUserRequestHandler
                                    (_connectionServer
                                     ,std::bind(&Server::handleRequestCompleted
                                                ,this,std::placeholders::_1)
                                     ,message_));
            break;
            
        case ConnectionMessageType::CR_VERIFY_USER_FOR_COOKIE:
            requestHandlerPtr.reset(new VerifyUserForCookieRequestHandler
                                    (_connectionServer
                                     ,std::bind(&Server::handleRequestCompleted
                                                ,this,std::placeholders::_1)
                                     ,message_));
            break;
            
        case ConnectionMessageType::CR_LOGOUT_USER_FOR_COOKIE:
            requestHandlerPtr.reset(new LogoutUserForCookieRequestHandler
                                    (_connectionServer
                                     ,std::bind(&Server::handleRequestCompleted
                                                ,this,std::placeholders::_1)
                                     ,message_));
            break;
            
        case ConnectionMessageType::CR_LOGOUT_USER_FOR_ALL_COOKIES:
            requestHandlerPtr.reset(new LogoutUserForAllCookiesRequestHandler
                                    (_connectionServer
                                     ,std::bind(&Server::handleRequestCompleted
                                                ,this,std::placeholders::_1)
                                     ,message_));
            break;
            
        case  ConnectionMessageType::CR_CHANGE_USER_PASSWORD:
            requestHandlerPtr.reset(new ChangeUserPasswordRequestHandler
                                    (_connectionServer
                                     ,std::bind(&Server::handleRequestCompleted
                                                ,this,std::placeholders::_1)
                                     ,message_));
            break;
        case ConnectionMessageType::CR_REMOVED_COOKIE_FROM_APPSERVER:
            requestHandlerPtr.reset(new RemovedCookieFromAppserverRequestHandler
                                    (_connectionServer
                                     ,std::bind(&Server::handleRequestCompleted
                                                ,this,std::placeholders::_1)
                                     ,message_));
            break;
            
        case ConnectionMessageType::CR_ADD_RESOURCE:
            requestHandlerPtr.reset(new AddResourceRequestHandler
                                    (_connectionServer
                                     ,std::bind(&Server::handleRequestCompleted
                                                ,this,std::placeholders::_1)
                                     ,message_));
            break;
             
        case ConnectionMessageType::SR_LOGOUT_USER_FOR_COOKIE:
            BOOST_ASSERT(false);
            break;
            
        case  ConnectionMessageType::SR_LOGOUT_USER_FOR_ALL_COOKIES:
            BOOST_ASSERT(false);
            break;
        }
        
        _activeRequestHandlersList.insert(std::make_pair(message_._requestId,requestHandlerPtr));
        requestHandlerPtr->start();
    }

    void Server::handleRequestReply(const ServerMessage& message_)
    {
        std::map<RequestId,RequestHandlerBasePtr>::iterator iter =
            _activeRequestHandlersList.find(message_._requestId);
    }

    void Server::handleRequestCompleted(const RequestId& requestId_)
    {
        std::map<RequestId,RequestHandlerBasePtr>::iterator iter =
            _activeRequestHandlersList.find(requestId_);
        //  _activeRequestHandlersList.find(message_._requestId);
        
        BOOST_ASSERT(iter == _activeRequestHandlersList.end());

        _activeRequestHandlersList.erase(iter);
    }


}} //closing namespace Santiago::Authentication
