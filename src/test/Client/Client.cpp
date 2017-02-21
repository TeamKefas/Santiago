#include "Client.h"
#include <iostream>

namespace Santiago { namespace User { namespace Client
{
    void Client::start()
    {
        std::cout<<"starting"<<std::endl;
        //_connectionMessageSocket.sendMessage();
        Server::RequestId requestId(1,1);
        // requestId._initiatingConnectionId =1;
        //requestId._requestNo =1;
        
        std::vector<std::string> parameters;
        parameters.push_back("name");
        parameters.push_back("abcd");

        Server::ConnectionMessage message(Server::ConnectionMessageType::CR_LOGIN_USER, parameters);
        //message._type = Server::ConnectionMessageType::CR_LOGIN_USER;
        //message._parameters[0] = "vijayaraj";
        //message._parameters[1] = "abcd";
        

        _connectionMessageSocket.sendMessage(requestId, message);
    }

    void Client::handleDisconnect()
    {
        std::cout<<"Disconnected"<<std::endl;
    }
    void Client::handleConnectionMessageSocketMessage(const Server::RequestId& requestId_,
                                                      const Server::ConnectionMessage& message_)
    {
        std::cout<<"initiating connection_id = "<<requestId_._initiatingConnectionId<<std::endl;
        std::cout<<"request_no = "<<requestId_._requestNo<<std::endl;
        std::cout<<"------------------Connection Message------------------- "<<std::endl;

        std::cout<<"Type = "<<static_cast<int>(message_._type);
        for(auto it = message_._parameters.begin();
            it != message_._parameters.end();
            ++it)
        {
            std::cout<<*it;
        }
        
    }

        }}}
