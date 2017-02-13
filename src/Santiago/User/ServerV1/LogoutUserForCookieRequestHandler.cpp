#include "LogoutUserForCookieRequestHandler.h"

namespace Santiago{ namespace User { namespace Server
{

    LogoutUserForCookieRequestHandler::LogoutUserForCookieRequestHandler(ServerData& serverData_,
                                                     SantiagoDBConnection& databaseConnection_,
                                                     const SendMessageCallbackFn& sendMessageCallbackFn_,
                                                     const OnCompletedCallbackFn& onCompletedCallbackFn_,
                                                     const ServerMessage& initiatingMessage_)
        :RequestHandlerBase(serverData_, databaseConnection_, sendMessageCallbackFn_, onCompletedCallbackFn_, initiatingMessage_)
    {}
    
    void LogoutUserForCookieRequestHandler::start()
    {
        std::error_code error; 

        SantiagoDBTables::SessionsRec sessionsRec;
        sessionsRec._userName = _serverData._cookieCookieDataMap.find(_initiatingMessage._connectionMessage->_parameters[0])->second._userName;
        sessionsRec._logoutTime = boost::posix_time::second_clock::local_time();
        sessionsRec._lastActiveTime = sessionsRec._logoutTime;
        _databaseConnection.get().updateSessionsRec(sessionsRec, error);

        if(!error)
        {
            _serverData._cookieCookieDataMap.erase(_initiatingMessage._connectionMessage->_parameters[0]);
            _serverData._userIdUserIdDataMap.find( sessionsRec._userName)->second.erase(_initiatingMessage._connectionId);
            
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



   
    
    void LogoutUserForCookieRequestHandler::handleReplyMessage(const ServerMessage& serverMessage)
    {
        BOOST_ASSERT(false);
    }
    
}}}
