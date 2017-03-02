#include "PingType1RequestHandler.h"

namespace Santiago{ namespace User{ namespace Server
{
    PingType1RequestHandler::PingType1RequestHandler(ServerData& serverData_,
                                                     SantiagoDBConnection& databaseConnection_,
                                                     const SendMessageCallbackFn& sendMessageCallbackFn_,
                                                     const OnCompletedCallbackFn& onCompletedCallbackFn_,
                                                     const ServerMessage& initiatingMessage_)
    :MyBase(serverData_,
            databaseConnection_,
            sendMessageCallbackFn_,
            onCompletedCallbackFn_,
            initiatingMessage_)
    {}

    void PingType1RequestHandler::handleInitiatingRequest()
    {
        std::cout<<"Received test request."<<std::endl;
        ServerMessage replyMessage(_initiatingMessage._connectionId,
                                   _initiatingMessage._requestId,
                                   ServerMessageType::CONNECTION_MESSAGE_REPLY,
                                   ConnectionMessage(ConnectionMessageType::SUCCEEDED,std::vector<std::string>()));
        _sendMessageCallbackFn(replyMessage);
        _onCompletedCallbackFn(_initiatingMessage._requestId);
    }

    void PingType1RequestHandler::handleReplyMessage(const ServerMessage& serverMessage_)
    {
        BOOST_ASSERT(false);
    }
    
}}}
