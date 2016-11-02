#include "SimplePageHandler.h"

namespace SimpleAppServer
{

    void SimplePageHandler::handleVerifiedRequest(const RequestPtr& request_,
                                                  const std::string& userName_,
                                                  const std::string& emailAddress_,
                                                  const std::string& cookieString_)
    {
        if(_documentURI == "/")
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
                            << " + '=; Path=/; Expires=Thu, 01 Jan 1970 00:00:01 GMT;'"
                            << "};"
                            << "</script></html>";
        }
        else if(_documentURI == "signup.html")
        {
            request_->out() << "Page not found... " << std::endl;        
        }
        request_->setAppStatus(0);
        std::error_code error;
        request_->commit(error);
    }

    void SimplePageHandler::handleNonVerifiedRequest(const RequestPtr& request_)
    {
        request_->setContentMIMEType(Santiago::MIMEType::HTML);
        //request_->out() << "You are not logged in... " << std::endl;        
        
        if(_documentURI == "/") 
        {
            request_->out() << "<html>"
                            << "<body>"
                            << "<form action='http://localhost:8080/login.fcgi' method='post'>"
                            << "Username: <input type='text' name='user_name'><br>"
                            << "Password: <input type='password' name='password'><br>"
                            << "<input type='submit' value='Login'>"
                            << "</form>"
                            << "<a href='signup.html'>Signup</a>"
                            << "</body>"
                            << "</html>";
        }
        else if(_documentURI == "signup.html")
        {
            request_->out() << "<html>"
                            << "<body>"
                            << "<form action='http://localhost:8080/signup.fcgi' method='post'>"
                            << "Username: <input type='text' name='user_name'><br>"
                            << "Password: <input type='password' name='password'><br>"
                            << "<input type='submit' value='Signup'>"
                            << "</form>"
                            << "</body>"
                            << "</html>";
        }
        request_->setAppStatus(0);

        std::error_code error;
        request_->commit(error);
    }



}
