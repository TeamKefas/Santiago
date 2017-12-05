#include "ChangeUserPasswordForEmailAddressAndRecoveryStringRequestHandler.h"


namespace Santiago{namespace Authentication { namespace MultiNode { namespace Server
{
    ChangeUserPasswordForEmailAddressAndRecoveryStringRequestHandler::ChangeUserPasswordForEmailAddressAndRecoveryStringRequestHandler(ConnectionServer& connectionServer_,
                                                                                                                                       ServerData& serverData_,
                                                                                                                                       const OnCompletedCallbackFn& onCompletedCallbackFn_,
                                                                                                                                       const ConnectionMessage& initiatingMessage_)
    :RequestHandlerBase(connectionServer_,serverData_,onCompletedCallbackFn_,initiatingMessage_)
    {}
    
    void ChangeUserPasswordForEmailAddressAndRecoveryStringRequestHandler::handleRequest()
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
        std::string newPassword = _initiatingMessage._parameters[2];

        boost::asio::spawn(
            *authenticatorStrandPair.second,
            [authenticatorStrandPair,emailAddress,recoveryString,newPassword](boost::asio::yield_context yield_)
            {
                std::error_code error;
                error = authenticatorStrandPair.first->ChangeUserPasswordForEmailAddressAndRecoveryStringRequestHandler(emailAddress,recoveryString,newPassword,yield_);
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
