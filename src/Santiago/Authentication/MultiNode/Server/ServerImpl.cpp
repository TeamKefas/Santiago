#include "ServerImpl.h"

namespace Santiago{ namespace Authentication{ namespace MultiNode{ namespace Server
{

    ServerImpl::ServerImpl(ThreadSpecificDbConnection& databaseConnection_,
               const StrandPtr& strandPtr_,
               ConnectionServer& connectionServer_)
            :_controller(databaseConnection_,
                         strandPtr_,
                         connectionServer_)
    {}

    void ServerImpl::processRequest(const ConnectionMessage& connectionMessage_, boost::asio::yield_context yield_)
    {
        switch(connectionMessage_._type)
        {
        case ConnectionMessageType::CR_CREATE_USER:
        {
            std::error_code error;
            error = authenticatorStrandPair.first->createUser(userName,emailAddres_,password,yield_);
            ConnectionMessage replyMessage(_initiatingMessage._requestId,
                                           ConnectionMessageType::SUCEEDED,
                                           std::vector<std::string>());
            if(error)
            {
                replyMessage._type = ConnectionMessageType::FAILED;
            }
            _connectionServer.sendMessage(_initiatingMessage._requestId,replyMessage,false,boost::none);
            break;
        }
        /*
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
            */
            }
        }

}}}}
