#ifndef SANTIAGO_USER_SERVER_PINGTYPE1REQUESTHANDLER_H
#define SANTIAGO_USER_SERVER_PINGTYPE1REQUESTHANDLER_H

/**
 * @file TestRequestHandler.h
 *
 * @section DESCRIPTION
 *
 * Contains the TestRequestHandler class for testing Server Client networking mechanism. 
 */

#include "RequestHandlerBase.h"

namespace Santiago{ namespace User { namespace Server
{
    class PingType1RequestHandler:public RequestHandlerBase
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
        PingType1RequestHandler(ServerData& serverData_,
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
        virtual void handleReplyMessage(const ServerMessage& serverMessage_);

    private:
    };

}}}

#endif
