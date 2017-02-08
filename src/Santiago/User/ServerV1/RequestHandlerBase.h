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
#include "ServerMessage.h"
#include "DatabaseInterface.h"
#include "ServerData.h"
#include "ConnectionServer.h"

namespace Santiago{ namespace User { namespace Server
{
    class RequestHandlerBase
    {
    public:
        typedef std::function<void(const RequestId&)> OnCompletedCallbackFn;
        /**
         * The constructor
         * @param connectionServer_- 
         * @param onCompletedCallbackFn_ -
         * @param initiatingMessage_ -
         */
        RequestHandlerBase(ServerData& serverData_,
                           ConnectionServer& connectionServer_,
                           const OnCompletedCallbackFn& onCompletedCallbackFn_,
                           const ServerMessage& initiatingMessage_)
            :_serverData(serverData_)
            ,_connectionServer(connectionServer_)
            ,_onCompletedCallbackFn(onCompletedCallbackFn_)
            ,_initiatingMessage(initiatingMessage_)
        {}
       /**
        * ///Message\\
        */
        virtual void start() = 0;
       /**
        * ///Message\\
        * @param serverMessage - \
        */
        virtual void handleReplyMessage(const ServerMessage& serverMessage_) = 0;

    protected:

        ConnectionServer&              &_connectionServer;
        OnCompletedCallbackFn          _onCompletedCallbackFn;
        ServerMessage                  _initiatingMessage;
        ServerData                     &_serverData;

        DatabaseInterface              _databaseInterface;
        

    };

}}}

#endif
