#ifndef SANTIAGO_USER_SERVER_REMOVEDCOOKIEFROMAPPSERVERREQUESTHANDLER_H
#define SANTIAGO_USER_SERVER_REMOVEDCOOKIEFROMAPPSERVERREQUESTHANDLER_H

#include "RequestHandlerBase.h"

namespace Santiago{ namespace User { namespace Server
{
    class RemovedCookieFromAppserverRequestHandler:public RequestHandlerBase
    {
    public:
        RemovedCookieFromAppserverRequestHandler(ConnectionServer& connectionServer_,
                                                 const OnCompletedCallbackFn& onCompletedCallbackFn_,
                                                 const ServerMessage& initiatingMessage_);
        virtual void start();
        virtual void handleReplyMessage(const ServerMessage& serverMessage);
        
    };

}}}

#endif
