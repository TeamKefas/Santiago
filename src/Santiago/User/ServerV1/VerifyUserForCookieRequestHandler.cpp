#include "VerifyUserForCookieRequestHandler.h"


namespace Santiago{namespace User { namespace Server
{
    VerifyUserForCookieRequestHandler::VerifyUserForCookieRequestHandler(ServerData& serverData_,
                                                       SantiagoDBConnection& databaseConnection_,
                                                       const SendMessageCallbackFn& sendMessageCallbackFn_,
                                                       const OnCompletedCallbackFn& onCompletedCallbackFn_,
                                                       const ServerMessage& initiatingMessage_)
        :RequestHandlerBase(serverData_, databaseConnection_, sendMessageCallbackFn_, onCompletedCallbackFn_, initiatingMessage_)
    {}

    void VerifyUserForCookieRequestHandler::start()
    {
        SantiagoDBTables::SessionsRec sessionsRec;
         std::error_code error;
        std::string cookie = _initiatingMessage._connectionMessage->_parameters[0];
        sessionsRec._cookieString = cookie;
        sessionsRec._loginTime = boost::posix_time::second_clock::local_time();
   
        bool match = (_serverData._cookieCookieDataMap.find(cookie) != _serverData._cookieCookieDataMap.end());
        
        if(match)
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
            _databaseConnection.get().addSessionsRec(sessionsRec, error);
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
    }

    void VerifyUserForCookieRequestHandler::handleReplyMessage(const ServerMessage& serverMessage)
    {
        BOOST_ASSERT(false);
    }

    
}}}
