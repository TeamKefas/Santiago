#include "PasswordRecoveryReceiveNewPasswordHandler.h"

namespace Test{ namespace AppServer
{

    void PasswordRecoveryReceiveNewPasswordHandler::handleVerifiedRequest(const RequestPtr& request_,
                                                                        const std::string& userName_,
                                                                        const std::string& emailAddress_,
                                                                        const std::string& cookieString_)
    {
        request_->setContentMIMEType(Santiago::MIMEType::TEXT);
        request_->out()<<"User already logged in."<<std::endl;
        request_->out()<<"User name: "<<userName_<<std::endl;
        request_->setAppStatus(0);
        std::error_code error;
        request_->commit(error);
    }

    void PasswordRecoveryInitiateRecoveryHandler::handleNonVerifiedRequest(const RequestPtr& request_)
    {
        request_->setContentMIMEType(Santiago::MIMEType::TEXT);
        std::map<std::string,std::string>::const_iterator emailAddressIter =  request_->getPostData().find("email_address");
        std::map<std::string,std::string>::const_iterator recoveryStringIter =  request_->getPostData().find("recovery_string");
         
 
        if(request_->getPostData().end() == emailAddressIter || request->getPostData().end() == recoveryStringIter)
        {
            request_->out()<<"email_address or recovery_string not send in the post data. \n";
            request_->setAppStatus(0);
            std::error_code error;
            request_->commit(error);        
        }
        else
        {
            _userController.createAndReturnRecoveryString(
                emailAddressIter->second,
                std::bind(&PasswordRecoveryInitiateRecoveryHandler::handleInitiatePasswordRecovery,
                          this->sharedFromThis<PasswordRecoveryInitiateRecoveryHandler>(),
                          request_,
                          emailAddressIter->second,
                          std::placeholders::_1,
                          std::placeholders::_2));
            
        }
        
    }
    
    void PasswordRecoveryInitiateRecoveryHandler::handleInitiatePasswordRecovery(const RequestPtr& request_,
                                                                                 const std::string& emailAddress_,
                                                                                 std::error_code error_,
                                                                                 const std::string& recoveryString_)
    {
        if(error_)
        {
            request_->out()<<"Password Recovery failed. \n";
            request_->out()<<error_.message()<<std::endl;
            request_->setAppStatus(0);
            std::error_code error;
            request_->commit(error);
        }
        else
        {
            std::stringstream passwordRecoverylink;
            passwordRecoverylink<<"localhost:8080/password-recovery-receive-new-password.fcgi?recovery-string=";
            passwordRecoverylink<<recoveryString_;
            passwordRecoverylink<<"&email-address=";
            passwordRecoverylink<<emailAddress_;
            std::string subject = "Password recovery";
            
            request_->out()<<"password recovery url has been send to email \n";
            request_->setAppStatus(0);
            std::error_code error;
            request_->commit(error);

            
        }
    }



}}
