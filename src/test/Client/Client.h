#ifndef SANTIAGO_USER_CLIENT_H
#define SANTIAGO_USER_CLIENT_H


#include "../../Santiago/AppServer/ServerBase.h"
#include "../../Santiago/Utils/STLog.h"
#include "../../Santiago/User/ServerV1/ConnectionMessageSocket.h"
#include <map>
using boost::asio::ip::tcp;

namespace Santiago { namespace User { namespace Client
{
    class Client
    {
    public:
         typedef std::function<void(unsigned)> OnDisconnectCallbackFn;
        Client(Server::ConnectionMessageSocket::MySocketPtr socketPtr_)
            :_connectionMessageSocketPtr(new Server::ConnectionMessageSocket(socketPtr_,
                                                                             std::bind(&Client::handleDisconnect,this),
                                                                             std::bind(
                                                                                 &Client::handleConnectionMessageSocketMessage,
                                                                                 this,
                                                                                 std::placeholders::_1,
                                                                                 std::placeholders::_2)))
        {
        }

        void startReadCycle();
        void inputMsgFromUserAndSendToServer();
        
        
    protected:
        
        void handleDisconnect();
        void handleConnectionMessageSocketMessage(const Server::RequestId& requestId_,
                                                  const Server::ConnectionMessage& message_);
        
        std::shared_ptr<Server::ConnectionMessageSocket>    _connectionMessageSocketPtr;
        
    const std::map<std::string, Server::ConnectionMessageType> stringConnectionMessageType =
        {{"SUCCEEDED", Server::ConnectionMessageType::SUCCEEDED},
         {"FAILED", Server::ConnectionMessageType::FAILED},
         {"CR_CREATE_USER", Server::ConnectionMessageType::CR_CREATE_USER},
         {"CR_LOGIN_USER", Server::ConnectionMessageType::CR_LOGIN_USER},
         {"CR_VERIFY_COOKIE_AND_GET_USER_INFO",Server::ConnectionMessageType::CR_VERIFY_COOKIE_AND_GET_USER_INFO},
         {"CR_LOGOUT_USER_FOR_COOKIE", Server::ConnectionMessageType::CR_LOGOUT_USER_FOR_COOKIE},
         {"CR_LOGOUT_USER_FOR_ALL_COOKIES", Server::ConnectionMessageType::CR_LOGOUT_USER_FOR_ALL_COOKIES},
         {"CR_CHANGE_USER_PASSWORD", Server::ConnectionMessageType::CR_CHANGE_USER_PASSWORD},
         {"CR_CHANGE_USER_EMAIL_ADDRESS", Server::ConnectionMessageType::CR_CHANGE_USER_EMAIL_ADDRESS},
         {"CR_DELETE_USER", Server::ConnectionMessageType::CR_DELETE_USER},
         {"SR_LOGOUT_USER_FOR_COOKIE", Server::ConnectionMessageType::SR_LOGOUT_USER_FOR_COOKIE},
         {"SR_LOGOUT_USER_FOR_ALL_COOKIES", Server::ConnectionMessageType::SR_LOGOUT_USER_FOR_ALL_COOKIES}};
        

        const std::map<Server::ConnectionMessageType, std::string> connectionMessageTypeString =
        {{Server::ConnectionMessageType::SUCCEEDED, "SUCCEEDED"},
         {Server::ConnectionMessageType::FAILED,"FAILED"},
         {Server::ConnectionMessageType::CR_CREATE_USER, "CR_CREATE_USER"},
         {Server::ConnectionMessageType::CR_LOGIN_USER, "CR_LOGIN_USER"},
         {Server::ConnectionMessageType::CR_VERIFY_COOKIE_AND_GET_USER_INFO, "CR_VERIFY_COOKIE_AND_GET_USER_INFO"},
         {Server::ConnectionMessageType::CR_LOGOUT_USER_FOR_COOKIE, "CR_LOGOUT_USER_FOR_COOKIE"},
         {Server::ConnectionMessageType::CR_LOGOUT_USER_FOR_ALL_COOKIES, "CR_LOGOUT_USER_FOR_ALL_COOKIES"},
         {Server::ConnectionMessageType::CR_CHANGE_USER_PASSWORD, "CR_CHANGE_USER_PASSWORD"},
         {Server::ConnectionMessageType::CR_CHANGE_USER_EMAIL_ADDRESS, "CR_CHANGE_USER_EMAIL_ADDRESS"},
         {Server::ConnectionMessageType::CR_DELETE_USER, "CR_DELETE_USER"},
         {Server::ConnectionMessageType::SR_LOGOUT_USER_FOR_COOKIE, "SR_LOGOUT_USER_FOR_COOKIE"},
         {Server::ConnectionMessageType::SR_LOGOUT_USER_FOR_ALL_COOKIES, "SR_LOGOUT_USER_FOR_ALL_COOKIES"}};
        
        
        
    };
        
}}}

#endif
    
