#include "RequestHandlerBase.h"

namespace SessionServer
{
    void RequestHandlerBase::handleRequest(const RequestPtr& request_)
    {

        std::map<std::string,std::string>::const_iterator iter =  request_->getHTTPCookiesReceived().find("SID");
        if(request_->getHTTPCookiesReceived().end() == iter)
        {
            handleNonVerifiedRequest(request_);
            return;
        }
     
        _userController.verifyCookieAndGetUserInfo(
            iter->second,
            std::bind(&RequestHandlerBase::handleVerifyCookieAndGetUserInfo,
                      this->sharedFromThis<RequestHandlerBase>(),
                      request_,
                      iter->second,
                      std::placeholders::_1,
                      std::placeholders::_2));

    }
    
    void RequestHandlerBase::handleVerifyCookieAndGetUserInfo(const RequestPtr& request_,
                                                              const std::string& cookieString_,
                                                              std::error_code error_,
                                                              const boost::optional<Santiago::Authentication::UserInfo>& userInfo_)
    {
        if(error_)
        {
            handleNonVerifiedRequest(request_);
        }
        else
        {
            BOOST_ASSERT(userInfo_);
            handleVerifiedRequest(request_,
                                  userInfo_->_userName,
                                  userInfo_->_emailAddress,
                                  cookieString_);
        }
    }

    void RequestHandlerBase::printEcho(const RequestPtr& request_)
    {
        const std::map<std::string,std::string>& fcgiParams = request_->getFCGIParams(); 
        
        request_->out()<<"-----------ECHO------------"<<std::endl;
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
    }
}
