#ifndef SANTIAGO_TEST_APPSERVER_ASYNCPASSWORDRECOVERYSETNEWPASSWORDHANDLER_H
#define SANTIAGO_TEST_APPSERVER_ASYNCPASSWORDRECOVERYSETNEWPAASWORDHANDLER_H


#include "RequestHandlerBase.h"

namespace Test{namespace AppServer{
    class AsyncPasswordRecoverySetNewPasswordHandler:public RequestHandlerBase
    {
    public:
        
        typedef RequestHandlerBase MyBase;
        typedef std::shared_ptr<AsyncPasswordRecoverySetNewPasswordHandler> Ptr;
        
        AsyncPasswordRecoverySetNewPasswordHandler(Santiago::Authentication::AuthenticatorBase& userController_):
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
