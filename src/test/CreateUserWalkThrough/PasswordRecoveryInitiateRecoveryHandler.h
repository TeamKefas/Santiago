#ifndef SANTIAGO_TEST_APPSERVER_PASSWORDRECOVERYINITIATERECOVERYHANDLER_H
#define SANTIAGO_TEST_APPSERVER_PASSWORDRECOVERYINITIATERECOVERYHANDLER_H


#include "RequestHandlerBase.h"
#include "../../Santiago/Utils/Email.h"

namespace Test{namespace AppServer{
    class PasswordRecoveryInitiateRecoveryHandler:public RequestHandlerBase
    {
    public:
        
        typedef RequestHandlerBase MyBase;
        typedef std::shared_ptr<PasswordRecoveryInitiateRecoveryHandler> Ptr;
        
        PasswordRecoveryInitiateRecoveryHandler(Santiago::Authentication::AuthenticatorBase& userController_):
            RequestHandlerBase(userController_)
        {}
        
    protected:
        
        void handleInitiatePasswordRecovery(const RequestPtr& request_,
                                            const std::string& emailAddress_,
                                            std::error_code error_,
                                            const std::string& recoveryString_);
        
        virtual void handleVerifiedRequest(const RequestPtr& request_,
                                           const std::string& userName_,
                                           const std::string& emailAddress_,
                                           const std::string& cookieString_);
        
        virtual void handleNonVerifiedRequest(const RequestPtr& request_);
        
    };

}}

#endif
