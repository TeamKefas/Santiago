#include "AsyncLoginUserHandler.h"

namespace Test{ namespace AppServer
{
    void AsyncLoginUserHandler::handleVerifiedRequest(const RequestPtr& request_,
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

    void AsyncLoginUserHandler::handleNonVerifiedRequest(const RequestPtr& request_,
                                                         boost::asio::yield_context yield_)
    {
        request_->setContentMIMEType(Santiago::MIMEType::TEXT);
        
        boost::optional<std::pair<Santiago::Authentication::UserInfo,std::string> > userInfoCookieStringPair;
        std::error_code error1;

        std::map<std::string,std::string>::const_iterator userNameIter =  request_->getPostData().find("user_name");
        std::map<std::string, std::string>::const_iterator emailIter = request_->getPostData().find("email_address");
        std::map<std::string,std::string>::const_iterator passwordIter =  request_->getPostData().find("password");
        if((request_->getPostData().end() == userNameIter && request_->getPostData().end() == emailIter)  || request_->getPostData().end() == passwordIter)
        {
            request_->out()<<"user_name/password not send in the post data."<<std::endl;
            request_->setAppStatus(0);
            std::error_code error;
            request_->commit(error);        
        }
        else if(request_->getPostData().end() != userNameIter)
        {
            userInfoCookieStringPair =
                _userController.loginUser(
                    userNameIter->second,
                    true,
                    passwordIter->second,
                    yield_,
                    error1);                
        }
        else
        {
           userInfoCookieStringPair =
               _userController.loginUser(
                   emailIter->second,
                   false,
                   passwordIter->second,
                   yield_,
                   error1);              
        }
        
        if (error1)
        {
            request_->out()<<"User authentication failed. \n";
            request_->out()<<error1.message()<<std::endl;
            request_->setAppStatus(0);
            std::error_code error;
            request_->commit(error);
        }
        else
        {
            ST_ASSERT(userInfoCookieStringPair);
            Santiago::HTTPCookieData cookieData;
            cookieData._name = "SID";
            cookieData._value = userInfoCookieStringPair->second;
//            cookieData._expiryTime = boost::posix_time::second_clock::universal_time() + boost::posix_time::seconds(240);
            bool flag = request_->responseHTTPCookies().insert(cookieData).second;
            ST_ASSERT(flag);

            request_->out()<<"User login successfull. \n";
            request_->setAppStatus(0);
            std::error_code error;
            request_->commit(error);
        }
            
        
    }

    }}
