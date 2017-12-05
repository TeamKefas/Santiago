#ifndef SANTIAGO_AUTHENTICATION_SERVER_DELETEUSERREQUESTHANDLER_H
#define SANTIAGO_AUTHENTICATION_SERVER_DELETEUSERREQUESTHANDLER_H

/**
 * @file DeleteUserRequestHandler.h
 *
 * @section DESCRIPTION
 *
 * Contains the DeleteUserRequestHandler class  
 */

#include "RequestHandlerBase.h"

namespace Santiago{namespace Authentication { namespace MultiNode { namespace Server
{

    class DeleteUserRequestHandler:public RequestHandlerBase
    {
    public:
        /**
         * The constructor
         * @param connectionServer_- 
         * @param onCompletedCallbackFn_- ///NEED TO WRITE\\\
         * @param initiatingMessage_ -
         */
        DeleteUserRequestHandler(ConnectionServer& connectionServer_,
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
