#ifndef SANTIAGO_USER_SERVER_LOGOUTUSERFORALLCOOKIESREQUESTHANDLER_H
#define SANTIAGO_USER_SERVER_LOGOUTUSERFORALLCOOKIESREQUESTHANDLER_H

#include "RequestHandlerBase.h"


namespace Santiago{ namespace User { namespace Server
{
    class LogoutUserForAllCookiesRequestHandler:public RequestHandlerBase
    {
    public:
        LogoutUserForAllCookiesRequestHandler(ConnectionServer& connectionServer_
                                              ,const OnCompletedCallbackFn& onCompletedCallbackFn_
                                              ,const ServerMessage& initiatingMessage_);
        virtual void start();
        virtual void handleReplyMessage(const ServerMessage& serverMessage);
    };

}}}

#endif
