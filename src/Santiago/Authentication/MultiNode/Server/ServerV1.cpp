#include "ServerV1.h"

#include "CreateUserRequestHandler.h"
#include "LoginUserRequestHandler.h"
#include "VerifyUserForCookieRequestHandler.h"
#include "LogoutUserForCookieRequestHandler.h"
#include "LogoutUserForAllCookiesRequestHandler.h"
#include "ChangeUserPasswordRequestHandler.h"
#include "GetUserForEmailAddressAndRecoveryStringRequestHandler.h"
#include "ChangeUserPasswordForEmailAddressAndRecoveryStringRequestHandler.h"
#include "ChangeUserEmailAddressRequestHandler.h"
#include "ChangeUserEmailAddressRequestHandler.h"
#include "CreateAndReturnRecoveryStringRequestHandler.h"
#include "DeleteUserRequestHandler.h"

namespace Santiago{ namespace User { namespace MultiNode { namespace Server
{
    ServerV1::ServerV1(boost::asio::io_service& ioService_,unsigned port_)
        :_ioService(ioService_),
         _port(port_),
         _connectionServer(_ioService,
                           _port,
                           std::bind(&Server::handleDisconnect,this,std::placeholders::_1),
                           std::bind(&Server::handleRequestNew,this,std::placeholders::_1))
    {}

    void ServerV1::start()
    {
        _connectionServer.start();
    }
    
    
    void ServerV1::handleDisconnect(unsigned connectionId_)
    {
        //TODO
    }

    void ServerV1::handleRequestNew(const ConnectionMessage& message_)
    {
        RequestHandlerBasePtr requestHandlerPtr;
        switch(message_._type)
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

    void ServerV1::handleRequestCompleted(const RequestId& requestId_)
    {
        std::map<RequestId,RequestHandlerBasePtr>::iterator iter =
            _activeRequestHandlersList.find(requestId_);
        //  _activeRequestHandlersList.find(message_._requestId);
        
        BOOST_ASSERT(iter == _activeRequestHandlersList.end());

        _activeRequestHandlersList.erase(iter);
    }


}}}} //closing namespace Santiago::User::MultiNode::Server
