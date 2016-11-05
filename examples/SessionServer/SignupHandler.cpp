#include "SignupHandler.h"

namespace SessionServer
{
    void SignupHandler::handleVerifiedRequest(const RequestPtr& request_,
                                              const std::string& userName_,
                                              const std::string& emailAddress_,
                                              const std::string& cookieString_)
    {
        request_->setContentMIMEType(Santiago::MIMEType::TEXT);
        request_->out()<<"User already logged in. Please logout(logout.fcgi) before trying again."<<std::endl;
        request_->out()<<"User name: "<<userName_<<std::endl;
        
        printEcho(request_);
        request_->setAppStatus(0);
        std::error_code error;
        request_->commit(error);
    }

    void SignupHandler::handleNonVerifiedRequest(const RequestPtr& request_)
    {
        request_->setContentMIMEType(Santiago::MIMEType::TEXT);
        
        std::map<std::string,std::string>::const_iterator userNameIter =  
            request_->getPostData().find("user_name");

        std::map<std::string,std::string>::const_iterator passwordIter =  
            request_->getPostData().find("password");

        //Check username is already in the database
        if(request_->getPostData().end() == userNameIter ||
           request_->getPostData().end() == passwordIter)
        {
            request_->out() << "This username is not available..." << std::endl;        
            request_->setAppStatus(0);
            std::error_code error;
            request_->commit(error);        
        }
        else
        {
            _userController.createUser(
                userNameIter->second,
                "example@example.com",
                passwordIter->second,
                std::bind(&SignupHandler::handleSignupUser,
                          this->sharedFromThis<SignupHandler>(),
                          request_,
                          userNameIter->second,
                          "example@example.com",
                          std::placeholders::_1));
        }
    }


    void SignupHandler::handleSignupUser(const RequestPtr& request_,
                                         const std::string& userName_,
                                         const std::string& emailAddress_,
                                         std::error_code error_)
                                             
    {
        if(error_)
        {
            //assuming it is always username already exist exception...
            request_->out() <<error_.message()<<std::endl;
            printEcho(request_);
            request_->setAppStatus(0);
            std::error_code error;
            request_->commit(error);
        }
        else
        {
            request_->out() << "Signup successfull.";
            printEcho(request_);
            request_->setAppStatus(0);
            std::error_code error;
            request_->commit(error);
        }
    }
}
