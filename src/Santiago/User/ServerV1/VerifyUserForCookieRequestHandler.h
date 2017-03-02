#ifndef SANTIAGO_USER_SERVER_VERIFYUSERFORCOOKIEREQUESTHANDLER_H
#define SANTIAGO_USER_SERVER_VERIFYUSERFORCOOKIEREQUESTHANDLER_H

/**
 * @file VerifyUserForCookieRequestHandler.h
 *
 * @section DESCRIPTION
 *
 * Contains the VerifyUserForCookieRequestHandler class  
 */

#include "RequestHandlerBase.h"


namespace Santiago{ namespace User { namespace Server
{
    class VerifyUserForCookieRequestHandler:public RequestHandlerBase
    {
    public:
         typedef ThreadSpecificVar::ThreadSpecificVar<Santiago::SantiagoDBTables::MariaDBConnection> SantiagoDBConnection;
        /**
         * The constructor
         * @param connectionServer_-
         * @param onCompletedCallbackFn_ -
         * @param initiatingMessage_ -
         */
        VerifyUserForCookieRequestHandler(ServerData& serverData_,
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
