#include "GetUserForEmailAddressAndRecoveryStringRequestHandler.h"


namespace Santiago{namespace Authentication { namespace MultiNode { namespace Server
{
    GetUserForEmailAddressAndRecoveryStringRequestHandler::GetUserForEmailAddressAndRecoveryStringRequestHandler(ConnectionServer& connectionServer_,
                                                                                                                 ServerData& serverData_,
                                                                                                                 const OnCompletedCallbackFn& onCompletedCallbackFn_,
                                                                                                                 const ConnectionMessage& initiatingMessage_)
    :RequestHandlerBase(connectionServer_,serverData_,onCompletedCallbackFn_,initiatingMessage_)
    {}
    
    void GetUserForEmailAddressAndRecoveryStringRequestHandler::handleRequest()
    {
        std::string emailAddress = _initiatingMessage._parameters[0];
        std::string userName;
        std::error_code errror;
        boost::optional<SantiagoDBTables::UsersRec> usersRec = serverData_._databaseConnection.get().getUsersRecForEmailAddress(emailAddress,error);
        if(usersRec)
        {
            userName = usersRec._userName;
        }
        std::pair<ControllerPtr,StrandPtr> authenticatorStrandPair =
            _serverData._authenticatorStrandPair[static_cast<int>(toupper(userName[0]))
                                                 - static_cast<int>('a')];

        std::string recoveryString = _initiatingMessage._parameters[1];

        boost::asio::spawn(
            *authenticatorStrandPair.second,
            [authenticatorStrandPair,emailAddress,recoveryString](boost::asio::yield_context yield_)
            {
                std::error_code error;
                boost::optional<std::string> stringOpt;
                std::tie(error,stringOpt) = authenticatorStrandPair.first->getUserForEmailAddressAndRecoveryString(emailAddress,recoveryString,yield_);
                std::vector<std::string> parameters = std::vector<std::string>();
                if(stringOpt)
                {
                    parameters.push_back(*stringOpt);
                }
                
                ConnectionMessage replyMessage(_initiatingMessage._requestId,
                                               ConnectionMessageType::SUCEEDED,
                                               parameters);
                if(error)
                {
                    replyMessage._type = ConnectionMessageType::FAILED;
                }
                _connectionServer.sendMessage(_initiatingMessage._requestId,replyMessage,false,boost::none);
                _onCompletedCallbackFn(_initiatingMessage._requestId);
            });
    }

}}}}
