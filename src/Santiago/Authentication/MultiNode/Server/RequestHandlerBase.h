#ifndef SANTIAGO_AUTHENTICATION_SERVER_CONNECTIONCONTROLLER_H
#define SANTIAGO_AUTHENTICATION_SERVER_CONNECTIONCONTROLLER_H

/**
 * @file RemovedCookieFromAppserverRequestHandler.h
 *
 * @section DESCRIPTION
 *
 * Contains the RemovedCookieFromAppserverRequestHandler class  
 */

#include <functional>
#include <algorithm>
#include "ServerData.h"
#include "ConnectionServer.h"

namespace Santiago{ namespace Authentication { namespace MultiNode { namespace Server
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
        RequestHandlerBase(ConnectionServer& connectionServer_,
                           ServerData& serverData_,
                           const OnCompletedCallbackFn& onCompletedCallbackFn_,
                           const ConnectionMessage& initiatingMessage_)
            :_connectionServer(connectionServer_),
             _serverData(serverData_),
             _onCompletedCallbackFn(onCompletedCallbackFn_),
             _initiatingMessage(initiatingMessage_)
        {}
       /**
        * ///Message\\
        */
        virtual void handleRequest() = 0;

    protected:

        ConnectionServer&              _connectionServer;
        ServerData                    &_serverData;
        OnCompletedCallbackFn          _onCompletedCallbackFn;
        ConnectionMessage              _initiatingMessage;
    };

}}}}

#endif
