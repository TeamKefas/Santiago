#include "Authenticator.h"

namespace Santiago{ namespace Authentication{ namespace MultiNode
{
    
    void Authenticator::createUserImpl(const std::string& userName_,
                                       const std::string& emailAddress_,
                                       const std::string& password_,
                                       const ErrorCodeCallbackFn& onCreateUserCallbackFn_)
    {
        std::vector<std::string> parameters;
        ConnectionMessageType connectionMessageType = CR_CREATE_USER;
        parameters.push_back(userName_);
        parameters.push_back(emailAddress_);
        parameters.push_back(generateSHA256(password_));

        ConnectionMessage connectionMessage(connectionMessageType, parameters);
        _messageSocket.sendMessage(connectionMessage,
                                   std::bind(&Authenticator::handleCreateUserConnectionMessage,
                                             this,
                                             std::placeholders::_1,
                                             std::placeholders::_2,
                                             onCreateUserCallbackFn_));
    }

    void Authenticator::handleCreateUserConnectionMessage(const std::error_code& error,
                                                          const ConnectionMessage& ConnectionMessage,
                                                          const onCreateUserCallbackFn& onCreateUserCallbackFn_)
    {
        
    }
    
    std::string Authenticator::generateSHA256(const std::string str)
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
