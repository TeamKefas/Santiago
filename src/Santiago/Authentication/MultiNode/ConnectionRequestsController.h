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

#include "AuthenticatorRequestMessage.h"
#include "ConnectionMessage.h"
#include "ConnectionMessageSocket.h"
#include "ConnectionRequestsControllerBase.h"

namespace Santiago{ namespace Authentication{ namespace MultiNode
{
    class ConnectionRequestsController:public ConnectionRequestsControllerBase
    {
    public:
        
        typedef std::shared_ptr<ConnectionRequestsControllerBase> Ptr;
        typedef std::function<void()> OnDisconnectCallbackFn;
        typedef std::function<void(const AuthenticatorRequestMessage&)> OnMessageCallbackFn;
        typedef boost::optional<ConnectionMessageSocket> ConnectionMessageSocketOpt;
        typedef std::shared_ptr<boost::asio::strand> StrandPtr;
        
        ConnectionRequestsController(boost::asio::io_service& ioService_,
                                     const StrandPtr& strandPtr_,
                                     const boost::asio::ip::tcp::endpoint& endpoint_,
                                     const OnDisconnectCallbackFn& onDisconnectCallbackFn_,
                                     const OnMessageCallbackFn& onServerRequestCallbackFn_);

        void sendMessage(const AuthenticatorRequestMessage& message_,
                         const boost::optional<OnMessageCallbackFn>& onMessageCallbackFn_);
    protected:

        void createAndInitializeConnectionMessageSocket(const boost::asio::ip::tcp::endpoint& endpoint_);
        
        virtual void handleRequestMessage(ConnectionMessageRequest messageType_,
                                          const RequestId& requestId_,
                                          const boost::optional<ConnectionMessageContent>& messageContentOpt_);

        virtual void queueConnectAfterDelay();
        virtual void furtherHandleConnectionMessageSocketDisconnect();
        virtual ConnectionMessageSocket& getConnectionMessageSocket();

        boost::asio::io_service                  &_ioService;
        StrandPtr                                 _strandPtr;
        boost::asio::ip::tcp::endpoint            _endpoint;
        OnDisconnectCallbackFn                    _onDisconnectCallbackFn;
        OnMessageCallbackFn                       _onServerRequestCallbackFn;

        unsigned                                  _connectionId;
        ConnectionMessageSocketOpt                _connectionMessageSocketOpt;

        std::map<RequestId,OnMessageCallbackFn>   _requestIdCallbackFnMap;
    };
}}}

#endif
