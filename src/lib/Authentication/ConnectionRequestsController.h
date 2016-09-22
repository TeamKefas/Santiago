#ifndef SANTIAGO_AUTHENTICATION_CONNECTIONREQUESTSCONTROLLER_H
#define SANTIAGO_AUTHENTICATION_CONNECTIONREQUESTSCONTROLLER_H

#include <memory>

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "ServerMessage.h"
#include "ConnectionMessageSocket.h"

namespace Santiago{ namespace Authentication
{
    class ConnectionRequestsController
    {
    public:

        typedef std::shared_ptr<ConnectionRequestsController> Ptr;
        typedef ConnectionMessageSocket::Ptr ConnectionMessageSocketPtr;
        
        typedef std::function<void(const ServerMessage&)> OnNewRequestCallbackFn;
        typedef std::function<void(const ServerMessage&)> OnRequestReplyCallbackFn;
        typedef std::function<void(unsigned)> OnDisconnectCallbackFn;

        ConnectionRequestsController(unsigned connectionId_,
                                     const ConnectionMessageSocket::MySocketPtr& socketPtr_,
                                     const OnDisconnectCallbackFn& onDisconnectCallbackFn_,
                                     const OnNewRequestCallbackFn& onNewRequestCallbackFn_,
                                     const OnRequestReplyCallbackFn& onRequestReplyCallbackFn_);

        void sendMessage(const ServerMessage& message_);
        

    protected:

        void handleConnectionMessageSocketDisconnect();
        void handleConnectionMessageSocketMessage(const RequestId& requestId_, const ConnectionMessage& message_);

        unsigned                      _connectionId;

        OnDisconnectCallbackFn        _onDisconnectCallbackFn;
        OnNewRequestCallbackFn        _onNewRequestCallbackFn;
        OnRequestReplyCallbackFn      _onRequestReplyCallbackFn;

        ConnectionMessageSocket       _connectionMessageSocket;

        std::map<RequestId,unsigned>  _replyPendingRequestList; //the pair.second gives the number of pending replies

    };

}}

#endif
