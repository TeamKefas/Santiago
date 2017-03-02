#include "ChangeUserEmailAddressRequestHandler.h"


namespace Santiago{namespace User { namespace Server
{
    ChangeUserEmailAddressRequestHandler::ChangeUserEmailAddressRequestHandler(ServerData& serverData_,
                                                       SantiagoDBConnection& databaseConnection_,
                                                       const SendMessageCallbackFn& sendMessageCallbackFn_,
                                                       const OnCompletedCallbackFn& onCompletedCallbackFn_,
                                                       const ServerMessage& initiatingMessage_)
        :RequestHandlerBase(serverData_, databaseConnection_, sendMessageCallbackFn_, onCompletedCallbackFn_, initiatingMessage_)
    {}

    void ChangeUserEmailAddressRequestHandler::handleInitiatingRequest()
    {
        SantiagoDBTables::UsersRec userRec;
        std::error_code error;
        userRec._userName = _initiatingMessage._connectionMessage->_parameters[0];
        userRec._emailAddress = _initiatingMessage._connectionMessage->_parameters[1];
        userRec._password = _initiatingMessage._connectionMessage->_parameters[2];
        
        _databaseConnection.get().updateUsersRec(userRec, error);

        if(!error)
        {
            ConnectionMessage connectionMessage(ConnectionMessageType::SUCCEEDED,std::vector<std::string>()); 
            ServerMessage serverMessage(_initiatingMessage._connectionId,
                                        _initiatingMessage._requestId,
                                        ServerMessageType::CONNECTION_MESSAGE_REPLY,
                                        connectionMessage);
            
            _sendMessageCallbackFn(serverMessage);
            _onCompletedCallbackFn(_initiatingMessage._requestId);
        }
        else
        {
            ConnectionMessage connectionMessage(ConnectionMessageType::FAILED,std::vector<std::string>()); 
            ServerMessage serverMessage(_initiatingMessage._connectionId,
                                        _initiatingMessage._requestId,
                                        ServerMessageType::CONNECTION_MESSAGE_REPLY,
                                        connectionMessage);
            _sendMessageCallbackFn(serverMessage);
            _onCompletedCallbackFn(_initiatingMessage._requestId);
        }   
    }

    void ChangeUserEmailAddressRequestHandler::handleReplyMessage(const ServerMessage& serverMessage)
    {
        ST_ASSERT(false);
    }

    
}}}
