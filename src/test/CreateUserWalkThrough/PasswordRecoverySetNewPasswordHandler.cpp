#include "PasswordRecoverySetNewPasswordHandler.h"

namespace Test{ namespace AppServer
{

    void PasswordRecoverySetNewPasswordHandler::handleVerifiedRequest(const RequestPtr& request_,
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

    void PasswordRecoverySetNewPasswordHandler::handleNonVerifiedRequest(const RequestPtr& request_,
                                                                         boost::asio::yield_context yield_)
    {
        request_->setContentMIMEType(Santiago::MIMEType::TEXT);
        std::map<std::string,std::string>::const_iterator emailAddressIter =  request_->getGetData().find("email_address");
        std::map<std::string,std::string>::const_iterator recoveryStringIter =  request_->getGetData().find("recovery_string");

        std::map<std::string,std::string>::const_iterator passwordIter =  request_->getGetData().find("password");
        std::map<std::string,std::string>::const_iterator repeatPasswordIter =  request_->getGetData().find("repeat_password");
       
       
            
 
        if(request_->getGetData().end() == emailAddressIter || request_->getGetData().end() == recoveryStringIter || request_->getGetData().end()== passwordIter || request_->getGetData().end() == repeatPasswordIter)
        {
            request_->out()<<"email_address or recovery_string not send in the post data. \n";
            request_->setAppStatus(0);
            std::error_code error;
            request_->commit(error);        
        }
        else
        {
            if(passwordIter->second != repeatPasswordIter->second)
            {
                request_->out()<<"password does not match. \n";
                request_->setAppStatus(0);
                std::error_code error;
                request_->commit(error);
            }
            else
            {
                _userController.changeUserPasswordForEmailAddressAndRecoveryString(
                    emailAddressIter->second,
                    recoveryStringIter->second,
                    passwordIter->second,
                    std::bind(&PasswordRecoverySetNewPasswordHandler::handleSetNewPassword,
                              this->sharedFromThis<PasswordRecoverySetNewPasswordHandler>(),
                              request_,
                              std::placeholders::_1));
                
            }
        }
        
    }
    void PasswordRecoverySetNewPasswordHandler::handleSetNewPassword(const RequestPtr& request_,
                                                                  std::error_code error_)
    {
        if(error_)
        {
            request_->out()<<"Password Recovery Set new password failed. \n";
            request_->out()<<error_.message()<<std::endl;
            request_->setAppStatus(0);
            std::error_code error;
            request_->commit(error);
        }
        else
        { 
            
            request_->out()<<"Successfully updated new password \n";
            request_->setAppStatus(0);
            std::error_code error;
            request_->commit(error);

            
        }
    }



}}
