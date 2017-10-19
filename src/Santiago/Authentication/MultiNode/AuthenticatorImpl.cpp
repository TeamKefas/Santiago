#include "AuthenticatorImpl.h"

namespace Santiago{ namespace Authentication{ namespace MultiNode
{
    AuthenticatorImpl::AuthenticatorImpl(boost::asio::io_service& ioService_,const boost::property_tree::ptree& config_)
        :AuthenticatorImplBase(ioService_,config_)
        ,_strandPtr(new boost::asio::strand(_ioService))
        ,_connectionRequestsController(_ioService,
                                       _strandPtr,
                                       boost::asio::ip::tcp::endpoint(
                                           boost::asio::ip::tcp::address(_config.get<unsigned>("Santiago.AuthServer.address")),
                                           _config.get<unsigned>("Santiago.AuthServer.port")),
                                       std::bind(&AuthenticatorImpl::handleConnectionDisconnect,this),
                                       std::bind(&AuthenticatorImpl::handleServerRequestMessage,
                                                 this,std::placeholders::_1))
        ,_clientCache()
    {        
    }
    
    void AuthenticatorImpl::verifyCookieAndGetUserInfoImpl(const std::string& cookieString_,
                                                       const ErrorCodeUserInfoCallbackFn& onVerifyUserCallbackFn_)
    {
        boost::optional<UserInfo> userInfoOpt = _clientCache.getCookieInfoFromLocalCache(cookieString_);
        
        if(userInfoOpt == boost::none)
        {
            ST_LOG_INFO("Cookie not in _cookieStringUserNameMap. cookieString:" <<cookieString_<<std::endl);
            std::vector<std::string> parameters;
            ConnectionMessageType connectionMessageType = ConnectionMessageType::CR_VERIFY_COOKIE_AND_GET_USER_INFO;
            parameters.push_back(cookieString_);
             
            ConnectionMessage ConnectionMessage(connectionMessageType, parameters);
            _messageSocket.sendMessage(ConnectionMessage,
                                       std::bind(&AuthenticatorImpl::handleVerifyCookieConnectionMessage,
                                                 this,
                                                 std::placeholders::_1,
                                                 std::placeholders::_2,
                                                 onVerifyUserCallbackFn_));
        }
        else
        {
                    onVerifyUserCallbackFn_(std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance()),
                                            *userInfoOpt); 
        }
    }

    void AuthenticatorImpl::handleVerifyCookieConnectionMessage(const std::error_code& error_,
                                                            const ConnectionMessage& connectionMessage_,
                                                            const ErrorCodeUserInfoCallbackFn& onVerifyUserCallbackFn_)
    {
        if(!error_)
        {
            if(connectionMessage_._type == ConnectionMessageType::SUCCEEDED)
            {
                onVerifyUserCallbackFn_(std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance()),
                                        UserInfo(connectionMessage_._parameters[0],  //userName
                                                 connectionMessage_._parameters[1])); //emailAddress
            }
            else if(connectionMessage_._type == ConnectionMessageType::FAILED)
            {
                //will change with appropriate error
                onVerifyUserCallbackFn_(std::error_code(ErrorCode::ERR_DATABASE_EXCEPTION,ErrorCategory::GetInstance()),
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
        std::vector<std::string> parameters;
        ConnectionMessageType connectionMessageType = ConnectionMessageType::CR_CREATE_USER;
        parameters.push_back(userName_);
        parameters.push_back(emailAddress_);
        parameters.push_back(generateSHA256(password_));
        
        ConnectionMessage connectionMessage(connectionMessageType, parameters);
        _messageSocket.sendMessage(connectionMessage,
                                   std::bind(&AuthenticatorImpl::handleCreateUserConnectionMessage,
                                             this,
                                             std::placeholders::_1,
                                             std::placeholders::_2,
                                             onCreateUserCallbackFn_));
    }

    void AuthenticatorImpl::handleCreateUserConnectionMessage(const std::error_code& error_,
                                                          const ConnectionMessage& connectionMessage_,
                                                          const ErrorCodeCallbackFn& onCreateUserCallbackFn_)
    {
        if(!error_)
        {
            if(connectionMessage_._type == ConnectionMessageType::SUCCEEDED)
            {
                onCreateUserCallbackFn_(std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance()));
            }
            else if(connectionMessage_._type == ConnectionMessageType::FAILED)
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

        ConnectionMessage connectionMessage(connectionMessageType, parameters);
        _messageSocket.sendMessage(connectionMessage,
                                   std::bind(&AuthenticatorImpl::handleLoginUserConnectionMessage,
                                             this,
                                             std::placeholders::_1,
                                             std::placeholders::_2,
                                             onLoginUserCallbackFn_));
    }

    void AuthenticatorImpl::handleLoginUserConnectionMessage(const std::error_code& error_,
                                                         const ConnectionMessage& connectionMessage_,
                                                         const ErrorCodeUserInfoStringPairCallbackFn& onLoginUserCallbackFn_)
    {
        if(!error_)
        {
            if (connectionMessage_._type == ConnectionMessageType::SUCCEEDED)
            {
                _clientCache.addCookieUserInfoToCache(connectionMessage_._parameters[2],  //ccokiestring_
                                                      connectionMessage_._parameters[0], //username
                                                      connectionMessage_._parameters[1]); //emailAddress
                
                onLoginUserCallbackFn_(std::error_code(ErrorCode::ERR_SUCCESS,
                                                       ErrorCategory::GetInstance()),
                                       std::make_pair(UserInfo(connectionMessage_._parameters[0], //username
                                                               connectionMessage_._parameters[1]), // emailAddress
                                                      connectionMessage_._parameters[2]));  // cookieString
            }
            else if(connectionMessage_._type == ConnectionMessageType::FAILED)
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
        std::vector<std::string> parameters;
        ConnectionMessageType connectionMessageType = ConnectionMessageType::CR_LOGOUT_USER_FOR_COOKIE;

        parameters.push_back(cookieString_);
        
        ConnectionMessage connectionMessage(connectionMessageType, parameters);
        _messageSocket.sendMessage(connectionMessage,
                                   std::bind(&AuthenticatorImpl::handleLogoutUserForCookieConnectionMessage,
                                             this,
                                             std::placeholders::_1,
                                             std::placeholders::_2,
                                             onLogoutCookieCallbackFn_));
    }

    void AuthenticatorImpl::handleLogoutUserForCookieConnectionMessage(const std::error_code& error_,
                                                                   const ConnectionMessage& connectionMessage_,
                                                                   const ErrorCodeCallbackFn& onLogoutCookieCallbackFn_)
    {
        if(!error_)
        {
            if (connectionMessage_._type == ConnectionMessageType::SUCCEEDED)
            {
                _clientCache.removeCookieUsernameFromCache(connectionMessage_._parameters[2], //cookie string
                                                           connectionMessage_._parameters[0], //userName
                                                           connectionMessage_._parameters[1]); //emailAddress
                onLogoutCookieCallbackFn_(std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance()));
            }
            else if(connectionMessage_._type == ConnectionMessageType::FAILED)
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
        std::vector<std::string> parameters;
        ConnectionMessageType connectionMessageType = ConnectionMessageType::CR_LOGOUT_USER_FOR_ALL_COOKIES;

        parameters.push_back(userName_);
        
        ConnectionMessage connectionMessage(connectionMessageType, parameters);
        _messageSocket.sendMessage(connectionMessage,
                                   std::bind(&AuthenticatorImpl::handleLogoutUserForAllCookiesConnectionMessage,
                                             this,
                                             std::placeholders::_1,
                                             std::placeholders::_2,
                                             onLogoutAllCookiesCallbackFn_));
    }

    void AuthenticatorImpl::handleLogoutUserForAllCookiesConnectionMessage(const std::error_code& error_,
                                                                       const ConnectionMessage& connectionMessage_,
                                                                       const ErrorCodeCallbackFn& onLogoutAllCookiesCallbackFn_)
    {
        if(!error_)
        {
            if (connectionMessage_._type == ConnectionMessageType::SUCCEEDED)
            {
                _clientCache.removeAllCookiesForUser(connectionMessage_._parameters[0], //userName
                                                     connectionMessage_._parameters[1]); //emailAddress 
                onLogoutAllCookiesCallbackFn_(std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance()));
            }
            else if(connectionMessage_._type == ConnectionMessageType::FAILED)
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
        std::vector<std::string> parameters;
        ConnectionMessageType connectionMessageType = ConnectionMessageType::CR_CHANGE_USER_PASSWORD;

        parameters.push_back(cookieString_);
        parameters.push_back(generateSHA256(oldPassword_));
        parameters.push_back(generateSHA256(newPassword_));

        ConnectionMessage connectionMessage(connectionMessageType, parameters);
        _messageSocket.sendMessage(connectionMessage,
                                   std::bind(&AuthenticatorImpl::handleChangeUserPasswordConnectionMessage,
                                             this,
                                             std::placeholders::_1,
                                             std::placeholders::_2,
                                             onChangePasswordCallbackFn_));
    }

    void AuthenticatorImpl::handleChangeUserPasswordConnectionMessage(const std::error_code& error_,
                                                                  const ConnectionMessage& connectionMessage_,
                                                                  const ErrorCodeCallbackFn& onChangePasswordCallbackFn_)
    {
        if(!error_)
        {
            if (connectionMessage_._type == ConnectionMessageType::SUCCEEDED)
            {
                onChangePasswordCallbackFn_(std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance()));
            }
            else if(connectionMessage_._type == ConnectionMessageType::FAILED)
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
        std::vector<std::string> parameters;
        ConnectionMessageType connectionMessageType = ConnectionMessageType::CR_GET_USER_FOR_EMAIL_ADDRESS_AND_RECOVERY_STRING;

        parameters.push_back(emailAddress_);
        parameters.push_back(recoveryString_);

        ConnectionMessage connectionMessage(connectionMessageType, parameters);
        _messageSocket.sendMessage(connectionMessage,
                                   std::bind(&AuthenticatorImpl::handleGetUserForEmailAddressAndRecoveryStringConnectionMessage,
                                             this,
                                             std::placeholders::_1,
                                             std::placeholders::_2,
                                             onGetUserForEmailAddressAndRecoveryStringCallbackFn_));
    }

    void AuthenticatorImpl::handleGetUserForEmailAddressAndRecoveryStringConnectionMessage(const std::error_code& error_,
                                                                                       const ConnectionMessage& connectionMessage_,
                                                                                       const ErrorCodeStringCallbackFn& onGetUserForEmailAddressAndRecoveryStringCallbackFn_)
    {
        if(!error_)
        {
            if (connectionMessage_._type == ConnectionMessageType::SUCCEEDED)
            {
                onGetUserForEmailAddressAndRecoveryStringCallbackFn_(std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance()),connectionMessage_._parameters[0]);
            }
            else if(connectionMessage_._type == ConnectionMessageType::FAILED)
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
        std::vector<std::string> parameters;
        ConnectionMessageType connectionMessageType = ConnectionMessageType::CR_CHANGE_USER_PASSWORD_FOR_EMAIL_ADDRESS_AND_RECOVERY_STRING;    

        parameters.push_back(emailAddress_);
        parameters.push_back(recoveryString_);
        parameters.push_back(generateSHA256(newPassword_));

        ConnectionMessage connectionMessage(connectionMessageType, parameters);
        _messageSocket.sendMessage(connectionMessage,
                                   std::bind(&AuthenticatorImpl::handleChangeUserPasswordForEmailAddressAndRecoveryStringConnectionMessage,
                                             this,
                                             std::placeholders::_1,
                                             std::placeholders::_2,
                                             onChangePasswordForEmailAddressAndRecoveryStringCallbackFn_));
    }

    void AuthenticatorImpl::handleChangeUserPasswordForEmailAddressAndRecoveryStringConnectionMessage(const std::error_code& error_,
                                                                                                  const ConnectionMessage& connectionMessage_,
                                                                                                  const ErrorCodeCallbackFn& onChangePasswordForEmailAddressAndRecoveryStringCallbackFn_)
    {
        if(!error_)
        {
            if (connectionMessage_._type == ConnectionMessageType::SUCCEEDED)
            {
                onChangePasswordForEmailAddressAndRecoveryStringCallbackFn_(std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance()));
            }
            else if(connectionMessage_._type == ConnectionMessageType::FAILED)
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
        std::vector<std::string> parameters;
        ConnectionMessageType connectionMessageType = ConnectionMessageType::CR_CHANGE_USER_EMAIL_ADDRESS;

        parameters.push_back(cookieString_);
        parameters.push_back(newEmailAddress_);
        parameters.push_back(generateSHA256(password_));

        ConnectionMessage connectionMessage(connectionMessageType, parameters);
        _messageSocket.sendMessage(connectionMessage,
                                   std::bind(&AuthenticatorImpl::handleChangeUserEmailAddressConnectionMessage,
                                             this,
                                             std::placeholders::_1,
                                             std::placeholders::_2,
                                             onChangeEmailAddressCallbackFn_));
    }

    void AuthenticatorImpl::handleChangeUserEmailAddressConnectionMessage(const std::error_code& error_,
                                                                      const ConnectionMessage& connectionMessage_,
                                                                      const ErrorCodeCallbackFn& onChangeEmailAddressCallbackFn_)
    {
        if(!error_)
        {
            if (connectionMessage_._type == ConnectionMessageType::SUCCEEDED)
            {
                onChangeEmailAddressCallbackFn_(std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance()));
            }
            else if(connectionMessage_._type == ConnectionMessageType::FAILED)
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
        std::vector<std::string> parameters;
        ConnectionMessageType connectionMessageType = ConnectionMessageType::CR_CREATE_AND_RETURN_RECOVERY_STRING;   

        parameters.push_back(emailAddress_);

        ConnectionMessage connectionMessage(connectionMessageType, parameters);
        _messageSocket.sendMessage(connectionMessage,
                                   std::bind(&AuthenticatorImpl::handleCreateAndReturnRecoveryStringConnectionMessage,
                                             this,
                                             std::placeholders::_1,
                                             std::placeholders::_2,
                                             onCreateAndReturnRecoveryStringCallbackFn_));
    }

    void AuthenticatorImpl::handleCreateAndReturnRecoveryStringConnectionMessage(const std::error_code& error_,
                                                                             const ConnectionMessage& connectionMessage_,
                                                                             const ErrorCodeStringCallbackFn& onCreateAndReturnRecoveryStringCallbackFn_)
    {
        if(!error_)
        {
            if (connectionMessage_._type == ConnectionMessageType::SUCCEEDED)
            {
                onCreateAndReturnRecoveryStringCallbackFn_(std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance()),connectionMessage_._parameters[0]);  //username
            }
            else if(connectionMessage_._type == ConnectionMessageType::FAILED)
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
        std::vector<std::string> parameters;
        ConnectionMessageType connectionMessageType = ConnectionMessageType::CR_DELETE_USER;

        parameters.push_back(cookieString_);

        ConnectionMessage connectionMessage(connectionMessageType, parameters);
        _messageSocket.sendMessage(connectionMessage,
                                   std::bind(&AuthenticatorImpl::handleDeleteUserConnectionMessage,
                                             this,
                                             std::placeholders::_1,
                                             std::placeholders::_2,
                                             onDeleteUserCallbackFn_));
    }

    void AuthenticatorImpl::handleDeleteUserConnectionMessage(const std::error_code& error_,
                                                          const ConnectionMessage& connectionMessage_,
                                                          const ErrorCodeCallbackFn& onDeleteUserCallbackFn_)
    {
        if(!error_)
        {
            if (connectionMessage_._type == ConnectionMessageType::SUCCEEDED)
            {
                _clientCache.removeCookieUsernameFromCache(connectionMessage_._parameters[2],
                                                           connectionMessage_._parameters[0],
                                                           connectionMessage_._parameters[1]);
                _clientCache.removeAllCookiesForUser(connectionMessage_._parameters[0],
                                                     connectionMessage_._parameters[1]);
                onDeleteUserCallbackFn_(std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance()));
            }
            else if(connectionMessage_._type == ConnectionMessageType::FAILED)
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

}}}
