#ifndef SANTIAGO_AUTHENTICATION_MULTINODE_SERVER_SERVERIMPL_H
#define SANTIAGO_AUTHENTICATION_MULTINODE_SERVER_SERVERIMPL_H

#include <memory>

#include <boost/asio/strand.hpp>

#include "Santiago/SantiagoDBTables/MariaDBConnection.h"
#include "Santiago/Thread/ThreadSpecificVar.h"
#include "Santiago/Authentication/MultiNode/AuthenticatorImpl.h"
#include "ConnectionServer.h"
#include "Controller.h"

namespace Santiago{ namespace Authentication{ namespace MultiNode{ namespace Server
{
    struct ServerImpl
    {
        typedef Thread::ThreadSpecificVar<SantiagoDBTables::MariaDBConnection> ThreadSpecificDbConnection;
        typedef std::shared_ptr<boost::asio::strand> StrandPtr;
        typedef std::shared_ptr<AuthenticatorImplBase> AuthenticatorImplBasePtr;
        typedef std::function<void(const Authentication::MultiNode::ConnectionMessage&)> OnNewRequestCallbackFn;
        typedef std::function<void(unsigned)> OnDisconnectCallbackFn;  

        ServerImpl(ThreadSpecificDbConnection& databaseConnection_,
                   const StrandPtr& strandPtr_,
                   ConnectionServer& connectionServer_,
                   boost::asio::io_service& ioService_,
                   unsigned port_,
                   const OnDisconnectCallbackFn& onDisconnectCallbackFn_,
                   const OnNewRequestCallbackFn& onNewRequestCallbackFn_);
                    
        void processRequest(const ConnectionMessage& connectionMessage_, boost::asio::yield_context yield_);

        const OnDisconnectCallbackFn&                 _onDisconnectCallbackFn;
        const OnNewRequestCallbackFn&                 _onNewRequestCallbackFn;
        ConnectionServer                              _connectionServer;
        Controller                                    _controller;
        std::pair<AuthenticatorImplBasePtr,StrandPtr> _authenticatorStrandPair[26];
    };

}}}}

#endif
