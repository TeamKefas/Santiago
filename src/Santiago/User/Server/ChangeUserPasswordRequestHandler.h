#ifndef SANTIAGO_USER_SERVER_CHANGEUSERPASSWORDREQUESTHANDLER_H
#define SANTIAGO_USER_SERVER_CHANGEUSERPASSWORDREQUESTHANDLER_H

#include "RequestHandlerBase.h"

namespace Santiago{ namespace User { namespace Server
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

}}}

#endif
