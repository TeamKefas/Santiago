#ifndef SANTIAGO_USER_SERVER_CHANGEUSERPASSWORDREQUESTHANDLER_H
#define SANTIAGO_USER_SERVER_CHANGEUSERPASSWORDREQUESTHANDLER_H

/**
 * @file ChangeUserPasswordRequestHandler.h
 *
 * @section DESCRIPTION
 *
 * Contains the ChangeUserPasswordRequestHandler class  
 */

#include "RequestHandlerBase.h"

namespace Santiago{namespace User { namespace Server
{

    class ChangeUserPasswordRequestHandler:public RequestHandlerBase
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
        ChangeUserPasswordRequestHandler(ServerData& serverData_,
                                 SantiagoDBConnection& databaseConnection_,
                                 const SendMessageCallbackFn& sendMessageCallbackFn_,
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