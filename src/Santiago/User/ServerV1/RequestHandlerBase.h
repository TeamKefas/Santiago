#ifndef SANTIAGO_USER_SERVER_CONNECTIONCONTROLLER_H
#define SANTIAGO_USER_SERVER_CONNECTIONCONTROLLER_H

/**
 * @file RemovedCookieFromAppserverRequestHandler.h
 *
 * @section DESCRIPTION
 *
 * Contains the RemovedCookieFromAppserverRequestHandler class  
 */

#include <functional>
#include <algorithm>
#include <openssl/sha.h>

#include "../../ThreadSpecificVar/ThreadSpecificVar.h"

#include "ServerMessage.h"
#include "../../SantiagoDBTables/MariaDBConnection.h"
#include "ServerData.h"
#include "ConnectionServer.h"
#include "../../Utils/STLog.h"
#include "../../Utils/PTimeUtils.h"

namespace Santiago{ namespace User { namespace Server
{
    class RequestHandlerBase
    {
    public:
        
        typedef ThreadSpecificVar::ThreadSpecificVar<Santiago::SantiagoDBTables::MariaDBConnection> SantiagoDBConnection;
        typedef std::function<void(const RequestId&)> OnCompletedCallbackFn;
        typedef std::function<void(const ServerMessage&)> SendMessageCallbackFn;
        /**
         * The constructor
         * @param connectionServer_- 
         * @param onCompletedCallbackFn_ -
         * @param initiatingMessage_ -
         */
        RequestHandlerBase(ServerData& serverData_,
                           SantiagoDBConnection& databaseConnection_,
                           const SendMessageCallbackFn& sendMessageCallbackFn_,
                           const OnCompletedCallbackFn& onCompletedCallbackFn_,
                           const ServerMessage& initiatingMessage_)
            :_serverData(serverData_)
            ,_databaseConnection(databaseConnection_)
            ,_sendMessageCallbackFn(sendMessageCallbackFn_)
            ,_onCompletedCallbackFn(onCompletedCallbackFn_)
            ,_initiatingMessage(initiatingMessage_)
        {}
       /**
        *  Used to perform the corresponding Handler Operations.
        */
        virtual void start() = 0;
       /**
        * ///Message\\
        * @param serverMessage - \
        */
        virtual void handleReplyMessage(const ServerMessage& serverMessage_) = 0;

    protected:

        std::string generateSHA256(const std::string str);
      
        ServerData                    &_serverData;       
        SantiagoDBConnection          &_databaseConnection;
        SendMessageCallbackFn          _sendMessageCallbackFn;
        OnCompletedCallbackFn          _onCompletedCallbackFn;
        ServerMessage                  _initiatingMessage;

    };

}}}

#endif
