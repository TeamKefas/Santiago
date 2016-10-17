#ifndef SANTIAGO_USER_SERVER_LOGINUSERREQUESTHANDLER_H
#define SANTIAGO_USER_SERVER_LOGINUSERREQUESTHANDLER_H

#include "RequestHandlerBase.h"

namespace Santiago{ namespace User { namespace Server
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

}}}

#endif
