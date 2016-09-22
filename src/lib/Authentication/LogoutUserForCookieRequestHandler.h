#ifndef SANTIAGO_AUTHENTICATION_LOGOUTUSERFORCOOKIEREQUESTHANDLER_H
#define SANTIAGO_AUTHENTICATION_LOGOUTUSERFORCOOKIEREQUESTHANDLER_H

#include "RequestHandlerBase.h"


namespace Santiago{ namespace Authentication
{
    class LogoutUserForCookieRequestHandler:public RequestHandlerBase
    {
    public:
        LogoutUserForCookieRequestHandler(ConnectionServer& connectionServer_
                                          ,const OnCompletedCallbackFn& onCompletedCallbackFn_
                                          ,const ServerMessage& initiatingMessage_);
        virtual void start();
        virtual void handleReplyMessage(const ServerMessage& serverMessage);
        
    };

}}

#endif
