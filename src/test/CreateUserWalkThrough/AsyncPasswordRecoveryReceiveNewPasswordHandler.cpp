#include "AsyncPasswordRecoveryReceiveNewPasswordHandler.h"

namespace Test{ namespace AppServer
{

    void AsyncPasswordRecoveryReceiveNewPasswordHandler::handleVerifiedRequest(const RequestPtr& request_,
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

    void AsyncPasswordRecoveryReceiveNewPasswordHandler::handleNonVerifiedRequest(const RequestPtr& request_,
                                                                                  boost::asio::yield_context yield_)
    {
        request_->setContentMIMEType(Santiago::MIMEType::TEXT);
        std::map<std::string,std::string>::const_iterator emailAddressIter =  request_->getPostData().find("email_address");
        std::map<std::string,std::string>::const_iterator recoveryStringIter =  request_->getPostData().find("recovery_string");
         
 
        if(request_->getPostData().end() == emailAddressIter || request_->getPostData().end() == recoveryStringIter)
        {
            request_->out()<<"email_address or recovery_string not send in the post data. \n";
            request_->setAppStatus(0);
            std::error_code error;
            request_->commit(error);        
        }
        else
        {
            std::error_code error1;
            boost::optional<std::string> userNameOpt =
                _userController.getUserForEmailAddressAndRecoveryString(
                    emailAddressIter->second,
                    recoveryStringIter->second,
                    yield_,
                    error1);
            if(error1)
            {
                request_->out()<<"Password Recovery Receive new password failed. \n";
                request_->out()<<error1.message()<<std::endl;
                request_->setAppStatus(0);
                std::error_code error;
                request_->commit(error);
            }
            else
            {
                ST_ASSERT(userNameOpt);
                request_->out()<<"<html> \n"
                               <<"<body> \n"
                               << "<form action='http://localhost:8080/password-recovey-set-new-password.fcgi' method='post'>\n"
                               << "hai"<<*userNameOpt<<"\n"
                               << "New Password: <input type='password' name='password'><br />\n"
                               <<"Repeat Password: <input type='password' name='repeat_password' />\n"
                               <<"<input type = 'hidden' name = 'email_address' value ="<<emailAddressIter->second<<"\n"
                               <<"<input type = 'hidden' name = 'recovery_string' value ="<<recoveryStringIter->second<<"\n" 
                               <<"<input type='submit' value='Submit' />\n"
                               <<"</form>\n"
                               <<"</body>\n"
                               <<"</html>";
                 
                request_->setAppStatus(0);
                std::error_code error;
                request_->commit(error);            
             }
        }
    }
    
}}
