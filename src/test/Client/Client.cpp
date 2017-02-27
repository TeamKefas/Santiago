#include "Client.h"
#include <iostream>

namespace Santiago { namespace User { namespace Client
{
    void Client::startReadCycle()
    {
        _connectionMessageSocketPtr->start();
    }

    void Client::inputMsgFromUserAndSendToServer()
    {
        //TODO: Here take the msg from user...part it and amake the ConnectionMessageObject and send it.
        std::cout<<"starting"<<std::endl;
        Server::RequestId requestId(1,1);
        
        std::vector<std::string> parameters;
        parameters.push_back("name");
        parameters.push_back("abcd");

        Server::ConnectionMessage message(Server::ConnectionMessageType::CR_LOGIN_USER, parameters);
        
        _connectionMessageSocketPtr->sendMessage(requestId, message);
    }

    void Client::handleDisconnect()
    {
        std::cerr<<"Disconnected"<<std::endl;
        std::cout<<"Disconnected"<<std::endl;
        throw std::runtime_error("Connecton disconnected...exiting");
    }

    void Client::handleConnectionMessageSocketMessage(const Server::RequestId& requestId_,
                                                      const Server::ConnectionMessage& message_)
    {
        std::cerr<<"initiating connection_id = "<<requestId_._initiatingConnectionId<<std::endl;
        std::cerr<<"request_no = "<<requestId_._requestNo<<std::endl;
        std::cerr<<"------------------Connection Message------------------- "<<std::endl;

        std::cerr<<"Type = "<<static_cast<int>(message_._type)<<std::endl;
        std::cerr<<connectionMessageTypeString.find(message_._type)->second;
        for(auto it = message_._parameters.begin();
            it != message_._parameters.end();
            ++it)
        {
            std::cerr<<*it;
        }       
    }

}}}
