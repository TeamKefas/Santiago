#include "DeleteUserRequestHandler.h"


namespace Santiago{namespace Authentication { namespace MultiNode { namespace Server
{
    DeleteUserRequestHandler::DeleteUserRequestHandler(ConnectionServer& connectionServer_,
                                                       ServerData& serverData_,
                                                       const OnCompletedCallbackFn& onCompletedCallbackFn_,
                                                       const ConnectionMessage& initiatingMessage_)
        :RequestHandlerBase(connectionServer_,serverData_,onCompletedCallbackFn_,initiatingMessage_)
    {}
    
    void DeleteUserRequestHandler::handleRequest()
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
                error = authenticatorStrandPair.first->deleteUser(cookieString,yield_);
                ConnectionMessage replyMessage(_initiatingMessage._requestId,
                                               ConnectionMessageType::SUCEEDED,
                                               std::vector<std::string>());
                if(error)
                {
                    replyMessage._type = ConnectionMessageType::FAILED;
                }
                _connectionServer.sendMessage(_initiatingMessage._requestId,replyMessage,false,boost::none);
                _onCompletedCallbackFn(_initiatingMessage._requestId);
            });
    }

}}}}
