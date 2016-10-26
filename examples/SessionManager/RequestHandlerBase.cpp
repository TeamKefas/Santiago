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

        MyBase::Ptr thisBasePtr = this->shared_from_this();
        Ptr thisPtr(std::static_pointer_cast<RequestHandlerBase>(thisBasePtr));
        
        _userController.verifyCookieAndGetUserName(
            iter->second,
            std::bind(&RequestHandlerBase::handleVerifyCookieAndGetUserName,
                      thisPtr,
                      request_,
                      iter->second,
                      std::placeholders::_1,
                      std::placeholders::_2));

    }
    
    void RequestHandlerBase::handleVerifyCookieAndGetUserName(const RequestPtr& request_,
                                                              const std::string& cookieString_,
                                                              std::error_code error_,
                                                              const boost::optional<std::string>& userName_)
    {
        if(error_)
        {
            handleNonVerifiedRequest(request_);
        }
        else
        {
            BOOST_ASSERT(userName_);
            handleVerifiedRequest(request_,
                                  *userName_,
                                  cookieString_);
        }
    }
}
