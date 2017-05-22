#ifndef SANTIAGO_TEST_APPSERVER_PASSWORDRECOVERYRECEIVENEWPASSWORDHANDLER_H
#define SANTIAGO_TEST_APPSERVER_PASSWORDRECOVERYRECEIVENEWPASSWORDHANDLER_H


#include "RequestHandlerBase.h"

namespace Test{namespace AppServer{
    class PasswordRecoveryReceiveNewPasswordHandler:public RequestHandlerBase
    {
    public:
        
        typedef RequestHandlerBase MyBase;
        typedef std::shared_ptr<PasswordRecoveryReceiveNewPasswordHandler> Ptr;
        
        PasswordRecoveryReceiveNewPasswordHandler(Santiago::Authentication::AuthenticatorBase& userController_):
            RequestHandlerBase(userController_)
        {}
        
    protected:
        
        void handleReceiveNewPassword(const RequestPtr& request_,
                                      const std::string& emailAddress_,
                                      const std::string& recoveryString_,
                                      std::error_code error_,
                                      const boost::optional<std::string>& userNameOpt_);
        
        virtual void handleVerifiedRequest(const RequestPtr& request_,
                                           const std::string& userName_,
                                           const std::string& emailAddress_,
                                           const std::string& cookieString_);
        
        virtual void handleNonVerifiedRequest(const RequestPtr& request_);
        
    };

}}

#endif
