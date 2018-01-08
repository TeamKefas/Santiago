#ifndef SANTIAGO_AUTHENTICATION_MULTINODE_SERVER_SERVERIMPL_H
#define SANTIAGO_AUTHENTICATION_MULTINODE_SERVER_SERVERIMPL_H

#include <memory>

#include <boost/asio/strand.hpp>

#include "Santiago/SantiagoDBTables/MariaDBConnection.h"
#include "Santiago/Thread/ThreadSpecificVar.h"

#include "ConnectionServer.h"
#include "Controller.h"

namespace Santiago{ namespace Authentication{ namespace MultiNode{ namespace Server
{
    struct ServerImpl
    {
        typedef Thread::ThreadSpecificVar<SantiagoDBTables::MariaDBConnection> ThreadSpecificDbConnection;
        typedef std::shared_ptr<boost::asio::strand> StrandPtr;

        ServerImpl(ThreadSpecificDbConnection& databaseConnection_,
                   const StrandPtr& strandPtr_,
                   ConnectionServer& connectionServer_)
            :_controller(databaseConnection_,
                         strandPtr_,
                         connectionServer_)
        {}

        void processRequest(const ConnectionMessage& connectionMessage_)
        {
            switch(connectionMessage_._type)
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
        }

        
        Controller   _controller;
    };

}}}}

#endif
