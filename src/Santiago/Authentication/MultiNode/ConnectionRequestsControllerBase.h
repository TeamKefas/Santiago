#ifndef SANTIAGO_AUTHENTICATION_MULTINODE_CONNECTIONREQUESTSCONTROLLERBASE_H
#define SANTIAGO_AUTHENTICATION_MULTINODE_CONNECTIONREQUESTSCONTROLLERBASE_H

/**
 * @file ConnectionRequestsController.h
 *
 * @section DESCRIPTION
 *
 * Contains the ConnectionRequestsControllerBase class  
 */

#include <memory>

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "ConnectionMessage.h"
#include "ConnectionMessageSocket.h"

namespace Santiago{ namespace Authentication{ namespace MultiNode
{
    class ConnectionRequestsControllerBase
    {
    public:

        typedef std::shared_ptr<ConnectionRequestsControllerBase> Ptr;
        typedef std::function<void()> OnDisconnectCallbackFn;
        
        ConnectionRequestsControllerBase();

    protected:

        void sendMessageImpl(const ConnectionMessage& message_,bool isReplyExpectingMessage_);
        void handleConnectionDisconnect();
        void handleConnectionMessage(const ConnectionMessage& message_);

        virtual void furtherHandleNewConnectionMessage(const ConnectionMessage& message_) = 0;
        virtual void furtherHandleReplyConnectionMessage(const ConnectionMessage& message_) = 0;
        virtual void furtherHandleConnectionDisconnectForReplyExpectingRequest(const RequestId& requestId_) = 0;
        virtual void furtherHandleConnectionDisconnect() = 0;
        virtual ConnectionMessageSocket& getConnectionMessageSocket() = 0;

        OnDisconnectCallbackFn          _onDisconnectCallbackFn;
        std::map<RequestId,unsigned>    _replyExpectingRequestList; //the pair.second gives the number of pending replies..usually should only be 1
    };

}}}

#endif
