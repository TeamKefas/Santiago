#include "LoginUserRequestHandler.h"

namespace Santiago{ namespace Authentication { namespace MultiNode { namespace Server
{

    LoginUserRequestHandler::LoginUserRequestHandler(ConnectionServer& connectionServer_,
                                                     ServerData& serverData_,
                                                     const OnCompletedCallbackFn& onCompletedCallbackFn_,
                                                     const ConnectionMessage& initiatingMessage_)
        :RequestHandlerBase(connectionServer_,serverData_,onCompletedCallbackFn_,initiatingMessage_)
    {}
    
    void LoginUserRequestHandler::handleRequest()
    {
        std::string userNameOrEmailAddress = _initiatingMessage._parameters[0];
        bool isUserNameNotEmailAddress = _initiatingMessage._parameters[1];
        
        if(!isUserNameNotEmailAddress)
        {
            std::error_code errror;
            boost::optional<SantiagoDBTables::UsersRec> usersRec = serverData_._databaseConnection.get().getUsersRecForEmailAddress(userNameOrEmailAddress,error);
            if(usersRec)
            {
                userNameOrEmailAddress = usersRec._userName;
            }
        }
        std::pair<ControllerPtr,StrandPtr> authenticatorStrandPair =
            _serverData._authenticatorStrandPair[static_cast<int>(toupper(userNameOrEmailAddress[0]))
                                                 - static_cast<int>('a')];

        std::string password = _initiatingMessage._parameters[2];

        boost::asio::spawn(
            *authenticatorStrandPair.second,
            [authenticatorStrandPair,userNameOrEmailAddress_,isUserNameNotEmailAddress,password](boost::asio::yield_context yield_)
            {
                std::error_code error;
                boost::optional<std::pair<UserInfo,std::string> > userInfoStringPairOpt;
                std::tie(error,userInfoStringPairOpt) = authenticatorStrandPair.first->loginUser(userNameOrEmailAddress,isUserNameNotEmailAddress,password,yield_);
                std::vector<std::string> parameters = std::vector<std::string>();
                if(userInfoStringPairOpt)
                {
                    parameters.push_back(userInfoStringPairOpt->first._userName);
                    parameters.push_back(userInfoStringPairOpt->first._emailAddress);
                    parameters.push_back(userInfoStringPairOpt->second);
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
