#include "ChangeUserPasswordRequestHandler.h"

namespace Santiago{ namespace Authentication
{

   ChangeUserPasswordRequestHandler::ChangeUserPasswordRequestHandler(ConnectionServer& connectionServer_
                                                                      ,const OnCompletedCallbackFn& onCompletedCallbackFn_
                                                                      ,const ServerMessage& initiatingMessage_)
       :RequestHandlerBase(connectionServer_,onCompletedCallbackFn_,initiatingMessage_)
   {}
    
    void ChangeUserPasswordRequestHandler::start()
    {
        if(_databaseInterface.changeUserPassword(_initiatingMessage._connectionMessage->_parameters[0],
                                                 _initiatingMessage._connectionMessage->_parameters[1],
                                                 _initiatingMessage._connectionMessage->_parameters[2]))
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
    
    void ChangeUserPasswordRequestHandler::handleReplyMessage(const ServerMessage& serverMessage)
    {
        BOOST_ASSERT(false);
    }
    
 }}
