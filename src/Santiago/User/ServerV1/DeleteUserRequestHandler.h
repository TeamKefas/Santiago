#ifndef SANTIAGO_USER_SERVER_DELETEUSERREQUESTHANDLER_H
#define SANTIAGO_USER_SERVER_DELETEUSERREQUESTHANDLER_H

/**
 * @file DeleteUserRequestHandler.h
 *
 * @section DESCRIPTION
 *
 * Contains the DeleteUserRequestHandler class  
 */

#include "RequestHandlerBase.h"

namespace Santiago{namespace User { namespace Server
{

    class DeleteUserRequestHandler:public RequestHandlerBase
    {
    public:
         typedef ThreadSpecificVar::ThreadSpecificVar<Santiago::SantiagoDBTables::MariaDBConnection> SantiagoDBConnection;
        
        /**
         * The constructor
         * @param connectionServer_- 
         * @param onCompletedCallbackFn_- ///NEED TO WRITE\\\
         * @param initiatingMessage_ -
         */
        typedef RequestHandlerBase MyBase;
        typedef typename MyBase::SendMessageCallbackFn SendMessageCallbackFn;
        typedef typename MyBase::OnCompletedCallbackFn OnCompletedCallbackFn;
        DeleteUserRequestHandler(ServerData& serverData_,
                                 SantiagoDBConnection& databaseConnection_,
                                 const SendMessageCallbackFn& sendMessageCallbackFn_,
                                 const OnCompletedCallbackFn& onCompletedCallbackFn_,
                                 const ServerMessage& initiatingMessage_);
       /**
        * ///Message\\
        */
        virtual void handleInitiatingRequest();
       /**
        * ///Message\\
        * @param serverMessage - 
        */
	virtual void handleReplyMessage(const ServerMessage& serverMessage);

               
    };

}}}

#endif
