#ifndef SANTIAGO_AUTHENTICATION_SERVER_CHANGEUSERPASSWORDFOREMAILADDRESSANDRECOVERYSTRINGREQUESTHANDLER_H
#define SANTIAGO_AUTHENTICATION_SERVER_CHANGEUSERPASSWORDFOREMAILADDRESSANDRECOVERYSTRINGREQUESTHANDLER_H

/**
 * @file ChangeUserPasswordForEmailAddressAndRecoveryStringRequestHandler.h
 *
 * @section DESCRIPTION
 *
 * Contains the ChangeUserPasswordForEmailAddressAndRecoveryStringRequestHandler class  
 */

#include "RequestHandlerBase.h"

namespace Santiago{namespace Authentication { namespace MultiNode { namespace Server
{

    class ChangeUserPasswordForEmailAddressAndRecoveryStringRequestHandler:public RequestHandlerBase
    {
    public:
        /**
         * The constructor
         * @param connectionServer_- 
         * @param onCompletedCallbackFn_- ///NEED TO WRITE\\\
         * @param initiatingMessage_ -
         */
        ChangeUserPasswordForEmailAddressAndRecoveryStringRequestHandler(ConnectionServer& connectionServer_,
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
