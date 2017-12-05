#ifndef SANTIAGO_AUTHENTICATION_SERVER_CREATEUSERREQUESTHANDLER_H
#define SANTIAGO_AUTHENTICATION_SERVER_CREATEUSERREQUESTHANDLER_H

/**
 * @file CreateUserRequestHandler.h
 *
 * @section DESCRIPTION
 *
 * Contains the CreateUserRequestHandler class  
 */

#include "RequestHandlerBase.h"

namespace Santiago{namespace Authentication { namespace MultiNode { namespace Server
{

    class CreateUserRequestHandler:public RequestHandlerBase
    {
    public:
        /**
         * The constructor
         * @param connectionServer_- 
         * @param onCompletedCallbackFn_- ///NEED TO WRITE\\\
         * @param initiatingMessage_ -
         */
        CreateUserRequestHandler(ConnectionServer& connectionServer_,
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
