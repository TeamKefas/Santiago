#ifndef SANTIAGO_TEST_SIMPLEAPPSERVER_LOGINPAGEHANDLER_H
#define SANTIAGO_TEST_SIMPLEAPPSERVER_LOGINPAGEHANDLER_H

#include <memory>

#include "RequestHandlerBase.h"

namespace SimpleAppServer
{
    class LoginPageHandler:public RequestHandlerBase
    {
    public:
        
        typedef RequestHandlerBase MyBase;
        typedef std::shared_ptr<LoginPageHandler> Ptr;

        LoginPageHandler(Santiago::User::ControllerBase& userController_):
            RequestHandlerBase(userController_)
        {}
        
    protected:
        
        void handleLoginUser(const RequestPtr& request_,
                             const std::string& userName_,
                             std::error_code error_,
                             const boost::optional<std::pair<Santiago::User::UserInfo,std::string> >& userInfoCookieStringPair_);
        
        virtual void handleVerifiedRequest(const RequestPtr& request_,
                                           const std::string& userName_,
                                           const std::string& emailAddress_,
                                           const std::string& cookieString_);
        
        virtual void handleNonVerifiedRequest(const RequestPtr& request_);
        
   };

}

#endif
