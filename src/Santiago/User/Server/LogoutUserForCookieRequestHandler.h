#ifndef SANTIAGO_USER_SERVER_LOGOUTUSERFORCOOKIEREQUESTHANDLER_H
#define SANTIAGO_USER_SERVER_LOGOUTUSERFORCOOKIEREQUESTHANDLER_H

/**
 * @file LogoutUserForCookieRequestHandler.h
 *
 * @section DESCRIPTION
 *
 * Contains the LogoutUserForCookieRequestHandler class  
 */

#include "RequestHandlerBase.h"


namespace Santiago{ namespace User { namespace Server
{
    class LogoutUserForCookieRequestHandler:public RequestHandlerBase
    {
    public:
        /**
         * The constructor
         * @param connectionServer_- 
         * @param onCompletedCallbackFn_ -
         * @param initiatingMessage_ -
         */
        LogoutUserForCookieRequestHandler(ConnectionServer& connectionServer_
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
