#ifndef SANTIAGO_AUTHENTICATION_ADDRESOURCEREQUESTHANDLER_H
#define SANTIAGO_AUTHENTICATION_ADDRESOURCEREQUESTHANDLER_H

#include "RequestHandlerBase.h"

namespace Santiago{ namespace Authentication
{
    class AddResourceRequestHandler:public RequestHandlerBase
    {
    public:
        AddResourceRequestHandler(ConnectionServer& connectionServer_,
                                  const OnCompletedCallbackFn& onCompletedCallbackFn_,
                                  const ServerMessage& initiatingMessage_);
        virtual void start();
        virtual void handleReplyMessage(const ServerMessage& serverMessage);
        
    };

}}

#endif
