#ifndef SLABS_APPSERVER_ECHOHANDLER_H
#define SLABS_APPSERVER_ECHOHANDLER_H

#include "RequestHandlerBase.h"

namespace Test{ namespace AppServer
{
    class CreateUserHandler:public RequestHandlerBase
    {
    public:
             
        CreateUserHandler(Santiago::User::ControllerBase& userController_):
            RequestHandlerBase(userController_)
       {}
        
    protected:
        
       void handleSignupUser(const RequestPtr& request_,
                             const std::string& userName_,
                             const std::string& emailAddress_,
                             std::error_code error_);

       virtual void handleVerifiedRequest(const RequestPtr& request_,
                                          const std::string& userName_,
                                          const std::string& emailAddress_,
                                          const std::string& cookieString_);

       virtual void handleNonVerifiedRequest(const RequestPtr& request_);
       
        
    };
    
}}

#endif
