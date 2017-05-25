#ifndef SANTIAGO_AUTHENTICATION_SERVER_REMOVEDCOOKIEFROMAPPSERVERREQUESTHANDLER_H
#define SANTIAGO_AUTHENTICATION_SERVER_REMOVEDCOOKIEFROMAPPSERVERREQUESTHANDLER_H

/**
 * @file RemovedCookieFromAppserverRequestHandler.h
 *
 * @section DESCRIPTION
 *
 * Contains the RemovedCookieFromAppserverRequestHandler class  
 */

#include "RequestHandlerBase.h"

namespace Santiago{ namespace Authentication { namespace Server
{
    class RemovedCookieFromAppserverRequestHandler:public RequestHandlerBase
    {
    public:
        /**
         * The constructor
         * @param connectionServer_- 
         * @param onCompletedCallbackFn_ -
         * @param initiatingMessage_ -
         */
        RemovedCookieFromAppserverRequestHandler(ConnectionServer& connectionServer_,
                                                 const OnCompletedCallbackFn& onCompletedCallbackFn_,
                                                 const ServerMessage& initiatingMessage_);
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
