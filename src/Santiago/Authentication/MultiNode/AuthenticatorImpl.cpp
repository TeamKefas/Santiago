#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <sstream>

#include <openssl/sha.h>

#include "AuthenticatorImpl.h"
#include "TimeDefs.h"

namespace Santiago{ namespace Authentication{ namespace MultiNode
        {
            AuthenticatorImpl::AuthenticatorImpl(boost::asio::io_service& ioService_,
                                                 const StrandPtr& strandPtr_,
                                                 const boost::asio::ip::tcp::endpoint& serverEndPoint_)
                :AuthenticatorImplBase()
                ,_ioService(ioService_)
                ,_strandPtr(new boost::asio::strand(_ioService))
                ,_connectionRequestsController(_ioService,
                                               _strandPtr,
                                               serverEndPoint_,
                                               std::bind(&AuthenticatorImpl::handleConnectionDisconnect,this),
                                               std::bind(&AuthenticatorImpl::handleServerRequestMessage,
                                                         this,std::placeholders::_1))
                ,_clientCache()
                ,_lastPingTimeOpt()
                ,_lastRequestId(0)
            {}

            std::pair<std::error_code,boost::optional<UserInfo> >
            verifyCookieAndGetUserInfo(const std::string& cookieString_,
                                       boost::asio::yield_context yield_)
            {
                typename boost::asio::handler_type<boost::asio::yield_context, void()>::type
                    handler(std::forward<boost::asio::yield_context>(yield_));
        
                boost::asio::async_result<decltype(handler)> result(handler);
                std::pair<std::error_code,boost::optional<UserInfo> > ret;
                
                verifyCookieAndGetUserInfoImpl(
                    cookieString_,
                    [&ret,handler](const std::error_code& error_,const boost::optional<UserInfo>& userInfo_)
                    {
                        ret.first = error_;
                        ret.second = userInfo_;
                        asio_handler_invoke(handler, &handler);
                    });
                result.get();
                return ret;
            }
                        
            void AuthenticatorImpl::verifyCookieAndGetUserInfoImpl(const std::string& cookieString_,
                                                                   const ErrorCodeUserInfoCallbackFn& onVerifyUserCallbackFn_)
            {
                if(!_lastPingTimeOpt ||
                   (static_cast<double>(clock()-*_lastPingTimeOpt)/CLOCKS_PER_SEC > WITHOUT_PING_DISCONNECT_INTERVAL))
                {
                    return onVerifyUserCallbackFn_(
                        std::error_code(ErrorCode::ERR_AUTH_SERVER_CONNECTION_ERROR,
                                        ErrorCategory::GetInstance()),
                        boost::none);
                }
        
                boost::optional<UserInfo> userInfoOpt = _clientCache.getCookieInfoFromLocalCache(cookieString_);
        
                if(userInfoOpt == boost::none)
                {
                    ST_LOG_INFO("Cookie not in _cookieStringUserNameMap. cookieString:" <<cookieString_<<std::endl);
                    std::vector<std::string> parameters;
                    ConnectionMessageType connectionMessageType = ConnectionMessageType::CR_VERIFY_COOKIE_AND_GET_USER_INFO;
                    parameters.push_back(cookieString_);

                    RequestId requestId(_connectionRequestsController.getConnectionId(),_lastRequestId+1);
                    _lastRequestId++;
            
                    ConnectionMessage connectionMessage(requestId, connectionMessageType, parameters);
                    ErrorCodeConnectionMessageOptCallBackFn errorCodeConnectionMessageOptCallBackFn =  std::bind(&AuthenticatorImpl::handleVerifyCookieConnectionMessage,
                                                                                                                 this,
                                                                                                                 std::placeholders::_1,
                                                                                                                 std::placeholders::_2,
                                                                                                                 onVerifyUserCallbackFn_);
                    const boost::optional<ErrorCodeConnectionMessageOptCallBackFn> errorCodeConnectionMessageOptCallBackFnOpt = errorCodeConnectionMessageOptCallBackFnOpt;
                    _connectionRequestsController.sendMessage(connectionMessage,
                                                              true,
                                                              errorCodeConnectionMessageOptCallBackFnOpt);
                }
                else
                {
                    onVerifyUserCallbackFn_(std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance()),
                                            *userInfoOpt); 
                }
            }

            void AuthenticatorImpl::handleVerifyCookieConnectionMessage(
                const std::error_code& error_,
                const boost::optional<ConnectionMessage>& connectionMessageOpt_,
                const ErrorCodeUserInfoCallbackFn& onVerifyUserCallbackFn_)
            {
                if(!error_)
                {
                    if(connectionMessageOpt_->_type == ConnectionMessageType::SUCCEEDED)
                    {
                        onVerifyUserCallbackFn_(std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance()),
                                                UserInfo(connectionMessageOpt_->_parameters[0],  //userName
                                                         connectionMessageOpt_->_parameters[1])); //emailAddress
                    }
                    else if(connectionMessageOpt_->_type == ConnectionMessageType::FAILED)
                    {
                        //will change with appropriate error
                        onVerifyUserCallbackFn_(getErrorCodeFromConnectionMessage(*connectionMessageOpt_),
                                                boost::none);
                    }
                }
                else
                {
                    onVerifyUserCallbackFn_(error_,boost::none);
                }        
            }   
    
            void AuthenticatorImpl::createUserImpl(const std::string& userName_,
                                                   const std::string& emailAddress_,
                                                   const std::string& password_,
                                                   const ErrorCodeCallbackFn& onCreateUserCallbackFn_)
            {
                if(static_cast<double>(clock()-*_lastPingTimeOpt)/CLOCKS_PER_SEC > WITHOUT_PING_DISCONNECT_INTERVAL)
                {
                    return onCreateUserCallbackFn_(
                        std::error_code(ErrorCode::ERR_AUTH_SERVER_CONNECTION_ERROR,
                                        ErrorCategory::GetInstance()));
                }
        
                std::vector<std::string> parameters;
                ConnectionMessageType connectionMessageType = ConnectionMessageType::CR_CREATE_USER;
                parameters.push_back(userName_);
                parameters.push_back(emailAddress_);
                parameters.push_back(generateSHA256(password_));

                RequestId requestId(_connectionRequestsController.getConnectionId(),_lastRequestId+1);
                _lastRequestId++;
        
                ConnectionMessage connectionMessage(requestId, connectionMessageType, parameters);
                ErrorCodeConnectionMessageOptCallBackFn errorCodeConnectionMessageOptCallBackFn =  std::bind(&AuthenticatorImpl::handleCreateUserConnectionMessage,
                                                                                                             this,
                                                                                                             std::placeholders::_1,
                                                                                                             std::placeholders::_2,
                                                                                                             onCreateUserCallbackFn_);
                const boost::optional<ErrorCodeConnectionMessageOptCallBackFn> errorCodeConnectionMessageOptCallBackFnOpt = errorCodeConnectionMessageOptCallBackFnOpt;
                _connectionRequestsController.sendMessage(connectionMessage,
                                                          true,
                                                          errorCodeConnectionMessageOptCallBackFnOpt);
            }

            void AuthenticatorImpl::handleCreateUserConnectionMessage(const std::error_code& error_,
                                                                      const boost::optional<ConnectionMessage>& connectionMessageOpt_,
                                                                      const ErrorCodeCallbackFn& onCreateUserCallbackFn_)
            {
                if(!error_)
                {
                    if(connectionMessageOpt_->_type == ConnectionMessageType::SUCCEEDED)
                    {
                        onCreateUserCallbackFn_(std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance()));
                    }
                    else if(connectionMessageOpt_->_type == ConnectionMessageType::FAILED)
                    {
                        //will change with appropriate error
                        onCreateUserCallbackFn_(std::error_code(ErrorCode::ERR_DATABASE_EXCEPTION,ErrorCategory::GetInstance()));
                    }
                }
                else
                {
                    onCreateUserCallbackFn_(error_);
                }    
            }
    
            void AuthenticatorImpl::loginUserImpl(const std::string& userNameOrEmailAddress_,
                                                  bool isUserNameNotEmailAddress_,
                                                  const std::string& password_,
                                                  const ErrorCodeUserInfoStringPairCallbackFn& onLoginUserCallbackFn_)
            {
                if(static_cast<double>(clock()-*_lastPingTimeOpt)/CLOCKS_PER_SEC > WITHOUT_PING_DISCONNECT_INTERVAL)
                {
                    return onLoginUserCallbackFn_(
                        std::error_code(ErrorCode::ERR_AUTH_SERVER_CONNECTION_ERROR,
                                        ErrorCategory::GetInstance()),
                        boost::none);
                }
        
                std::vector<std::string> parameters;
                ConnectionMessageType connectionMessageType = ConnectionMessageType::CR_LOGIN_USER;

                parameters.push_back(userNameOrEmailAddress_);
                if(isUserNameNotEmailAddress_)
                {
                    parameters.push_back("1");
                }
                else
                {
                    parameters.push_back("0");
                }
                parameters.push_back(generateSHA256(password_));

                RequestId requestId(_connectionRequestsController.getConnectionId(),_lastRequestId+1);
                _lastRequestId++;

                ConnectionMessage connectionMessage(requestId, connectionMessageType, parameters);
                ErrorCodeConnectionMessageOptCallBackFn errorCodeConnectionMessageOptCallBackFn =  std::bind(&AuthenticatorImpl::handleLoginUserConnectionMessage,
                                                                                                             this,
                                                                                                             std::placeholders::_1,
                                                                                                             std::placeholders::_2,
                                                                                                             onLoginUserCallbackFn_);
                const boost::optional<ErrorCodeConnectionMessageOptCallBackFn> errorCodeConnectionMessageOptCallBackFnOpt = errorCodeConnectionMessageOptCallBackFnOpt;
                _connectionRequestsController.sendMessage(connectionMessage,
                                                          true,
                                                          errorCodeConnectionMessageOptCallBackFnOpt);
            }

            void AuthenticatorImpl::handleLoginUserConnectionMessage(const std::error_code& error_,
                                                                     const boost::optional<ConnectionMessage>& connectionMessageOpt_,
                                                                     const ErrorCodeUserInfoStringPairCallbackFn& onLoginUserCallbackFn_)
            {
                if(!error_)
                {
                    if (connectionMessageOpt_->_type == ConnectionMessageType::SUCCEEDED)
                    {
                        _clientCache.addCookieUserInfoToCache(connectionMessageOpt_->_parameters[2],  //ccokiestring_
                                                              connectionMessageOpt_->_parameters[0], //username
                                                              connectionMessageOpt_->_parameters[1]); //emailAddress
                
                        onLoginUserCallbackFn_(std::error_code(ErrorCode::ERR_SUCCESS,
                                                               ErrorCategory::GetInstance()),
                                               std::make_pair(UserInfo(connectionMessageOpt_->_parameters[0], //username
                                                                       connectionMessageOpt_->_parameters[1]), // emailAddress
                                                              connectionMessageOpt_->_parameters[2]));  // cookieString
                    }
                    else if(connectionMessageOpt_->_type == ConnectionMessageType::FAILED)
                    {
                        //will change with appropriate error
                        onLoginUserCallbackFn_(std::error_code(ErrorCode::ERR_DATABASE_EXCEPTION,ErrorCategory::GetInstance()),boost::none);
                    }
                }
                else
                {
                    onLoginUserCallbackFn_(error_,boost::none);
                }    
            }

            void AuthenticatorImpl::logoutUserForCookieImpl(const std::string& cookieString_,
                                                            const ErrorCodeCallbackFn& onLogoutCookieCallbackFn_)
            {
                if(static_cast<double>(clock()-*_lastPingTimeOpt)/CLOCKS_PER_SEC > WITHOUT_PING_DISCONNECT_INTERVAL)
                {
                    return onLogoutCookieCallbackFn_(
                        std::error_code(ErrorCode::ERR_AUTH_SERVER_CONNECTION_ERROR,
                                        ErrorCategory::GetInstance()));
                }
        
                std::vector<std::string> parameters;
                ConnectionMessageType connectionMessageType = ConnectionMessageType::CR_LOGOUT_USER_FOR_COOKIE;

                parameters.push_back(cookieString_);

                RequestId requestId(_connectionRequestsController.getConnectionId(),_lastRequestId+1);
                _lastRequestId++;
        
                ConnectionMessage connectionMessage(requestId, connectionMessageType, parameters);
                ErrorCodeConnectionMessageOptCallBackFn errorCodeConnectionMessageOptCallBackFn =  std::bind(&AuthenticatorImpl::handleLogoutUserForCookieConnectionMessage,
                                                                                                             this,
                                                                                                             std::placeholders::_1,
                                                                                                             std::placeholders::_2,
                                                                                                             onLogoutCookieCallbackFn_);
                const boost::optional<ErrorCodeConnectionMessageOptCallBackFn> errorCodeConnectionMessageOptCallBackFnOpt = errorCodeConnectionMessageOptCallBackFnOpt;
                _connectionRequestsController.sendMessage(connectionMessage,
                                                          true,
                                                          errorCodeConnectionMessageOptCallBackFnOpt);
            }

            void AuthenticatorImpl::handleLogoutUserForCookieConnectionMessage(const std::error_code& error_,
                                                                               const boost::optional<ConnectionMessage>& connectionMessageOpt_,
                                                                               const ErrorCodeCallbackFn& onLogoutCookieCallbackFn_)
            {
                if(!error_)
                {
                    if (connectionMessageOpt_->_type == ConnectionMessageType::SUCCEEDED)
                    {
                        _clientCache.removeCookieUsernameFromCache(connectionMessageOpt_->_parameters[2], //cookie string
                                                                   connectionMessageOpt_->_parameters[0], //userName
                                                                   connectionMessageOpt_->_parameters[1]); //emailAddress
                        onLogoutCookieCallbackFn_(std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance()));
                    }
                    else if(connectionMessageOpt_->_type == ConnectionMessageType::FAILED)
                    {
                        //will change with appropriate error
                        onLogoutCookieCallbackFn_(std::error_code(ErrorCode::ERR_DATABASE_EXCEPTION,ErrorCategory::GetInstance()));
                    }
                }
                else
                {
                    onLogoutCookieCallbackFn_(error_);
                }    
            }

            void AuthenticatorImpl::logoutUserForAllCookiesImpl(const std::string& userName_,
                                                                const ErrorCodeCallbackFn& onLogoutAllCookiesCallbackFn_)
            {
                if(static_cast<double>(clock()-*_lastPingTimeOpt)/CLOCKS_PER_SEC > WITHOUT_PING_DISCONNECT_INTERVAL)
                {
                    return onLogoutAllCookiesCallbackFn_(
                        std::error_code(ErrorCode::ERR_AUTH_SERVER_CONNECTION_ERROR,
                                        ErrorCategory::GetInstance()));
                }
        
                std::vector<std::string> parameters;
                ConnectionMessageType connectionMessageType = ConnectionMessageType::CR_LOGOUT_USER_FOR_ALL_COOKIES;

                parameters.push_back(userName_);

                RequestId requestId(_connectionRequestsController.getConnectionId(),_lastRequestId+1);
                _lastRequestId++;
        
                ConnectionMessage connectionMessage(requestId, connectionMessageType, parameters);
                ErrorCodeConnectionMessageOptCallBackFn errorCodeConnectionMessageOptCallBackFn =  std::bind(&AuthenticatorImpl::handleLogoutUserForAllCookiesConnectionMessage,
                                                                                                             this,
                                                                                                             std::placeholders::_1,
                                                                                                             std::placeholders::_2,
                                                                                                             onLogoutAllCookiesCallbackFn_);
                const boost::optional<ErrorCodeConnectionMessageOptCallBackFn> errorCodeConnectionMessageOptCallBackFnOpt = errorCodeConnectionMessageOptCallBackFnOpt;
                _connectionRequestsController.sendMessage(connectionMessage,
                                                          true,
                                                          errorCodeConnectionMessageOptCallBackFnOpt);
            }

            void AuthenticatorImpl::handleLogoutUserForAllCookiesConnectionMessage(const std::error_code& error_,
                                                                                   const boost::optional<ConnectionMessage>& connectionMessageOpt_,
                                                                                   const ErrorCodeCallbackFn& onLogoutAllCookiesCallbackFn_)
            {
                if(!error_)
                {
                    if (connectionMessageOpt_->_type == ConnectionMessageType::SUCCEEDED)
                    {
                        _clientCache.removeAllCookiesForUser(connectionMessageOpt_->_parameters[0], //userName
                                                             connectionMessageOpt_->_parameters[1]); //emailAddress 
                        onLogoutAllCookiesCallbackFn_(std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance()));
                    }
                    else if(connectionMessageOpt_->_type == ConnectionMessageType::FAILED)
                    {
                        //will change with appropriate error
                        onLogoutAllCookiesCallbackFn_(std::error_code(ErrorCode::ERR_DATABASE_EXCEPTION,ErrorCategory::GetInstance()));
                    }
                }
                else
                {
                    onLogoutAllCookiesCallbackFn_(error_);
                }    
            }

            void AuthenticatorImpl::changeUserPasswordImpl(const std::string& cookieString_,
                                                           const std::string& oldPassword_,
                                                           const std::string& newPassword_,
                                                           const ErrorCodeCallbackFn& onChangePasswordCallbackFn_)
            {
                if(static_cast<double>(clock()-*_lastPingTimeOpt)/CLOCKS_PER_SEC > WITHOUT_PING_DISCONNECT_INTERVAL)
                {
                    return onChangePasswordCallbackFn_(
                        std::error_code(ErrorCode::ERR_AUTH_SERVER_CONNECTION_ERROR,
                                        ErrorCategory::GetInstance()));
                }
        
                std::vector<std::string> parameters;
                ConnectionMessageType connectionMessageType = ConnectionMessageType::CR_CHANGE_USER_PASSWORD;

                parameters.push_back(cookieString_);
                parameters.push_back(generateSHA256(oldPassword_));
                parameters.push_back(generateSHA256(newPassword_));

                RequestId requestId(_connectionRequestsController.getConnectionId(),_lastRequestId+1);
                _lastRequestId++;
        
                ConnectionMessage connectionMessage(requestId, connectionMessageType, parameters);
                ErrorCodeConnectionMessageOptCallBackFn errorCodeConnectionMessageOptCallBackFn =  std::bind(&AuthenticatorImpl::handleChangeUserPasswordConnectionMessage,
                                                                                                             this,
                                                                                                             std::placeholders::_1,
                                                                                                             std::placeholders::_2,
                                                                                                             onChangePasswordCallbackFn_);
                const boost::optional<ErrorCodeConnectionMessageOptCallBackFn> errorCodeConnectionMessageOptCallBackFnOpt = errorCodeConnectionMessageOptCallBackFnOpt;
                _connectionRequestsController.sendMessage(connectionMessage,
                                                          true,
                                                          errorCodeConnectionMessageOptCallBackFnOpt);
            }

            void AuthenticatorImpl::handleChangeUserPasswordConnectionMessage(const std::error_code& error_,
                                                                              const boost::optional<ConnectionMessage>& connectionMessageOpt_,
                                                                              const ErrorCodeCallbackFn& onChangePasswordCallbackFn_)
            {
                if(!error_)
                {
                    if (connectionMessageOpt_->_type == ConnectionMessageType::SUCCEEDED)
                    {
                        onChangePasswordCallbackFn_(std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance()));
                    }
                    else if(connectionMessageOpt_->_type == ConnectionMessageType::FAILED)
                    {
                        //will change with appropriate error
                        onChangePasswordCallbackFn_(std::error_code(ErrorCode::ERR_DATABASE_EXCEPTION,ErrorCategory::GetInstance()));
                    }
                }
                else
                {
                    onChangePasswordCallbackFn_(error_);
                }    
            }

            void AuthenticatorImpl::getUserForEmailAddressAndRecoveryStringImpl(const std::string& emailAddress_,
                                                                                const std::string& recoveryString_,
                                                                                const ErrorCodeStringCallbackFn& onGetUserForEmailAddressAndRecoveryStringCallbackFn_)
            {
                if(static_cast<double>(clock()-*_lastPingTimeOpt)/CLOCKS_PER_SEC > WITHOUT_PING_DISCONNECT_INTERVAL)
                {
                    return onGetUserForEmailAddressAndRecoveryStringCallbackFn_(
                        std::error_code(ErrorCode::ERR_AUTH_SERVER_CONNECTION_ERROR,
                                        ErrorCategory::GetInstance()),
                        boost::none);
                }
        
                std::vector<std::string> parameters;
                ConnectionMessageType connectionMessageType = ConnectionMessageType::CR_GET_USER_FOR_EMAIL_ADDRESS_AND_RECOVERY_STRING;

                parameters.push_back(emailAddress_);
                parameters.push_back(recoveryString_);

                RequestId requestId(_connectionRequestsController.getConnectionId(),_lastRequestId+1);
                _lastRequestId++;
        
                ConnectionMessage connectionMessage(requestId, connectionMessageType, parameters);
                ErrorCodeConnectionMessageOptCallBackFn errorCodeConnectionMessageOptCallBackFn =  std::bind(&AuthenticatorImpl::handleGetUserForEmailAddressAndRecoveryStringConnectionMessage,
                                                                                                             this,
                                                                                                             std::placeholders::_1,
                                                                                                             std::placeholders::_2,
                                                                                                             onGetUserForEmailAddressAndRecoveryStringCallbackFn_);
                const boost::optional<ErrorCodeConnectionMessageOptCallBackFn> errorCodeConnectionMessageOptCallBackFnOpt = errorCodeConnectionMessageOptCallBackFnOpt;
                _connectionRequestsController.sendMessage(connectionMessage,
                                                          true,
                                                          errorCodeConnectionMessageOptCallBackFnOpt);
            }

            void AuthenticatorImpl::handleGetUserForEmailAddressAndRecoveryStringConnectionMessage(const std::error_code& error_,
                                                                                                   const boost::optional<ConnectionMessage>& connectionMessageOpt_,
                                                                                                   const ErrorCodeStringCallbackFn& onGetUserForEmailAddressAndRecoveryStringCallbackFn_)
            {
                if(!error_)
                {
                    if (connectionMessageOpt_->_type == ConnectionMessageType::SUCCEEDED)
                    {
                        onGetUserForEmailAddressAndRecoveryStringCallbackFn_(std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance()),connectionMessageOpt_->_parameters[0]);
                    }
                    else if(connectionMessageOpt_->_type == ConnectionMessageType::FAILED)
                    {
                        //will change with appropriate error
                        onGetUserForEmailAddressAndRecoveryStringCallbackFn_(std::error_code(ErrorCode::ERR_DATABASE_EXCEPTION,ErrorCategory::GetInstance()),boost::none);
                    }
                }
                else
                {
                    onGetUserForEmailAddressAndRecoveryStringCallbackFn_(error_,boost::none);
                }    
            }

            void AuthenticatorImpl::changeUserPasswordForEmailAddressAndRecoveryStringImpl(const std::string& emailAddress_,
                                                                                           const std::string& recoveryString_,
                                                                                           const std::string& newPassword_,
                                                                                           const ErrorCodeCallbackFn& onChangePasswordForEmailAddressAndRecoveryStringCallbackFn_)
            {
                if(static_cast<double>(clock()-*_lastPingTimeOpt)/CLOCKS_PER_SEC > WITHOUT_PING_DISCONNECT_INTERVAL)
                {
                    return onChangePasswordForEmailAddressAndRecoveryStringCallbackFn_(
                        std::error_code(ErrorCode::ERR_AUTH_SERVER_CONNECTION_ERROR,
                                        ErrorCategory::GetInstance()));
                }
        
                std::vector<std::string> parameters;
                ConnectionMessageType connectionMessageType = ConnectionMessageType::CR_CHANGE_USER_PASSWORD_FOR_EMAIL_ADDRESS_AND_RECOVERY_STRING;    

                parameters.push_back(emailAddress_);
                parameters.push_back(recoveryString_);
                parameters.push_back(generateSHA256(newPassword_));

                RequestId requestId(_connectionRequestsController.getConnectionId(),_lastRequestId+1);
                _lastRequestId++;
        
                ConnectionMessage connectionMessage(requestId, connectionMessageType, parameters);
                ErrorCodeConnectionMessageOptCallBackFn errorCodeConnectionMessageOptCallBackFn =  std::bind(&AuthenticatorImpl::handleChangeUserPasswordForEmailAddressAndRecoveryStringConnectionMessage,
                                                                                                             this,
                                                                                                             std::placeholders::_1,
                                                                                                             std::placeholders::_2,
                                                                                                             onChangePasswordForEmailAddressAndRecoveryStringCallbackFn_);
                const boost::optional<ErrorCodeConnectionMessageOptCallBackFn> errorCodeConnectionMessageOptCallBackFnOpt = errorCodeConnectionMessageOptCallBackFnOpt;
                _connectionRequestsController.sendMessage(connectionMessage,
                                                          true,
                                                          errorCodeConnectionMessageOptCallBackFnOpt);
            }

            void AuthenticatorImpl::handleChangeUserPasswordForEmailAddressAndRecoveryStringConnectionMessage(const std::error_code& error_,
                                                                                                              const boost::optional<ConnectionMessage>& connectionMessageOpt_,
                                                                                                              const ErrorCodeCallbackFn& onChangePasswordForEmailAddressAndRecoveryStringCallbackFn_)
            {
                if(!error_)
                {
                    if (connectionMessageOpt_->_type == ConnectionMessageType::SUCCEEDED)
                    {
                        onChangePasswordForEmailAddressAndRecoveryStringCallbackFn_(std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance()));
                    }
                    else if(connectionMessageOpt_->_type == ConnectionMessageType::FAILED)
                    {
                        //will change with appropriate error
                        onChangePasswordForEmailAddressAndRecoveryStringCallbackFn_(std::error_code(ErrorCode::ERR_DATABASE_EXCEPTION,ErrorCategory::GetInstance()));
                    }
                }
                else
                {
                    onChangePasswordForEmailAddressAndRecoveryStringCallbackFn_(error_);
                }    
            }

            void AuthenticatorImpl::changeUserEmailAddressImpl(const std::string& cookieString_,
                                                               const std::string& newEmailAddress_,
                                                               const std::string& password_,
                                                               const ErrorCodeCallbackFn& onChangeEmailAddressCallbackFn_)
            {
                if(static_cast<double>(clock()-*_lastPingTimeOpt)/CLOCKS_PER_SEC > WITHOUT_PING_DISCONNECT_INTERVAL)
                {
                    return onChangeEmailAddressCallbackFn_(
                        std::error_code(ErrorCode::ERR_AUTH_SERVER_CONNECTION_ERROR,
                                        ErrorCategory::GetInstance()));
                }
        
                std::vector<std::string> parameters;
                ConnectionMessageType connectionMessageType = ConnectionMessageType::CR_CHANGE_USER_EMAIL_ADDRESS;

                parameters.push_back(cookieString_);
                parameters.push_back(newEmailAddress_);
                parameters.push_back(generateSHA256(password_));

                RequestId requestId(_connectionRequestsController.getConnectionId(),_lastRequestId+1);
                _lastRequestId++;
        
                ConnectionMessage connectionMessage(requestId, connectionMessageType, parameters);
                ErrorCodeConnectionMessageOptCallBackFn errorCodeConnectionMessageOptCallBackFn =  std::bind(&AuthenticatorImpl::handleChangeUserEmailAddressConnectionMessage,
                                                                                                             this,
                                                                                                             std::placeholders::_1,
                                                                                                             std::placeholders::_2,
                                                                                                             onChangeEmailAddressCallbackFn_);
                const boost::optional<ErrorCodeConnectionMessageOptCallBackFn> errorCodeConnectionMessageOptCallBackFnOpt = errorCodeConnectionMessageOptCallBackFnOpt;
                _connectionRequestsController.sendMessage(connectionMessage,
                                                          true,
                                                          errorCodeConnectionMessageOptCallBackFnOpt);
            }

            void AuthenticatorImpl::handleChangeUserEmailAddressConnectionMessage(const std::error_code& error_,
                                                                                  const boost::optional<ConnectionMessage>& connectionMessageOpt_,
                                                                                  const ErrorCodeCallbackFn& onChangeEmailAddressCallbackFn_)
            {
                if(!error_)
                {
                    if (connectionMessageOpt_->_type == ConnectionMessageType::SUCCEEDED)
                    {
                        onChangeEmailAddressCallbackFn_(std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance()));
                    }
                    else if(connectionMessageOpt_->_type == ConnectionMessageType::FAILED)
                    {
                        //will change with appropriate error
                        onChangeEmailAddressCallbackFn_(std::error_code(ErrorCode::ERR_DATABASE_EXCEPTION,ErrorCategory::GetInstance()));
                    }
                }
                else
                {
                    onChangeEmailAddressCallbackFn_(error_);
                }    
            }
        
            void AuthenticatorImpl::createAndReturnRecoveryStringImpl(const std::string& emailAddress_,
                                                                      const ErrorCodeStringCallbackFn& onCreateAndReturnRecoveryStringCallbackFn_)
            {
                if(static_cast<double>(clock()-*_lastPingTimeOpt)/CLOCKS_PER_SEC > WITHOUT_PING_DISCONNECT_INTERVAL)
                {
                    return onCreateAndReturnRecoveryStringCallbackFn_(
                        std::error_code(ErrorCode::ERR_AUTH_SERVER_CONNECTION_ERROR,
                                        ErrorCategory::GetInstance()),
                        boost::none);
                }
        
                std::vector<std::string> parameters;
                ConnectionMessageType connectionMessageType = ConnectionMessageType::CR_CREATE_AND_RETURN_RECOVERY_STRING;   

                parameters.push_back(emailAddress_);

                RequestId requestId(_connectionRequestsController.getConnectionId(),_lastRequestId+1);
                _lastRequestId++;
        
                ConnectionMessage connectionMessage(requestId, connectionMessageType, parameters);
                ErrorCodeConnectionMessageOptCallBackFn errorCodeConnectionMessageOptCallBackFn =  std::bind(&AuthenticatorImpl::handleCreateAndReturnRecoveryStringConnectionMessage,
                                                                                                             this,
                                                                                                             std::placeholders::_1,
                                                                                                             std::placeholders::_2,
                                                                                                             onCreateAndReturnRecoveryStringCallbackFn_);
                const boost::optional<ErrorCodeConnectionMessageOptCallBackFn> errorCodeConnectionMessageOptCallBackFnOpt = errorCodeConnectionMessageOptCallBackFnOpt;
                _connectionRequestsController.sendMessage(connectionMessage,
                                                          true,
                                                          errorCodeConnectionMessageOptCallBackFnOpt);
            }

            void AuthenticatorImpl::handleCreateAndReturnRecoveryStringConnectionMessage(const std::error_code& error_,
                                                                                         const boost::optional<ConnectionMessage>& connectionMessageOpt_,
                                                                                         const ErrorCodeStringCallbackFn& onCreateAndReturnRecoveryStringCallbackFn_)
            {
                if(!error_)
                {
                    if (connectionMessageOpt_->_type == ConnectionMessageType::SUCCEEDED)
                    {
                        onCreateAndReturnRecoveryStringCallbackFn_(std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance()),connectionMessageOpt_->_parameters[0]);  //username
                    }
                    else if(connectionMessageOpt_->_type == ConnectionMessageType::FAILED)
                    {
                        //will change with appropriate error
                        onCreateAndReturnRecoveryStringCallbackFn_(std::error_code(ErrorCode::ERR_DATABASE_EXCEPTION,ErrorCategory::GetInstance()),boost::none);
                    }
                }
                else
                {
                    onCreateAndReturnRecoveryStringCallbackFn_(error_,boost::none);
                }    
            }

            void AuthenticatorImpl::deleteUserImpl(const std::string& cookieString_,
                                                   const ErrorCodeCallbackFn& onDeleteUserCallbackFn_)
            {
                if(static_cast<double>(clock()-*_lastPingTimeOpt)/CLOCKS_PER_SEC > WITHOUT_PING_DISCONNECT_INTERVAL)
                {
                    return onDeleteUserCallbackFn_(
                        std::error_code(ErrorCode::ERR_AUTH_SERVER_CONNECTION_ERROR,
                                        ErrorCategory::GetInstance()));
                }
        
                std::vector<std::string> parameters;
                ConnectionMessageType connectionMessageType = ConnectionMessageType::CR_DELETE_USER;

                parameters.push_back(cookieString_);

                RequestId requestId(_connectionRequestsController.getConnectionId(),_lastRequestId+1);
                _lastRequestId++;
        
                ConnectionMessage connectionMessage(requestId, connectionMessageType, parameters);
                ErrorCodeConnectionMessageOptCallBackFn errorCodeConnectionMessageOptCallBackFn =  std::bind(&AuthenticatorImpl::handleDeleteUserConnectionMessage,
                                                                                                             this,
                                                                                                             std::placeholders::_1,
                                                                                                             std::placeholders::_2,
                                                                                                             onDeleteUserCallbackFn_);
                const boost::optional<ErrorCodeConnectionMessageOptCallBackFn> errorCodeConnectionMessageOptCallBackFnOpt = errorCodeConnectionMessageOptCallBackFnOpt;
                _connectionRequestsController.sendMessage(connectionMessage,
                                                          true,
                                                          errorCodeConnectionMessageOptCallBackFnOpt);
            }

            void AuthenticatorImpl::handleDeleteUserConnectionMessage(const std::error_code& error_,
                                                                      const boost::optional<ConnectionMessage>& connectionMessageOpt_,
                                                                      const ErrorCodeCallbackFn& onDeleteUserCallbackFn_)
            {
                if(!error_)
                {
                    if (connectionMessageOpt_->_type == ConnectionMessageType::SUCCEEDED)
                    {
                        _clientCache.removeCookieUsernameFromCache(connectionMessageOpt_->_parameters[2],
                                                                   connectionMessageOpt_->_parameters[0],
                                                                   connectionMessageOpt_->_parameters[1]);
                        _clientCache.removeAllCookiesForUser(connectionMessageOpt_->_parameters[0],
                                                             connectionMessageOpt_->_parameters[1]);
                        onDeleteUserCallbackFn_(std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance()));
                    }
                    else if(connectionMessageOpt_->_type == ConnectionMessageType::FAILED)
                    {
                        //will change with appropriate error
                        onDeleteUserCallbackFn_(std::error_code(ErrorCode::ERR_DATABASE_EXCEPTION,ErrorCategory::GetInstance()));
                    }
                }
                else
                {
                    onDeleteUserCallbackFn_(error_);
                }    
            }
    
            std::string AuthenticatorImpl::generateSHA256(const std::string str)
            {
                unsigned char hash[SHA256_DIGEST_LENGTH];
                SHA256_CTX sha256;
                SHA256_Init(&sha256);
                SHA256_Update(&sha256, str.c_str(), str.size());
                SHA256_Final(hash, &sha256);
                std::stringstream ss;
                for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
                {
                    ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
                }
                return ss.str();
            }

            void AuthenticatorImpl::handleServerRequestMessage(const ConnectionMessage& connectionMessage_)
            {
                switch(connectionMessage_._type)
                {
                case ConnectionMessageType::SR_PING:
                    handleServerRequestPing(connectionMessage_);
                    break;
                    
                case ConnectionMessageType::SR_LOGOUT_USER_FOR_COOKIE:
                    handleServerRequestLogoutUserForCookie(connectionMessage_);
                    break;
                    
                case ConnectionMessageType::SR_LOGOUT_USER_FOR_ALL_COOKIES:
                    handleServerRequestLogoutUserForAllCookies(connectionMessage_);
                    break;
                    
                default:
                    ST_ASSERT(false);
                    return;
                }                
            }

            void AuthenticatorImpl::handleServerRequestPing(const ConnectionMessage& connectionMessage_)
            {
                _lastPingTimeOpt = clock();
                ConnectionMessage reply(connectionMessage_._requestId,
                                        ConnectionMessageType::SUCCEEDED,
                                        std::vector<std::string>());
                _connectionRequestsController.sendMessage(reply,false,boost::none);
            }

            void AuthenticatorImpl::handleServerRequestLogoutUserForCookie(const ConnectionMessage& connectionMessage_)
            {
                ST_ASSERT(connectionMessage_._parameters.size() == 1);
                _clientCache.removeCookieUsernameFromCache(connectionMessage_._parameters[0],
                                                           connectionMessage_._parameters[1],
                                                           connectionMessage_._parameters[2]);
                
                ConnectionMessage reply(connectionMessage_._requestId,
                                        ConnectionMessageType::SUCCEEDED,
                                        std::vector<std::string>());
                _connectionRequestsController.sendMessage(reply,false,boost::none);
            }

            void AuthenticatorImpl::
            handleServerRequestLogoutUserForAllCookies(const ConnectionMessage& connectionMessage_)
            {
                ST_ASSERT(connectionMessage_._parameters.size() == 1);
                _clientCache.removeAllCookiesForUser(connectionMessage_._parameters[0],
                                                     connectionMessage_._parameters[1]);
                
                ConnectionMessage reply(connectionMessage_._requestId,
                                        ConnectionMessageType::SUCCEEDED,
                                        std::vector<std::string>());
                _connectionRequestsController.sendMessage(reply,false,boost::none);
            }

            
}}}
