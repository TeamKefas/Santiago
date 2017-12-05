#ifndef SANTIAGO_AUTHENTICATION_SERVER_CREATEANDRETURNRECOVERYSTRINGREQUESTHANDLER_H
#define SANTIAGO_AUTHENTICATION_SERVER_CREATEANDRETURNRECOVERYSTRINGREQUESTHANDLER_H

/**
 * @file CreateAndReturnRecoveryStringRequestHandler.h
 *
 * @section DESCRIPTION
 *
 * Contains the CreateAndReturnRecoveryStringRequestHandler class  
 */

#include "RequestHandlerBase.h"

namespace Santiago{namespace Authentication { namespace MultiNode { namespace Server
{

    class CreateAndReturnRecoveryStringRequestHandler:public RequestHandlerBase
    {
    public:
        /**
         * The constructor
         * @param connectionServer_- 
         * @param onCompletedCallbackFn_- ///NEED TO WRITE\\\
         * @param initiatingMessage_ -
         */
        CreateAndReturnRecoveryStringRequestHandler(ConnectionServer& connectionServer_,
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
