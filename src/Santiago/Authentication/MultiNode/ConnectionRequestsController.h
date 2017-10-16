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
                                     const boost::asio::ip::tcp::endpoint& endpoint_,
                                     const OnDisconnectCallbackFn& onDisconnectCallbackFn_,
                                     const OnMessageCallbackFn& onMessageCallbackFn_);

        void sendMessage(const AuthenticatorRequestMessage& message_);//BOOST_ASSERT(ConnectionMessageSocketOpt) and call MyBase::sendMessage()

    protected:

        void createAndInitializeConnectionMessageSocket(const boost::asio::ip::tcp::endpoint& endpoint_); //creates asio socketPtr, blocking reads an unsigned, sets the _connectionId, sets the ConnectionMessageSocketOpt
        
        virtual void handleRequestMessage(ConnectionMessageType2 messageType_,
                                          const RequestId& requestId_,
                                          const boost::optional<ConnectionMessageContent>& messageContentOpt_);//creates an AuthenticatorRequestMessage and call the onMessageCallbackFn
        
        virtual void furtherHandleConnectionMessageSocketDisconnect(); //reset the ConnectionMessageSocketOpt...and call createAndInitializeConnectionMessageSocket

        boost::asio::io_service        &_ioService;
        boost::asio::ip::tcp::endpoint  _endpoint;
        OnDisconnectCallbackFn          _onDisconnectCallbackFn;
        OnMessageCallbackFn             _onMessageCallbackFn;

        unsigned                        _connectionId;
        ConnectionMessageSocketOpt      _connectionMessageSocketOpt;
    };
}}}

#endif
