#include "LogoutUserForAllCookiesRequestHandler.h"

namespace Santiago{ namespace User { namespace Server
{

    LoginUserRequestHandler::LoginUserRequestHandler(ServerData& serverData_,
                                                     SantiagoDBConnection& databaseConnection_,
                                                     const SendMessageCallbackFn& sendMessageCallbackFn_,
                                                     const OnCompletedCallbackFn& onCompletedCallbackFn_,
                                                     const ServerMessage& initiatingMessage_)
        :RequestHandlerBase(serverData_, databaseConnection_, sendMessageCallbackFn_, onCompletedCallbackFn_, initiatingMessage_)
    {}
    
    void LogoutUserForAllCookiesRequestHandler::start()
    {
        std::error_code error; 

        SantiagoDBTables::SessionsRec sessionsRec;
        sessionsRec._userName = _serverData._cookieCookieDataMap.find(_initiatingMessage._connectionMessage->_parameters[0])->second._userName;
        sessionsRec._logoutTime = boost::posix_time::second_clock::local_time();
        sessionsRec._lastActiveTime = sessionsRec._logoutTime;
        _databaseConnection.get().updateSessionsRec(sessionsRec, error);

        if(!error)
        {
            for(auto it = _serverData._cookieCookieDataMap.begin(); it != _serverData._cookieCookieDataMap.end(); ++it)
            {
                if(it->second._userName == sessionsRec._userName)
                {
                    _serverData._cookieCookieDataMap.erase(it);   
                }
            }
            
            _serverData._userIdUserIdDataMap.erase(sessionsRec._userName);
            
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
    
    void LogoutUserForAllCookiesRequestHandler::handleReplyMessage(const ServerMessage& serverMessage)
    {
        BOOST_ASSERT(false);
    }
    
}}}
