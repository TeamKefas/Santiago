#include "LoginPageHandler.h"

namespace SimpleAppServer
{

    void LoginPageHandler::handleVerifiedRequest(const RequestPtr& request_,
                                                 const std::string& userName_,
                                                 const std::string& cookieString_)
    {
        request_->setContentMIMEType(Santiago::MIMEType::HTML);
        request_->out() << "Page not found..." << std::endl;      
        
        request_->setAppStatus(0);
        std::error_code error;
        request_->commit(error);
    }

    void LoginPageHandler::handleNonVerifiedRequest(const RequestPtr& request_)
    {
        request_->setContentMIMEType(Santiago::MIMEType::HTML);
        
        std::map<std::string,std::string>::const_iterator userNameIter =  
            request_->getPostData().find("user_name");
        std::map<std::string,std::string>::const_iterator passwordIter =  
            request_->getPostData().find("password");
        if(request_->getPostData().end() == userNameIter || 
           request_->getPostData().end() == passwordIter)
        {
            request_->out() << "Page not found... " << std::endl;        
            request_->setAppStatus(0);
            std::error_code error;
            request_->commit(error);        
        }
        else
        {
            MyBase::MyBase::Ptr thisBasePtr = this->shared_from_this();
            Ptr thisPtr(std::static_pointer_cast<LoginPageHandler>(thisBasePtr));
            
            _userController.loginUser(
                userNameIter->second,
                passwordIter->second,
                std::bind(&LoginPageHandler::handleLoginUser,
                          thisPtr,
                          request_,
                          userNameIter->second,
                          std::placeholders::_1,
                          std::placeholders::_2));
            
        }
        
    }

    void LoginPageHandler::handleLoginUser(const RequestPtr& request_,
                                           const std::string& userName_,
                                           std::error_code error_,
                                           const boost::optional<std::string>& cookieString_)
    {
        request_->setContentMIMEType(Santiago::MIMEType::HTML);
        if(error_)
        {
            request_->out() << "<html><body>"
                            << "Invalid username or password.." 
                            << "<a href='/'>Try again..</a>"
                            << "</body></html>";
            //<< error_.message()<<std::endl;
            request_->setAppStatus(0);
            std::error_code error;
            request_->commit(error);
        }
        else
        {
            Santiago::HTTPCookieData cookieData;
            cookieData._name = "SID";
            cookieData._value = *cookieString_;
            cookieData._expiryTime = (boost::posix_time::second_clock::universal_time() + 
                                      boost::posix_time::seconds(240));
            bool flag = request_->responseHTTPCookies().insert(cookieData).second;
            BOOST_ASSERT(flag);

            request_->out() << "Hi " << userName_ << ". " 
                            << "<html> <body>" 
                            << "<a href='javascript:void(0)' onclick=deleteCookie('SID')>"
                            << "Logout</a>."
                            << "</body>"
                            << "<script type='text/javascript'>"
                            << "function deleteCookie(name) {"
                            << "document.cookie = name"
                            << "+ '=; Path=/; Expires=Thu, 01 Jan 1970 00:00:01 GMT;';"
                            << "window.location.href='/';"
                            << "};"
                            << "</script></html>";
            request_->setAppStatus(0);
            std::error_code error;
            request_->commit(error);
        }
    }



}
