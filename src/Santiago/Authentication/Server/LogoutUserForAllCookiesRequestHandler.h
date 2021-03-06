#ifndef SANTIAGO_AUTHENTICATION_SERVER_LOGOUTUSERFORALLCOOKIESREQUESTHANDLER_H
#define SANTIAGO_AUTHENTICATION_SERVER_LOGOUTUSERFORALLCOOKIESREQUESTHANDLER_H

/**
 * @file LogoutUserForAllCookiesRequestHandler.h
 *
 * @section DESCRIPTION
 *
 * Contains the LogoutUserForAllCookiesRequestHandler class  
 */

#include "RequestHandlerBase.h"


namespace Santiago{ namespace Authentication { namespace Server
{
    class LogoutUserForAllCookiesRequestHandler:public RequestHandlerBase
    {
    public:
        /**
         * The constructor
         * @param connectionServer_- 
         * @param onCompletedCallbackFn_ -
         * @param initiatingMessage_ -
         */
        LogoutUserForAllCookiesRequestHandler(ConnectionServer& connectionServer_
                                              ,const OnCompletedCallbackFn& onCompletedCallbackFn_
                                              ,const ServerMessage& initiatingMessage_);
       /**
        * ///Message\\
        */
        virtual void start();
       /**
        * ///Message\\
        * @param serverMessage - \
        */
        virtual void handleReplyMessage(const ServerMessage& serverMessage);
    };

}}}

#endif
