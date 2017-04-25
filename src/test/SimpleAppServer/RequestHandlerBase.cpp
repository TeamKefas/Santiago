#include "RequestHandlerBase.h"

namespace SimpleAppServer
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
}
