#ifndef SANTIAGO_USER_SERVER_LOGINUSERREQUESTHANDLER_H
#define SANTIAGO_USER_SERVER_LOGINUSERREQUESTHANDLER_H

/**
 * @file LoginUserRequestHandler.h
 *
 * @section DESCRIPTION
 *
 * Contains the LoginUserRequestHandler class  
 */

#include "RequestHandlerBase.h"


namespace Santiago{ namespace User { namespace Server
{
    class LoginUserRequestHandler:public RequestHandlerBase
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
        LoginUserRequestHandler(ServerData& serverData_,
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

    private:
        std::string generateUniqueCookie();
    };

}}}

#endif
