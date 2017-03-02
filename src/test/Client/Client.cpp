#include "Client.h"
#include <iostream>

namespace Santiago { namespace User { namespace Client
{
    void Client::startReadCycle()
    {
        _connectionMessageSocketPtr->startRead();
    }

    void Client::inputMsgFromUserAndSendToServer()
    {
        //TODO: Here take the msg from user...part it and amake the ConnectionMessageObject and send it.
        unsigned initiatingConnectionId, requestNo;
        std::cout<<"enter Initiating Connection_id and Request_no"<<std::endl;
        std::cin>>initiatingConnectionId>>requestNo;
        
        Server::RequestId requestId(initiatingConnectionId, requestNo);
        
        std::vector<std::string> parameters;
        Server::ConnectionMessageType connectionMessageType;
        
        std::cout<<"Enter Parameters"<<std::endl;
        unsigned choice;
        do{
            std::cout<<"Connection_Message_Type"<<std::endl;
            std::cout<<"1.CR_CREATE_USER\n" 
                     <<"2.CR_LOGIN_USER\n"
                     <<"3.CR_VERIFY_COOKIE_AND_GET_USER_INFO\n"
                     <<"4.CR_LOGOUT_USER_FOR_COOKIE\n"
                     <<"5.CR_LOGOUT_USER_FOR_ALL_COOKIES\n"
                     <<"6.CR_CHANGE_USER_PASSWORD\n"
                     <<"7.CR_CHANGE_USER_EMAIL_ADDRESS\n"
                     <<"8.CR_DELETE_USER\n"
                     <<"9.CR_PING_TYPE1\n"
                     <<std::endl;
            std::cin>>choice;
            switch(choice)
            {
            case 1: connectionMessageType = Server::ConnectionMessageType::CR_CREATE_USER;
                break;
            case 2: connectionMessageType = Server::ConnectionMessageType::CR_LOGIN_USER;
                break;
            case 3: connectionMessageType = Server::ConnectionMessageType::CR_VERIFY_COOKIE_AND_GET_USER_INFO;
                break;
            case 4: connectionMessageType = Server::ConnectionMessageType::CR_LOGOUT_USER_FOR_COOKIE;
                break;
            case 5: connectionMessageType = Server::ConnectionMessageType::CR_LOGOUT_USER_FOR_ALL_COOKIES;
                break;
            case 6: connectionMessageType = Server::ConnectionMessageType::CR_CHANGE_USER_PASSWORD;
                break;
            case 7: connectionMessageType = Server::ConnectionMessageType::CR_CHANGE_USER_EMAIL_ADDRESS;
                break;
            case 8: connectionMessageType = Server::ConnectionMessageType::CR_DELETE_USER;
                break;
            case 9: connectionMessageType = Server::ConnectionMessageType::CR_PING_TYPE1;
                break;
            }
        }while(choice > 10);   
        if (connectionMessageType == Server::ConnectionMessageType::CR_CREATE_USER)
        {
            std::string userName, emailAddress, password;
            std::cout<<"user_name";
            std::cin>>userName;
            parameters.push_back(userName);
            std::cout<<"email_address";
            std::cin>>emailAddress;
            parameters.push_back(emailAddress);
            std::cout<<"password";
            std::cin>>password;
            parameters.push_back(password);
        }
        
        if(connectionMessageType == Server::ConnectionMessageType::CR_LOGIN_USER)
        {
            std::string userNameorEmailAdress, password;
            std::cout<<"user_name_or_email_address";
            std::cin>>userNameorEmailAdress;
            parameters.push_back(userNameorEmailAdress);
            std::cout<<"password";
            std::cin>>password;
            parameters.push_back(password);
        }
        
        Server::ConnectionMessage message(connectionMessageType, parameters);   
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
