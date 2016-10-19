#include "LoginPageHandler.h"

namespace SimpleAppServer
{

    void LoginPageHandler::handleVerifiedRequest(const RequestPtr& request_,
                                                  const std::string& userName_,
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

    void SimplePageHandler::handleNonVerifiedRequest(const RequestPtr& request_)
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

        std::map<std::string,std::string>::const_iterator userNameIter =  request_.getPostData().find("user_name");
        std::map<std::string,std::string>::const_iterator passwordIter =  request_.getPostData().find("password");
        if(request_.getPostData().end() == userNameIter || request_.getPostData().end() == passwordIter)
        {
            request_->out()<<"user_name/password not send in the post data. \n";
            request_->setAppStatus(0);
            std::error_code error;
            request_->commit(error);        
        }
        else
        {
            MyBase::Ptr thisBasePtr = this->shared_from_this();
            Ptr thisPtr(static_pointer_cast<RequestHandlerBase>(thisBasePtr));
            
            _userController.loginUser(
                userNameIter->second,
                passwordIter->second,
                std::bind(&RequestHandlerBase::handleLoginUser,
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
                                           const std::string& cookieString_)
    {
        if(error_)
        {
            request_->out()<<"User authentication failed. \n";
            request_->out()<<error_.description()<<std::endl;
            request_->setAppStatus(0);
            std::error_code error;
            request_->commit(error);
        }
        else
        {
            Santiago::HTTPCookieData cookieData;
            cookieData._name = "SID";
            cookieData._value = cookieString_;
            cookieData._expiryTime = boost::posix_time::second_clock::local_time() + boost::posix_time::seconds(900);
            bool flag = request_->responseHTTPCookies().insert(cookieData).second;

            request_->out()<<"User login successfull. \n";
            request_->setAppStatus(0);
            std::error_code error;
            request_->commit(error);
        }
    }



}
