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

        void sendMessageImpl(ConnectionMessageType2 messageType_,
                             const RequestId& requestId_,
                             const ConnectionMessageContent& messageContent_);
        
        void handleConnectionMessageSocketDisconnect();
        void handleConnectionMessageSocketMessage(const RequestId& requestId_, const ConnectionMessageContent& messageContent_);

        virtual void handleRequestMessage(ConnectionMessageType2 messageType_,
                                          const RequestId& requestId_,
                                          const boost::optional<ConnectionMessageContent>& messageContentOpt_) = 0;
        virtual void furtherHandleConnectionMessageSocketDisconnect() = 0;        
        virtual ConnectionMessageSocket& getConnectionMessageSocket() = 0;

        OnDisconnectCallbackFn          _onDisconnectCallbackFn;
        ConnectionMessageSocket         _connectionMessageSocket;
        std::map<RequestId,unsigned>    _replyPendingRequestList; //the pair.second gives the number of pending replies..usually should only be 1
    };

}}}

#endif
