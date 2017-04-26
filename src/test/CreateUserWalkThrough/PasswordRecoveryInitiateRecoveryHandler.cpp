#include "PasswordRecoveryInitiateRecoveryHandler.h"

namespace Test{ namespace AppServer
{

    void PasswordRecoveryInitiateRecoveryHandler::handleVerifiedRequest(const RequestPtr& request_,
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
 
        if(request_->getPostData().end() == emailAddressIter)
        {
            request_->out()<<"email_address not send in the post data. \n";
            request_->out()<<"in buffer: "<<request_->getStdinBuffer()<<std::endl;
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
            passwordRecoverylink<<"localhost:8080/password-recovery-receive-new-password.fcgi?recovery_string=";
            passwordRecoverylink<<recoveryString_;
            passwordRecoverylink<<"&email_address=";
            passwordRecoverylink<<emailAddress_;
            std::string subject = "Password recovery";
            
            Santiago::Utils::Email::sendEmail(emailAddress_,
                                              subject,
                                              passwordRecoverylink.str());
            
            request_->out()<<"password recovery url has been send to email \n";
            request_->setAppStatus(0);
            std::error_code error;
            request_->commit(error);

            
        }
    }



}}
