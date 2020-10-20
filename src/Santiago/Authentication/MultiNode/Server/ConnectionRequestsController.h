#ifndef SANTIAGO_AUTHENTICATION_MULTINODE_CONNECTIONREQUESTSCONTROLLER_H
#define SANTIAGO_AUTHENTICATION_MULTINODE_CONNECTIONREQUESTSCONTROLLER_H

/**
 * @file ConnectionRequestsController.h
 *
 * @section DESCRIPTION
 *
 * Contains the ConnectionRequestsController class  
 */

#include <functional>
#include <memory>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>

#include "../ConnectionMessage.h"
#include "../ConnectionMessageSocket.h"
#include "../ConnectionRequestsControllerBase.h"

namespace Santiago{ namespace Authentication{ namespace MultiNode{ namespace Server
{
    /* TODO: Need to move the fns that are common with
     * MultiNode::ConnectionRequestsController to 
     * MultiNode::ConnectionRequestsControllerBase 
     */
    class ConnectionRequestsController:public ConnectionRequestsControllerBase
    {
    public:
        
        typedef std::function<void()> OnDisconnectCallbackFn;
        typedef std::function<void(const std::error_code&,
                                   const boost::optional<ConnectionMessage>&)> OnReplyMessageCallbackFn;
        typedef std::function<void(const ConnectionMessage&)> OnNewRequestMessageCallbackFn;
        typedef boost::asio::ip::tcp::socket MySocket;
        typedef std::shared_ptr<MySocket> MySocketPtr;
        typedef std::shared_ptr<AsioStrand> StrandPtr;
                
        ConnectionRequestsController(boost::asio::io_service& ioService_,
                                     const MySocketPtr& socketPtr_,
                                     unsigned connectionId_,
                                     const StrandPtr& strandPtr_,
                                     const OnDisconnectCallbackFn& onDisconnectCallbackFn_,
                                     const OnNewRequestMessageCallbackFn& onNewRequestMessageCallbackFn_);

        void sendMessage(const ConnectionMessage& message_,
                         bool isReplyExpectingMessage_,
                         const boost::optional<OnReplyMessageCallbackFn>& onReplyMessageCallbackFn_);
        
        unsigned getConnectionId();
    protected:

        virtual void furtherHandleNewConnectionMessage(const ConnectionMessage& message_);
        virtual void furtherHandleReplyConnectionMessage(const ConnectionMessage& message_);
        virtual void furtherHandleConnectionDisconnectForReplyExpectingRequest(const RequestId& requestId_);
        virtual void furtherHandleConnectionDisconnect();
        virtual ConnectionMessageSocket& getConnectionMessageSocket();
 
        boost::asio::io_service                       &_ioService;
        ConnectionMessageSocket                        _connectionMessageSocket;
        unsigned                                       _connectionId;

        OnDisconnectCallbackFn                         _onDisconnectCallbackFn;
        OnNewRequestMessageCallbackFn                  _onNewRequestMessageCallbackFn;
        
        std::map<RequestId,OnReplyMessageCallbackFn>   _requestIdCallbackFnMap;
    };
}}}}

#endif
