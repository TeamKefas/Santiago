#ifndef SANTIAGO_TEST_APPSERVER_ASYNCCREATEUSERHANDLER_H
#define SANTIAGO_TEST_APPSERVER_ASYNCCREATEUSERHANDLER_H

#include "RequestHandlerBase.h"

namespace Test{ namespace AppServer
{
    class AsyncCreateUserHandler:public RequestHandlerBase
    {
    public:
             
        AsyncCreateUserHandler(Santiago::Authentication::AuthenticatorBase& userController_):
            RequestHandlerBase(userController_)
       {}
        
    protected:
     
        virtual void handleVerifiedRequest(const RequestPtr& request_,
                                           boost::asio::yield_context yield_,
                                           const std::string& userName_,
                                           const std::string& emailAddress_,
                                           const std::string& cookieString_);

        virtual void handleNonVerifiedRequest(const RequestPtr& request_,
                                              boost::asio::yield_context yield_);   
    };
    
}}

#endif
