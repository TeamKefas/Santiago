#ifndef SANTIAGO_AUTHENTICATION_SERVER_VERIFYUSERFORCOOKIEREQUESTHANDLER_H
#define SANTIAGO_AUTHENTICATION_SERVER_VERIFYUSERFORCOOKIEREQUESTHANDLER_H

/**
 * @file VerifyUserForCookieRequestHandler.h
 *
 * @section DESCRIPTION
 *
 * Contains the VerifyUserForCookieRequestHandler class  
 */

#include "RequestHandlerBase.h"


namespace Santiago{ namespace Authentication { namespace Server
{
    class VerifyUserForCookieRequestHandler:public RequestHandlerBase
    {
    public:
        /**
         * The constructor
         * @param connectionServer_-
         * @param onCompletedCallbackFn_ -
         * @param initiatingMessage_ -
         */
        VerifyUserForCookieRequestHandler(ConnectionServer& connectionServer_
                                          ,const OnCompletedCallbackFn& onCompletedCallbackFn_
                                          ,const ServerMessage& initiatingMessage_);
       /**
        * ///Message\\
        */
        virtual void start();
       /**
        * ///Message\\
        * @param serverMessage - 
        */
        virtual void handleReplyMessage(const ServerMessage& serverMessage);
        
    };

}}}

#endif
