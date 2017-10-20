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

#include "ConnectionMessage.h"
#include "ConnectionMessageSocket.h"
#include "ConnectionRequestsControllerBase.h"

namespace Santiago{ namespace Authentication{ namespace MultiNode
{
    class ConnectionRequestsController:public ConnectionRequestsControllerBase
    {
    public:
        
        typedef std::function<void()> OnDisconnectCallbackFn;
        typedef std::function<void(const std::error_code&,
                                   const boost::optional<ConnectionMessage>&)> OnReplyMessageCallbackFn;
        typedef std::function<void(const ConnectionMessage&)> OnServerRequestMessageCallbackFn;
        typedef boost::optional<ConnectionMessageSocket> ConnectionMessageSocketOpt;
        typedef std::shared_ptr<boost::asio::strand> StrandPtr;
        
        ConnectionRequestsController(boost::asio::io_service& ioService_,
                                     const StrandPtr& strandPtr_,
                                     const boost::asio::ip::tcp::endpoint& endpoint_,
                                     const OnDisconnectCallbackFn& onDisconnectCallbackFn_,
                                     const OnServerRequestMessageCallbackFn& onServerRequestMessageCallbackFn_);

        void sendMessage(const ConnectionMessage& message_,
                         bool isReplyExpectingMessage_,
                         const boost::optional<OnReplyMessageCallbackFn>& onReplyMessageCallbackFn_);
    protected:

        void createAndInitializeConnectionMessageSocket();
        void queueConnectAfterDelay();

        virtual void furtherHandleNewConnectionMessage(const ConnectionMessage& message_);
        virtual void furtherHandleReplyConnectionMessage(const ConnectionMessage& message_);
        virtual void furtherHandleConnectionDisconnectForReplyExpectingRequest(const RequestId& requestId_);
        virtual void furtherHandleConnectionDisconnect();
        virtual ConnectionMessageSocket& getConnectionMessageSocket();
       
 
        boost::asio::io_service                  &_ioService;
        StrandPtr                                 _strandPtr;
        boost::asio::ip::tcp::endpoint            _endpoint;
        OnDisconnectCallbackFn                    _onDisconnectCallbackFn;
        OnServerRequestMessageCallbackFn          _onServerRequestMessageCallbackFn;

        unsigned                                  _connectionId;
        ConnectionMessageSocketOpt                _connectionMessageSocketOpt;

        std::map<RequestId,OnReplyMessageCallbackFn>   _requestIdCallbackFnMap;
    };
}}}

#endif
