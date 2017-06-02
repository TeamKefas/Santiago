#include "AsyncPasswordRecoverySetNewPasswordHandler.h"

namespace Test{ namespace AppServer
{

    void AsyncPasswordRecoverySetNewPasswordHandler::handleVerifiedRequest(const RequestPtr& request_,
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

    void AsyncPasswordRecoverySetNewPasswordHandler::handleNonVerifiedRequest(const RequestPtr& request_,
                                                                              boost::asio::yield_context yield_)
    {
        request_->setContentMIMEType(Santiago::MIMEType::TEXT);
        std::map<std::string,std::string>::const_iterator emailAddressIter =  request_->getPostData().find("email_address");
        std::map<std::string,std::string>::const_iterator recoveryStringIter =  request_->getPostData().find("recovery_string");

        std::map<std::string,std::string>::const_iterator passwordIter =  request_->getPostData().find("password");
        std::map<std::string,std::string>::const_iterator repeatPasswordIter =  request_->getPostData().find("repeat_password");
       
       
            
 
        if(request_->getPostData().end() == emailAddressIter || request_->getPostData().end() == recoveryStringIter || request_->getPostData().end()== passwordIter || request_->getPostData().end() == repeatPasswordIter)
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
                std::error_code error1;
               _userController.changeUserPasswordForEmailAddressAndRecoveryString(
                   emailAddressIter->second,
                   recoveryStringIter->second,
                   passwordIter->second,
                   yield_,
                   error1);
               
               if(error1)
               {
                   request_->out()<<"Password Recovery Set new password failed. \n";
                   request_->out()<<error1.message()<<std::endl;
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
        }   
    }
   
}}
