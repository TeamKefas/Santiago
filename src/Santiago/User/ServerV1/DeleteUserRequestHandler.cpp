#include "DeleteUserRequestHandler.h"

namespace Santiago{ namespace User { namespace Server
{

    DeleteUserRequestHandler::DeleteUserRequestHandler(ServerData& serverData_,
                                                     SantiagoDBConnection& databaseConnection_,
                                                     const SendMessageCallbackFn& sendMessageCallbackFn_,
                                                     const OnCompletedCallbackFn& onCompletedCallbackFn_,
                                                     const ServerMessage& initiatingMessage_)
        :RequestHandlerBase(serverData_, databaseConnection_, sendMessageCallbackFn_, onCompletedCallbackFn_, initiatingMessage_)
    {}
    
    void DeleteUserRequestHandler::handleInitiatingRequest()
    {
        std::error_code error1, error2; 

        SantiagoDBTables::SessionsRec sessionsRec;
        unsigned  cookiesErasedCount = 0;
        sessionsRec._userName = _initiatingMessage._connectionMessage->_parameters[0];
        sessionsRec._logoutTime = boost::posix_time::second_clock::local_time();
        sessionsRec._lastActiveTime = *(sessionsRec._logoutTime);
        std::vector<std::string> cookieList =
            _serverData._userIdUserIdDataMap.find(sessionsRec._userName)->second._cookieList;
        auto it = cookieList.begin();
        while(it != cookieList.end())
        {
            sessionsRec._cookieString = *it;
            _databaseConnection.get().updateSessionsRec(sessionsRec, error1);
            if(!error1)
            {
                auto iter = _serverData._cookieCookieDataMap.find(*it);
                _serverData._cookieCookieDataMap.erase(iter); 
                ++cookiesErasedCount;
                ++it;
            }
            else
            {
                ++it;
            }
        }
        if(cookiesErasedCount == cookieList.size())
        {
            _serverData._userIdUserIdDataMap.erase(sessionsRec._userName);
            _databaseConnection.get().deleteUsersRec(sessionsRec._userName, error2);
             if(!error2)
             {
                 ConnectionMessage connectionMessage(ConnectionMessageType::SUCCEEDED,std::vector<std::string>()); 
                 ServerMessage serverMessage(_initiatingMessage._connectionId,
                                             _initiatingMessage._requestId,
                                             ServerMessageType::CONNECTION_MESSAGE_REPLY,
                                             connectionMessage);
                 
                 _sendMessageCallbackFn(serverMessage);
                 _onCompletedCallbackFn(_initiatingMessage._requestId);
             }
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
    
    void DeleteUserRequestHandler::handleReplyMessage(const ServerMessage& serverMessage)
    {
        ST_ASSERT(false);
    }
    
}}}
