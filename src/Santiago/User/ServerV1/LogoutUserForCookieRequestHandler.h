#ifndef SANTIAGO_USER_SERVER_LOGOUTUSERFORCOOKIEREQUESTHANDLER_H
#define SANTIAGO_USER_SERVER_LOGOUTUSERFORCOOKIEREQUESTHANDLER_H

/**
 * @file LogoutUserForCookieRequestHandler.h
 *
 * @section DESCRIPTION
 *
 * Contains the LogoutUserForCookieRequestHandler class  
 */

#include "RequestHandlerBase.h"


namespace Santiago{ namespace User { namespace Server
{
    class LogoutUserForCookieRequestHandler : public RequestHandlerBase
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
        LogoutUserForCookieRequestHandler(ServerData& serverData_,
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
        * @param serverMessage - \
        */
        virtual void handleReplyMessage(const ServerMessage& serverMessage);

    };

}}}

#endif
