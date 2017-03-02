#ifndef SANTIAGO_USER_SERVER_LOGOUTUSERFORALLCOOKIESREQUESTHANDLER_H
#define SANTIAGO_USER_SERVER_LOGOUTUSERFORALLCOOKIESREQUESTHANDLER_H

/**
 * @file LogoutUserForAllCookiesRequestHandler.h
 *
 * @section DESCRIPTION
 *
 * Contains the LogoutUserForAllCookiesRequestHandler class  
 */

#include "RequestHandlerBase.h"

namespace Santiago{ namespace User { namespace Server
{
    class LogoutUserForAllCookiesRequestHandler:public RequestHandlerBase
    {
    public:
        typedef ThreadSpecificVar::ThreadSpecificVar<Santiago::SantiagoDBTables::MariaDBConnection> SantiagoDBConnection;
        typedef RequestHandlerBase MyBase;
        typedef typename MyBase::SendMessageCallbackFn SendMessageCallbackFn;
        typedef typename MyBase::OnCompletedCallbackFn OnCompletedCallbackFn;
        /**
         * The constructor
         * @param connectionServer_- 
         * @param onCompletedCallbackFn_ -
         * @param initiatingMessage_ -
         */
        LogoutUserForAllCookiesRequestHandler(ServerData& serverData_,
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
        * @param serverMessage - \
        */
        virtual void handleReplyMessage(const ServerMessage& serverMessage);

    };

}}}

#endif
