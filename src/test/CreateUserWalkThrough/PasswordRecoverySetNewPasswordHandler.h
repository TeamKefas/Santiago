#ifndef SANTIAGO_TEST_APPSERVER_PASSWORDRECOVERYSETNEWPASSWORDHANDLER_H
#define SANTIAGO_TEST_APPSERVER_PASSWORDRECOVERYSETNEWPAASWORDHANDLER_H


#include "RequestHandlerBase.h"

namespace Test{namespace AppServer{
    class PasswordRecoverySetNewPasswordHandler:public RequestHandlerBase
    {
    public:
        
        typedef RequestHandlerBase MyBase;
        typedef std::shared_ptr<PasswordRecoverySetNewPasswordHandler> Ptr;
        
        PasswordRecoverySetNewPasswordHandler(Santiago::Authentication::AuthenticatorBase& userController_):
            RequestHandlerBase(userController_)
        {}
        
    protected:
        
        void handleSetNewPassword(const RequestPtr& request_,
                                  std::error_code error_);
        
        virtual void handleVerifiedRequest(const RequestPtr& request_,
                                           const std::string& userName_,
                                           const std::string& emailAddress_,
                                           const std::string& cookieString_);
        
        virtual void handleNonVerifiedRequest(const RequestPtr& request_);
        
    };

}}

#endif
