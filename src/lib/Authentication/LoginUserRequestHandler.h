#ifndef SANTIAGO_AUTHENTICATION_LOGINUSERREQUESTHANDLER_H
#define SANTIAGO_AUTHENTICATION_LOGINUSERREQUESTHANDLER_H

#include "RequestHandlerBase.h"

namespace Santiago{ namespace Authentication
{
    class LoginUserRequestHandler:public RequestHandlerBase
    {
    public:
        LoginUserRequestHandler(ConnectionServer& connectionServer_
                                ,const OnCompletedCallbackFn& onCompletedCallbackFn_
                                ,const ServerMessage& initiatingMessage_);
        virtual void start();
        virtual void handleReplyMessage(const ServerMessage& serverMessage);
    };

}}

#endif
