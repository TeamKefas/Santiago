#include "SignupPageHandler.h"

namespace SimpleAppServer
{
    void SignupPageHandler::handleVerifiedRequest(const RequestPtr& request_,
                                                  const std::string& userName_,
                                                  const std::string& emailAddress_,
                                                  const std::string& cookieString_)
    {
        request_->setContentMIMEType(Santiago::MIMEType::HTML);
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

    void SignupPageHandler::handleNonVerifiedRequest(const RequestPtr& request_)
    {
        request_->setContentMIMEType(Santiago::MIMEType::HTML);
        
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
                std::bind(&SignupPageHandler::handleSignupUser,
                          this->sharedFromThis<SignupPageHandler>(),
                          request_,
                          userNameIter->second,
                          "example@example.com",
                          std::placeholders::_1));
        }
    }


    void SignupPageHandler::handleSignupUser(const RequestPtr& request_,
                                             const std::string& userName_,
                                             const std::string& emailAddress_,
                                             std::error_code error_)
                                             
    {
        if(error_)
        {
            //assuming it is always username already exist exception...
            request_->out() << "<html><body>"
                            << "Username already exists.." 
                            << "<a href='/signup.html'>Try another</a>"
                            << "</body></html>";
            //<< error_.message()<<std::endl;
            request_->setAppStatus(0);
            std::error_code error;
            request_->commit(error);
        }
        else
        {
            /* 
             * Santiago::HTTPCookieData cookieData;
             * cookieData._name = "SID";
             * cookieData._value = *cookieString_;
             * cookieData._expiryTime = (boost::posix_time::second_clock::universal_time() + 
             *                           boost::posix_time::seconds(240));
             * bool flag = request_->responseHTTPCookies().insert(cookieData).second;
             * BOOST_ASSERT(flag);
             */

            request_->out() << "<html><body>"
                            << "<a href='/'>Login</a>"
                            << "</body></html>";
            request_->setAppStatus(0);
            std::error_code error;
            request_->commit(error);
        }
    }
}
