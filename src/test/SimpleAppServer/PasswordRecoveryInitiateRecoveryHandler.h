#ifndef SANTIAGO_TEST_SIMPLEAPPSERVER_PASSWORDRECOVERYINITIATERECOVERYHANDLER_H
#define SANTIAGO_TEST_SIMPLEAPPSERVER_PASSWORDRECOVERYINITIATERECOVERYHANDLER_H


#include "RequestHandlerBase.h"
#include "Santiago/utils/Email.h"

namespace SimpleAppServer{
    class PasswordRecoveryInitiateRecoveryHandler:public RequestHandlerBase
    {
    public:
        
        typedef RequestHandlerBase MyBase;
        typedef std::shared_ptr<PasswordRecoveryInitiateRecoveryHandler> Ptr;
        
        PasswordRecoveryInitiateRecoveryHandler(Santiago::User::ControllerBase& userController_):
            RequestHandlerBase(userController_)
        {}
        
    protected:
        
        void handleInitiatePasswordRecovery(const RequestPtr& request_,
                                            const std::string& userName_,
                                            std::error_code error_,
                                            const boost::optional<std::d::pair<Santiago::User::UserInfo,std::string> >& userInfoCookieStringPair_);
        
        virtual void handleVerifiedRequest(const RequestPtr& request_,
                                           const std::string& userName_,
                                           const std::string& emailAddress_,
                                           const std::string& cookieString_);
        
        virtual void handleNonVerifiedRequest(const RequestPtr& request_);
        
    };
    
}

#endif
