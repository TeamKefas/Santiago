#include "RemovedCookieFromAppserverRequestHandler.h"


namespace Santiago{ namespace Authentication
{

    RemovedCookieFromAppserverRequestHandler::RemovedCookieFromAppserverRequestHandler(ConnectionServer& connectionServer_
                                                                                       ,const OnCompletedCallbackFn& onCompletedCallbackFn_
                                                                                       ,const ServerMessage& initiatingMessage_)
        :RequestHandlerBase(connectionServer_,onCompletedCallbackFn_,initiatingMessage_)
    {}
    
    void RemovedCookieFromAppserverRequestHandler::start()
    {
        std::string removedCookie = _initiatingMessage._connectionMessage->_parameters[0];
        
        CookieData cookieData = _serverData._cookieCookieDataMap.find(removedCookie)->second;
        if(std::find(cookieData._connectionIds.begin(),
                     cookieData._connectionIds.end(),
                     _connectionServer._nextConnectionId) != cookieData._connectionIds.end())
        {
            cookieData._connectionIds.erase(std::remove( cookieData._connectionIds.begin(),
                                                         cookieData._connectionIds.end(),
                                                         _connectionServer._nextConnectionId),
                                            cookieData._connectionIds.end());
            std::string userId = cookieData._userId;

            UserIdData userIdData = _serverData._userIdUserIdDataMap.find(userId)->second;
            userIdData._cookieList.erase(std::remove(userIdData._cookieList.begin(),
                                                     userIdData._cookieList.end(),
                                                     removedCookie),
                                         userIdData._cookieList.end());
        }
    }
    
    void RemovedCookieFromAppserverRequestHandler::handleReplyMessage(const ServerMessage& serverMessage)
    { 
        BOOST_ASSERT(false);
    }
    
}}
