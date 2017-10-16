#ifndef SANTIAGO_AUTHENTICATION_SERVER_LOGINUSERREQUESTHANDLER_H
#define SANTIAGO_AUTHENTICATION_SERVER_LOGINUSERREQUESTHANDLER_H

/**
 * @file LoginUserRequestHandler.h
 *
 * @section DESCRIPTION
 *
 * Contains the LoginUserRequestHandler class  
 */

#include "RequestHandlerBase.h"

namespace Santiago{ namespace Authentication { namespace Server
{
    class LoginUserRequestHandler:public RequestHandlerBase
    {
    public:
        /**
         * The constructor
         * @param connectionServer_- 
         * @param onCompletedCallbackFn_ -
         * @param initiatingMessage_ -
         */
        LoginUserRequestHandler(ConnectionServer& connectionServer_
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
