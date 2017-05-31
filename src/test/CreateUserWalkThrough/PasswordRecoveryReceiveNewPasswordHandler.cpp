#include "PasswordRecoveryReceiveNewPasswordHandler.h"

namespace Test{ namespace AppServer
{

    void PasswordRecoveryReceiveNewPasswordHandler::handleVerifiedRequest(const RequestPtr& request_,
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

    void PasswordRecoveryReceiveNewPasswordHandler::handleNonVerifiedRequest(const RequestPtr& request_,
                                                                             boost::asio::yield_context yield_)
    {
        request_->setContentMIMEType(Santiago::MIMEType::TEXT);
        
         std::map<std::string,std::string>::const_iterator recoveryStringIter =  request_->getGetData().find("recovery_string");
        std::map<std::string,std::string>::const_iterator emailAddressIter =  request_->getGetData().find("email_address");
       
 
        if(request_->getPostData().end() == emailAddressIter || request_->getPostData().end() == recoveryStringIter)
        {
            request_->out()<<"email_address or recovery_string not send in the post data. \n";
            request_->setAppStatus(0);
            std::error_code error;
            request_->commit(error);        
        }
        else
        {
            _userController.getUserForEmailAddressAndRecoveryString(
                emailAddressIter->second,
                recoveryStringIter->second,
                std::bind(&PasswordRecoveryReceiveNewPasswordHandler::handleReceiveNewPassword,
                          this->sharedFromThis<PasswordRecoveryReceiveNewPasswordHandler>(),
                          request_,
                          emailAddressIter->second,
                          recoveryStringIter->second,
                          std::placeholders::_1,
                          std::placeholders::_2));
            
        }
        
    }
    
    void PasswordRecoveryReceiveNewPasswordHandler::handleReceiveNewPassword(const RequestPtr& request_,
                                                                             const std::string& emailAddress_,
                                                                             const std::string& recoveryString_,
                                                                             std::error_code error_,
                                                                             const boost::optional<std::string>& userNameOpt_)
    {
        if(error_)
        {
            request_->out()<<"Password Recovery Receive new password failed. \n";
            request_->out()<<error_.message()<<std::endl;
            request_->setAppStatus(0);
            std::error_code error;
            request_->commit(error);
        }
        else
        {
            ST_ASSERT(userNameOpt_);
            request_->setContentMIMEType(Santiago::MIMEType::HTML);
            request_->out()<<"<html> \n"
                           <<"<body> \n"
                           <<"<form action='http://localhost:8080/password-recovery-set-new-password.fcgi' method='get'>\n"
                           <<"hai"<<userNameOpt_<<"\n"
                           <<"<br />"
                           <<"New Password: <input type='password' name='password'><br />\n"
                           <<"Repeat Password: <input type='password' name='repeat_password' />\n"
                           <<"<input type = 'hidden' name = 'email_address' value ='"<<emailAddress_<<"' />\n"
                           <<"<input type = 'hidden' name = 'recovery_string' value ='"<<recoveryString_<<"' />\n"
                           <<"<br />"
                           <<"<input type= 'submit' value= 'Submit' />\n"
                           <<"</form>\n"
                           <<"</body>\n"
                           <<"</html>";
               
            request_->setAppStatus(0);
            std::error_code error;
            request_->commit(error);

            
        }
    }
}}
