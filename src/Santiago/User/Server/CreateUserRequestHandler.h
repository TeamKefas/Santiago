#ifndef SANTIAGO_USER_SERVER_CREATEUSERREQUESTHANDLER_H
#define SANTIAGO_USER_SERVER_CREATEUSERREQUESTHANDLER_H

/**
 * @file CreateUserRequestHandler.h
 *
 * @section DESCRIPTION
 *
 * Contains the CreateUserRequestHandler class  
 */

#include "RequestHandlerBase.h"

namespace Santiago{namespace User { namespace Server
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
        CreateUserRequestHandler(ConnectionServer& connectionServer_
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
