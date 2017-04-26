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

        request_->out()<<"Echo - Tcp Socket Version\n"<<"FCGI Params:\n";
        
        const std::map<std::string,std::string>& fcgiParams = request_->getFCGIParams(); 
        
        for(std::map<std::string,std::string>::const_iterator iter = fcgiParams.begin();iter != fcgiParams.end();iter++)
        {
            request_->out()<<iter->first<<" \t"<<iter->second<<"\n";
        }
        
        request_->out()<<"Cookies:\n";
        
        const std::map<std::string,std::string>& cookies = request_->getHTTPCookiesReceived(); 
        
        for(std::map<std::string,std::string>::const_iterator iter = cookies.begin();iter != cookies.end();iter++)
        {
            request_->out()<<iter->first<<" \t"<<iter->second<<"\n";
        }
        
        request_->out()<<"Get query data:\n";
        
        const std::map<std::string,std::string>& getData = request_->getGetData(); 
        
        for(std::map<std::string,std::string>::const_iterator iter = getData.begin();iter != getData.end();iter++)
        {
            request_->out()<<iter->first<<" \t"<<iter->second<<"\n";
        }
        
        request_->out()<<"Post data:\n";
        
        const std::map<std::string,std::string>& postData = request_->getPostData(); 
        
        for(std::map<std::string,std::string>::const_iterator iter = postData.begin();iter != postData.end();iter++)
        {
            request_->out()<<iter->first<<" \t"<<iter->second<<"\n";
        }
        
//    std::cout<<request_->getFCGIParams()[REQUEST_URI];
        
        request_->setAppStatus(0);
        std::error_code error;
        request_->commit(error);
    }

    void PasswordRecoveryInitiateRecoveryHandler::handleNonVerifiedRequest(const RequestPtr& request_)
    {
        request_->setContentMIMEType(Santiago::MIMEType::TEXT);
        
        request_->out()<<"Echo - Tcp Socket Version\n"<<"FCGI Params:\n";
        
        const std::map<std::string,std::string>& fcgiParams = request_->getFCGIParams(); 
        
        for(std::map<std::string,std::string>::const_iterator iter = fcgiParams.begin();iter != fcgiParams.end();iter++)
        {
            request_->out()<<iter->first<<" \t"<<iter->second<<"\n";
        }
        
        request_->out()<<"Cookies:\n";
        
        const std::map<std::string,std::string>& cookies = request_->getHTTPCookiesReceived(); 
        
        for(std::map<std::string,std::string>::const_iterator iter = cookies.begin();iter != cookies.end();iter++)
        {
            request_->out()<<iter->first<<" \t"<<iter->second<<"\n";
        }
        
        request_->out()<<"Get query data:\n";
        
        const std::map<std::string,std::string>& getData = request_->getGetData(); 
        
        for(std::map<std::string,std::string>::const_iterator iter = getData.begin();iter != getData.end();iter++)
        {
            request_->out()<<iter->first<<" \t"<<iter->second<<"\n";
        }
        
        request_->out()<<"Post data:\n";
        
        const std::map<std::string,std::string>& postData = request_->getPostData(); 
        
        for(std::map<std::string,std::string>::const_iterator iter = postData.begin();iter != postData.end();iter++)
        {
            request_->out()<<iter->first<<" \t"<<iter->second<<"\n";
        }
        
//    std::cout<<request_->getFCGIParams()[REQUEST_URI];

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
