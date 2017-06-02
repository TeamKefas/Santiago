#include "AsyncPasswordRecoveryInitiateRecoveryHandler.h"

namespace Test{ namespace AppServer
{

    void AsyncPasswordRecoveryInitiateRecoveryHandler::handleVerifiedRequest(const RequestPtr& request_,
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

    void AsyncPasswordRecoveryInitiateRecoveryHandler::handleNonVerifiedRequest(const RequestPtr& request_,
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
            std::error_code error1;
            boost::optional<std::string> recoveryStringOpt=
                _userController.createAndReturnRecoveryString(emailAddressIter->second,
                                                              yield_,
                                                              error1);
            if(error1)
            {
                request_->out()<<"Password Recovery failed. \n";
                request_->out()<<error1.message()<<std::endl;
                request_->setAppStatus(0);
                std::error_code error;
                request_->commit(error);
            }
            else
            {
                ST_ASSERT(recoveryStringOpt);
                std::stringstream passwordRecoverylink;
                passwordRecoverylink<<"localhost:8080/password-recovery-receive-new-password.fcgi?recovery_string=";
                passwordRecoverylink<<*recoveryStringOpt;
                passwordRecoverylink<<"&email_address=";
                passwordRecoverylink<<emailAddressIter->second;
                std::string subject = "Password recovery";
                
                Santiago::Utils::Email::sendEmail(emailAddressIter->second,
                                                  subject,
                                                  passwordRecoverylink.str());
                
                request_->out()<<"password recovery url has been send to email \n";
                request_->setAppStatus(0);
                std::error_code error;
                request_->commit(error);
                       
            }
              
        }
    }
    
}}
