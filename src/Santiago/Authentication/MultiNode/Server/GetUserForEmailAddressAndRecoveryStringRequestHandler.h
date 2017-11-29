#ifndef SANTIAGO_AUTHENTICATION_SERVER_GETUSERFOREMAILADDRESSANDRECOVERYSTRINGREQUESTHANDLER_H
#define SANTIAGO_AUTHENTICATION_SERVER_GETUSERFOREMAILADDRESSANDRECOVERYSTRINGREQUESTHANDLER_H

/**
 * @file GetUserForEmailAddressAndRecoveryStringRequestHandler.h
 *
 * @section DESCRIPTION
 *
 * Contains the GetUserForEmailAddressAndRecoveryStringRequestHandler class  
 */

#include "RequestHandlerBase.h"

namespace Santiago{namespace Authentication { namespace Server
{

    class GetUserForEmailAddressAndRecoveryStringRequestHandler:public RequestHandlerBase
    {
    public:
        /**
         * The constructor
         * @param connectionServer_- 
         * @param onCompletedCallbackFn_- ///NEED TO WRITE\\\
         * @param initiatingMessage_ -
         */
        GetUserForEmailAddressAndRecoveryStringRequestHandler(ConnectionServer& connectionServer_,
                                                              ServerData& serverData_,
                                                              const OnCompletedCallbackFn& onCompletedCallbackFn_,
                                                              const ConnectionMessage& initiatingMessage_);
       /**
        * ///Message\\
        */
        virtual void handleRequest();
    };

}}}

#endif
