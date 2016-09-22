#ifndef SANTIAGO_AUTHENTICATION_CREATEUSERREQUESTHANDLER_H
#define SANTIAGO_AUTHENTICATION_CREATEUSERREQUESTHANDLER_H

#include "RequestHandlerBase.h"

namespace Santiago{namespace Authentication
{

    class CreateUserRequestHandler:public RequestHandlerBase
    {
    public:
        CreateUserRequestHandler(ConnectionServer& connectionServer_
                                 ,const OnCompletedCallbackFn& onCompletedCallbackFn_
                                 ,const ServerMessage& initiatingMessage_);
        virtual void start();
	virtual void handleReplyMessage(const ServerMessage& serverMessage);
        
    };

}}

#endif
