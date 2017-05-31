#include "PasswordRecoveryInitiateRecoveryHandler.h"

namespace Test{ namespace AppServer
{

    void PasswordRecoveryInitiateRecoveryHandler::handleVerifiedRequest(const RequestPtr& request_,
                                                                        boost::asio::yield_context yield_,
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

    void PasswordRecoveryInitiateRecoveryHandler::handleNonVerifiedRequest(const RequestPtr& request_,
                                                                           boost::asio::yield_context yield_)
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
                                                                                 const boost::optional<std::string>& recoveryStringOpt_)
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
            ST_ASSERT(recoveryStringOpt_);
            std::string sendingAddress = _config.get<std::string>("Santiago.Email.sending_address");
            std::string senderName = _config.get<std::string>("Santiago.Email.sender_name");
            std::string password = _config.get<std::string>("Santiago.Email.password");
            std::string serverAddress = _config.get<std::string>("Santiago.Email.server_address");

            Santiago::Utils::Email::initEmail(sendingAddress, senderName, password, serverAddress);
            
            std::stringstream passwordRecoverylink;
            passwordRecoverylink<<"http://localhost:8080/password-recovery-receive-new-password.fcgi?recovery_string=";
            passwordRecoverylink<<*recoveryStringOpt_;
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
