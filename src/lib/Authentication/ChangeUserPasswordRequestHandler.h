#ifndef SANTIAGO_AUTHENTICATION_CHANGEUSERPASSWORDREQUESTHANDLER_H
#define SANTIAGO_AUTHENTICATION_CHANGEUSERPASSWORDREQUESTHANDLER_H

#include "RequestHandlerBase.h"

namespace Santiago{ namespace Authentication
{
    class ChangeUserPasswordRequestHandler:public RequestHandlerBase
    {
    public:
        ChangeUserPasswordRequestHandler(ConnectionServer& connectionServer_,
                                         const OnCompletedCallbackFn& onCompletedCallbackFn_,
                                         const ServerMessage& initiatingMessage_);
        virtual void start();
        virtual void handleReplyMessage(const ServerMessage& serverMessage);
        
    };

}}

#endif
