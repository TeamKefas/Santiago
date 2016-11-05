#include "LoginHandler.h"

namespace SessionServer
{
    void LoginHandler::handleVerifiedRequest(const RequestPtr& request_,
                                             const std::string& userName_,
                                             const std::string& emailAddress_,
                                             const std::string& cookieString_)
    {
        request_->setContentMIMEType(Santiago::MIMEType::TEXT);
        request_->out()<<"User already logged in. Please logout(logout.fcgi) before trying again."<<std::endl;
        request_->out()<<"User name: "<<userName_<<std::endl;
        
        printEcho();
        request_->setAppStatus(0);
        std::error_code error;
        request_->commit(error);
    }

    void LoginHandler::handleNonVerifiedRequest(const RequestPtr& request_)
    {
        request_->setContentMIMEType(Santiago::MIMEType::TEXT);
        
//    std::cout<<request_->getFCGIParams()[REQUEST_URI];

        std::map<std::string,std::string>::const_iterator userNameIter =  request_->getPostData().find("user_name");
        std::map<std::string,std::string>::const_iterator passwordIter =  request_->getPostData().find("password");
        if(request_->getPostData().end() == userNameIter || request_->getPostData().end() == passwordIter)
        {
            request_->out()<<"user_name/password not send in the post data."<<std::endl;

            printEcho();
            request_->setAppStatus(0);
            std::error_code error;
            request_->commit(error);        
        }
        else
        {
            _userController.loginUser(
                userNameIter->second,
                true,
                passwordIter->second,
                std::bind(&LoginHandler::handleLoginUser,
                          this->sharedFromThis<LoginHandler>(),
                          request_,
                          userNameIter->second,
                          std::placeholders::_1,
                          std::placeholders::_2));
            
        }
        
    }

    void LoginHandler::handleLoginUser(const RequestPtr& request_,
                                       const std::string& userName_,
                                       std::error_code error_,
                                       const boost::optional<std::pair<Santiago::User::UserInfo,std::string> >& userInfoCookieStringPair_)
    {
        if(error_)
        {
            request_->out()<<"User authentication failed. \n";
            request_->out()<<error_.message()<<std::endl;
            printEcho();
            request_->setAppStatus(0);
            std::error_code error;
            request_->commit(error);
        }
        else
        {
            BOOST_ASSERT(userInfoCookieStringPair_);
            Santiago::HTTPCookieData cookieData;
            cookieData._name = "SID";
            cookieData._value = userInfoCookieStringPair_->second;
//            cookieData._expiryTime = boost::posix_time::second_clock::universal_time() + boost::posix_time::seconds(240);
            bool flag = request_->responseHTTPCookies().insert(cookieData).second;
            BOOST_ASSERT(flag);

            request_->out()<<"User login successfull. \n";
            printEcho();
            request_->setAppStatus(0);
            std::error_code error;
            request_->commit(error);
        }
    }
}
