#ifndef SANTIAGO_USER_SERVER_ADDRESOURCEREQUESTHANDLER_H
#define SANTIAGO_USER_SERVER_ADDRESOURCEREQUESTHANDLER_H

/**
 * @file AddResourceRequestHandler.h
 *
 * @section DESCRIPTION
 *
 * Contains the AddResourceRequestHandler class 
 */

#include "RequestHandlerBase.h"

namespace Santiago{ namespace User { namespace Server
{
    class AddResourceRequestHandler:public RequestHandlerBase
    {
    public:
        /**
         * The constructor
         * @param connectionServer_- 
         * @param onCompletedCallbackFn_- 
         * @param initiatingMessage_ -
         */
        AddResourceRequestHandler(ConnectionServer& connectionServer_,
                                  const OnCompletedCallbackFn& onCompletedCallbackFn_,
                                  const ServerMessage& initiatingMessage_);
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
