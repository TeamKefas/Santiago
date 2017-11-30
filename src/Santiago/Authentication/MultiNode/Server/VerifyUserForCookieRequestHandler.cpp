#include "VerifyUserForCookieRequestHandler.h"

namespace Santiago{ namespace User { namespace Server
{

    VerifyUserForCookieRequestHandler::VerifyUserForCookieRequestHandler(ConnectionServer& connectionServer_,
                                                                         ServerData& serverData_,
                                                                         const OnCompletedCallbackFn& onCompletedCallbackFn_,
                                                                         const ConnectionMessage& initiatingMessage_)
        :RequestHandlerBase(connectionServer_,serverData_,onCompletedCallbackFn_,initiatingMessage_)
    {}
    
    void VerifyUserForCookieRequestHandler::handleRequest()
    {
        std::string cookieString = _initiatingMessage._parameters[0];
        std::string userName;
        boost::optional<SantiagoDBTables::UsersRec> sessionsRec = serverData_._databaseConnection.get().getSessionsRec(cookieString,error);
        if(sessionsRec)
        {
            userName = sessionsRec._userName;
        }
        std::pair<ControllerPtr,StrandPtr> authenticatorStrandPair =
            _serverData._authenticatorStrandPair[static_cast<int>(toupper(userName[0]))
                                                 - static_cast<int>('a')];
        
        boost::asio::spawn(
            *authenticatorStrandPair.second,
            [authenticatorStrandPair,cookieString](boost::asio::yield_context yield_)
            {
                std::error_code error;
                boost::optional<UserInfo> userInfoOpt;
                std::tie(error,userInfoOpt) = authenticatorStrandPair.first->verifyCookieAndGetUserInfo(cookieString,yield_);
                std::vector<std::string> parameters = std::vector<std::string>();
                if(userInfoOpt)
                {
                    parameters.push_back(userInfoOpt->_userName);
                    parameters.push_back(userInfoOpt->_emailAddress);
                }
                
                ConnectionMessage replyMessage(_initiatingMessage._requestId,
                                               ConnectionMessageType::SUCEEDED,
                                               parameters);
                if(error)
                {
                    replyMessage._type = ConnectionMessageType::FAILED;
                }
                _connectionServer.sendMessage(replyMessage,false,boost::none);
                _onCompletedCallbackFn(_initiatingMessage._requestId);
            });
    }
    
}}}
