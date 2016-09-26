#ifndef SANTIAGO_AUTHENTICATION_CONNECTIONCONTROLLER_H
#define SANTIAGO_AUTHENTICATION_CONNECTIONCONTROLLER_H

#include <functional>
#include <algorithm>

#include "ServerMessage.h"
#include "DatabaseInterface.h"
#include "ServerData.h"
#include "ConnectionServer.h"

namespace Santiago{ namespace Authentication
{
    class RequestHandlerBase
    {
    public:
        typedef std::function<void(const RequestId&)> OnCompletedCallbackFn;

        RequestHandlerBase(ConnectionServer& connectionServer_
                           ,const OnCompletedCallbackFn& onCompletedCallbackFn_
                           ,const ServerMessage& initiatingMessage_)
            :_connectionServer(connectionServer_)
            ,_onCompletedCallbackFn(onCompletedCallbackFn_)
            ,_initiatingMessage(initiatingMessage_)
        {}

        virtual void start() = 0;
        virtual void handleReplyMessage(const ServerMessage& serverMessage_) = 0;

    protected:

        ConnectionServer&              _connectionServer;
        OnCompletedCallbackFn          _onCompletedCallbackFn;
        ServerMessage                  _initiatingMessage;
        ServerData                     _serverData;

        DatabaseInterface              _databaseInterface; 

    };

}}

#endif
