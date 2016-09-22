#include "VerifyUserForCookieRequestHandler.h"

namespace Santiago{ namespace Authentication
{

    VerifyUserForCookieRequestHandler::VerifyUserForCookieRequestHandler(ConnectionServer& connectionServer_
                                                                         ,const OnCompletedCallbackFn& onCompletedCallbackFn_
                                                                         ,const ServerMessage& initiatingMessage_)
        :RequestHandlerBase(connectionServer_,onCompletedCallbackFn_,initiatingMessage_)
    {}
    
    void VerifyUserForCookieRequestHandler::start()
    {
        std::string cookie = _initiatingMessage._connectionMessage->_parameters[0];
        bool match = (_serverData._cookieCookieDataMap.find(cookie) != _serverData._cookieCookieDataMap.end());
        
        if(match)
        {
            ConnectionMessage connectionMessage(ConnectionMessageType::SUCCEEDED,std::vector<std::string>()); 
            ServerMessage serverMessage(_initiatingMessage._connectionId
                                        ,_initiatingMessage._requestId
                                        ,ServerMessageType::CONNECTION_MESSAGE_REPLY
                                        ,connectionMessage);
            
            _connectionServer.sendMessage(serverMessage);
            _onCompletedCallbackFn(_initiatingMessage._requestId);
        }
        else
        {
            if(_databaseInterface.verifyUserForCookie(_initiatingMessage._connectionMessage->_parameters[0]))
            {
                ConnectionMessage connectionMessage(ConnectionMessageType::SUCCEEDED,std::vector<std::string>()); 
                ServerMessage serverMessage(_initiatingMessage._connectionId
                                            ,_initiatingMessage._requestId
                                            ,ServerMessageType::CONNECTION_MESSAGE_REPLY
                                            ,connectionMessage);
                
                _connectionServer.sendMessage(serverMessage);
                _onCompletedCallbackFn(_initiatingMessage._requestId);
            }
            else
            {
                ConnectionMessage connectionMessage(ConnectionMessageType::FAILED,std::vector<std::string>()); 
                ServerMessage serverMessage(_initiatingMessage._connectionId
                                            ,_initiatingMessage._requestId
                                            ,ServerMessageType::CONNECTION_MESSAGE_REPLY
                                            ,connectionMessage);
                _connectionServer.sendMessage(serverMessage);
                _onCompletedCallbackFn(_initiatingMessage._requestId);
            }
        }
    }
    void VerifyUserForCookieRequestHandler::handleReplyMessage(const ServerMessage& serverMessage)
    {
        BOOST_ASSERT(false);
    }
    
}}
