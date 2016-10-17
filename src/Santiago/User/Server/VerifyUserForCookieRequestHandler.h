#ifndef SANTIAGO_USER_SERVER_VERIFYUSERFORCOOKIEREQUESTHANDLER_H
#define SANTIAGO_USER_SERVER_VERIFYUSERFORCOOKIEREQUESTHANDLER_H

#include "RequestHandlerBase.h"


namespace Santiago{ namespace User { namespace Server
{
    class VerifyUserForCookieRequestHandler:public RequestHandlerBase
    {
    public:
        VerifyUserForCookieRequestHandler(ConnectionServer& connectionServer_
                                          ,const OnCompletedCallbackFn& onCompletedCallbackFn_
                                          ,const ServerMessage& initiatingMessage_);
        virtual void start();
        virtual void handleReplyMessage(const ServerMessage& serverMessage);
        
    };

}}}

#endif
