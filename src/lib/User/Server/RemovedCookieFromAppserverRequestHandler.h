#ifndef SANTIAGO_AUTHENTICATION_REMOVEDCOOKIEFROMAPPSERVERREQUESTHANDLER_H
#define SANTIAGO_AUTHENTICATION_REMOVEDCOOKIEFROMAPPSERVERREQUESTHANDLER_H

#include "RequestHandlerBase.h"

namespace Santiago{ namespace Authentication
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

}}

#endif
