#include "SimplePageHandler.h"

namespace SimpleAppServer
{

    void SimplePageHandler::handleVerifiedRequest(const RequestPtr& request_,
                                                  const std::string& userName_,
                                                  const std::string& cookieString_)
    {
        request_->setContentMIMEType(Santiago::MIMEType::TEXT);

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

        request_->out()<<"User name: NOT LOGGED IN"<<std::endl;
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



}
