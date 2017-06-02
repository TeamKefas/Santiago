#include "AsyncCreateUserHandler.h"

namespace Test { namespace AppServer
{
    void AsyncCreateUserHandler::handleVerifiedRequest(const RequestPtr& request_,
                                                       boost::asio::yield_context yield_,
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

    void AsyncCreateUserHandler::handleNonVerifiedRequest(const RequestPtr& request_,
                                                          boost::asio::yield_context yield_)
    {
        request_->setContentMIMEType(Santiago::MIMEType::TEXT);
        
        std::map<std::string,std::string>::const_iterator userNameIter =  
            request_->getPostData().find("user_name");

        std::map<std::string,std::string>::const_iterator passwordIter =  
            request_->getPostData().find("password");

        std::map<std::string,std::string>::const_iterator emailAddressIter =  
            request_->getPostData().find("email_address");

        //Check username is already in the database
        if(request_->getPostData().end() == userNameIter ||
           request_->getPostData().end() == passwordIter ||
           request_->getPostData().end() == emailAddressIter)
        {
            request_->out() << "Invalid arguments from browser..." << std::endl;        
            request_->setAppStatus(0);
            std::error_code error;
            request_->commit(error);        
        }
        else
        {
            std::error_code error1;
            _userController.createUser(
                userNameIter->second,
                emailAddressIter->second,
                passwordIter->second,
                yield_,
                error1);
            
            if(error1)
            {
                //assuming it is always username already exist exception...
                request_->out() <<error1.message()<<std::endl;
                printEcho(request_);
                request_->setAppStatus(0);
                std::error_code error;
                request_->commit(error);
            }
            else
            {
                request_->out() << "Signup successfull."<<std::endl;
                printEcho(request_);
                request_->setAppStatus(0);
                std::error_code error;
                request_->commit(error);
            }
            
            
        }
    }
    
    }}
