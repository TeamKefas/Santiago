#include "ServerImpl.h"

namespace Santiago{ namespace Authentication{ namespace MultiNode{ namespace Server
{   
    ServerImpl::ServerImpl(ThreadSpecificDbConnection& databaseConnection_,
                           const StrandPtr& strandPtr_,
                           ConnectionServer& connectionServer_,
                           boost::asio::io_service& ioService_,
                           unsigned port_,
                           const OnDisconnectCallbackFn& onDisconnectCallbackFn_,
                           const OnNewRequestCallbackFn& onNewRequestCallbackFn_)
        :_onDisconnectCallbackFn(onDisconnectCallbackFn_),
         _onNewRequestCallbackFn(onNewRequestCallbackFn_),
         _connectionServer(ioService_,
                           port_,
                           _onDisconnectCallbackFn,
                           _onNewRequestCallbackFn),
         _controller(databaseConnection_,
                     strandPtr_,
                     connectionServer_)
    {}

    void ServerImpl::processRequest(const ConnectionMessage& connectionMessage_, boost::asio::yield_context yield_)
    {
        switch(connectionMessage_._type)
        {
        case ConnectionMessageType::CR_CREATE_USER:
        {
            std::error_code error;
            error = _authenticatorStrandPair->first->createUser(connectionMessage_._parameters[0],
                                                                connectionMessage_._parameters[1],
                                                                connectionMessage_._parameters[2],
                                                                yield_);
            ConnectionMessage replyMessage(connectionMessage_._requestId,
                                           ConnectionMessageType::SUCCEEDED,
                                           std::vector<std::string>());
            if(error)
            {
                replyMessage._type = ConnectionMessageType::FAILED;
            }
            _connectionServer.sendMessage(connectionMessage_._requestId._initiatingConnectionId,
                                          replyMessage,
                                          false,
                                          boost::none);
            break;
        }
        
        case ConnectionMessageType::CR_LOGIN_USER:
        {
            std::error_code error;
            boost::optional<std::pair<UserInfo,std::string> > userInfoStringPairOpt;
            std::tie(error,userInfoStringPairOpt) = _authenticatorStrandPair->first->loginUser(connectionMessage_._parameters[0],
                                                                                               boost::lexical_cast<bool>(connectionMessage_._parameters[1]),
                                                                                               connectionMessage_._parameters[2],
                                                                                               yield_);
            std::vector<std::string> parameters = std::vector<std::string>();
            if(userInfoStringPairOpt)
            {
                parameters.push_back(userInfoStringPairOpt->first._userName);
                parameters.push_back(userInfoStringPairOpt->first._emailAddress);
                parameters.push_back(userInfoStringPairOpt->second);
            }
                
            ConnectionMessage replyMessage(connectionMessage_._requestId,
                                           ConnectionMessageType::SUCCEEDED,
                                           parameters);
            if(error)
            {
                replyMessage._type = ConnectionMessageType::FAILED;
            }
            _connectionServer.sendMessage(connectionMessage_._requestId._initiatingConnectionId,
                                          replyMessage,
                                          false,
                                          boost::none);
            break;
        }
            
        case ConnectionMessageType::CR_VERIFY_COOKIE_AND_GET_USER_INFO:
        {
            std::error_code error;
            boost::optional<UserInfo> userInfoOpt;
            std::tie(error,userInfoOpt) = _authenticatorStrandPair->first->verifyCookieAndGetUserInfo(connectionMessage_._parameters[0],
                                                                                                      yield_);
            std::vector<std::string> parameters = std::vector<std::string>();
            if(userInfoOpt)
            {
                parameters.push_back(userInfoOpt->_userName);
                parameters.push_back(userInfoOpt->_emailAddress);
            }
                
            ConnectionMessage replyMessage(connectionMessage_._requestId,
                                           ConnectionMessageType::SUCCEEDED,
                                           parameters);
            if(error)
            {
                replyMessage._type = ConnectionMessageType::FAILED;
            }
            _connectionServer.sendMessage(connectionMessage_._requestId._initiatingConnectionId,
                                          replyMessage,
                                          false,
                                          boost::none);
            break;
        }

        case ConnectionMessageType::CR_LOGOUT_USER_FOR_COOKIE:
        {
            std::error_code error;
            error = _authenticatorStrandPair->first->logoutUserForCookie(connectionMessage_._parameters[0],
                                                                         yield_);
            ConnectionMessage replyMessage(connectionMessage_._requestId,
                                           ConnectionMessageType::SUCCEEDED,
                                           std::vector<std::string>());
            if(error)
            {
                replyMessage._type = ConnectionMessageType::FAILED;
            }
            _connectionServer.sendMessage(connectionMessage_._requestId._initiatingConnectionId,
                                          replyMessage,
                                          false,
                                          boost::none);
            break;
        }
                
        case ConnectionMessageType::CR_LOGOUT_USER_FOR_ALL_COOKIES:
        {
            std::error_code error;
            error = _authenticatorStrandPair->first->logoutUserForAllCookies(connectionMessage_._parameters[0],
                                                                             yield_);
            ConnectionMessage replyMessage(connectionMessage_._requestId,
                                           ConnectionMessageType::SUCCEEDED,
                                           std::vector<std::string>());
            if(error)
            {
                replyMessage._type = ConnectionMessageType::FAILED;
            }
            _connectionServer.sendMessage(connectionMessage_._requestId._initiatingConnectionId,
                                          replyMessage,
                                          false,
                                          boost::none);    
            break;
        }
                
        case  ConnectionMessageType::CR_CHANGE_USER_PASSWORD:
        {
            std::error_code error;
            error = _authenticatorStrandPair->first->changeUserPassword(connectionMessage_._parameters[0],
                                                                        connectionMessage_._parameters[1],
                                                                        connectionMessage_._parameters[2],
                                                                        yield_);
            ConnectionMessage replyMessage(connectionMessage_._requestId,
                                           ConnectionMessageType::SUCCEEDED,
                                           std::vector<std::string>());
            if(error)
            {
                replyMessage._type = ConnectionMessageType::FAILED;
            }
            _connectionServer.sendMessage(connectionMessage_._requestId._initiatingConnectionId,
                                          replyMessage,
                                          false,
                                          boost::none);
            break;
        }
        
        case ConnectionMessageType::CR_GET_USER_FOR_EMAIL_ADDRESS_AND_RECOVERY_STRING:
        {
            std::error_code error;
            boost::optional<std::string> stringOpt;
            std::tie(error,stringOpt) = _authenticatorStrandPair->first->getUserForEmailAddressAndRecoveryString(connectionMessage_._parameters[0],
                                                                                                                 connectionMessage_._parameters[1],
                                                                                                                 yield_);
            std::vector<std::string> parameters = std::vector<std::string>();
            if(stringOpt)
            {
                parameters.push_back(*stringOpt);
            }
                
            ConnectionMessage replyMessage(connectionMessage_._requestId,
                                           ConnectionMessageType::SUCCEEDED,
                                           parameters);
            if(error)
            {
                replyMessage._type = ConnectionMessageType::FAILED;
            }
            _connectionServer.sendMessage(connectionMessage_._requestId._initiatingConnectionId,
                                          replyMessage,
                                          false,
                                          boost::none);
            break;
        }   
                
        case ConnectionMessageType::CR_CHANGE_USER_PASSWORD_FOR_EMAIL_ADDRESS_AND_RECOVERY_STRING:
        {
            std::error_code error;
            error = _authenticatorStrandPair->first->changeUserPasswordForEmailAddressAndRecoveryString(connectionMessage_._parameters[0],
                                                                                                        connectionMessage_._parameters[1],
                                                                                                        connectionMessage_._parameters[2],
                                                                                                        yield_);
            ConnectionMessage replyMessage(connectionMessage_._requestId,
                                           ConnectionMessageType::SUCCEEDED,
                                           std::vector<std::string>());
            if(error)
            {
                replyMessage._type = ConnectionMessageType::FAILED;
            }
            _connectionServer.sendMessage(connectionMessage_._requestId._initiatingConnectionId,
                                          replyMessage,
                                          false,
                                          boost::none);
            break;
        }
            
        case ConnectionMessageType::CR_CHANGE_USER_EMAIL_ADDRESS:
        {
            std::error_code error;
            error = _authenticatorStrandPair->first->changeUserEmailAddress(connectionMessage_._parameters[0],
                                                                            connectionMessage_._parameters[1],
                                                                            connectionMessage_._parameters[2],
                                                                            yield_);
            ConnectionMessage replyMessage(connectionMessage_._requestId,
                                           ConnectionMessageType::SUCCEEDED,
                                           std::vector<std::string>());
            if(error)
            {
                replyMessage._type = ConnectionMessageType::FAILED;
            }
            _connectionServer.sendMessage(connectionMessage_._requestId._initiatingConnectionId,
                                          replyMessage,
                                          false,
                                          boost::none);
            break;
        }   
                
        case ConnectionMessageType::CR_CREATE_AND_RETURN_RECOVERY_STRING:
        {
            std::error_code error;
            boost::optional<std::string> stringOpt;
            std::tie(error,stringOpt) = _authenticatorStrandPair->first->createAndReturnRecoveryString(connectionMessage_._parameters[0],
                                                                                                       yield_);
            std::vector<std::string> parameters = std::vector<std::string>();
            if(stringOpt)
            {
                parameters.push_back(*stringOpt);
            }
                
            ConnectionMessage replyMessage(connectionMessage_._requestId,
                                           ConnectionMessageType::SUCCEEDED,
                                           parameters);
            if(error)
            {
                replyMessage._type = ConnectionMessageType::FAILED;
            }
            _connectionServer.sendMessage(connectionMessage_._requestId._initiatingConnectionId,
                                          replyMessage,
                                          false,
                                          boost::none);
            break;
        }   
                
        case ConnectionMessageType::CR_DELETE_USER:
        {
            std::error_code error;
            error = _authenticatorStrandPair->first->deleteUser(connectionMessage_._parameters[0],
                                                                yield_);
            ConnectionMessage replyMessage(connectionMessage_._requestId,
                                           ConnectionMessageType::SUCCEEDED,
                                           std::vector<std::string>());
            if(error)
            {
                replyMessage._type = ConnectionMessageType::FAILED;
            }
            _connectionServer.sendMessage(connectionMessage_._requestId._initiatingConnectionId,
                                          replyMessage,
                                          false,
                                          boost::none);
            break;
        }   
                
        case ConnectionMessageType::SR_PING:
            BOOST_ASSERT(false);
            break;
                
        case ConnectionMessageType::SR_LOGOUT_USER_FOR_COOKIE:
            BOOST_ASSERT(false);
            break;

        case  ConnectionMessageType::SR_LOGOUT_USER_FOR_ALL_COOKIES:
            BOOST_ASSERT(false);
            break;
            
        }
    }

}}}}
