#ifndef SANTIAGO_AUTHENTICATION_SERVER_CHANGEUSERPASSWORDREQUESTHANDLER_H
#define SANTIAGO_AUTHENTICATION_SERVER_CHANGEUSERPASSWORDREQUESTHANDLER_H

/**
 * @file ChangeUserPasswordRequestHandler.h
 *
 * @section DESCRIPTION
 *
 * Contains the ChangeUserPasswordRequestHandler class 
 */

#include "RequestHandlerBase.h"

namespace Santiago{ namespace Authentication { namespace MultiNode { namespace Server
{
    class ChangeUserPasswordRequestHandler:public RequestHandlerBase
    {
    public:
         /**
         * The constructor
         * @param connectionServer_- 
         * @param onCompletedCallbackFn_- ///NEED TO WRITE\\\
         * @param initiatingMessage
         */
        ChangeUserPasswordRequestHandler(ConnectionServer& connectionServer_,
                                         ServerData& serverData_,
                                         const OnCompletedCallbackFn& onCompletedCallbackFn_,
                                         const ConnectionMessage& initiatingMessage_);
        /**
         * ///Message\\
         */
        virtual void handleRequest();
        
    };

}}}}

#endif
