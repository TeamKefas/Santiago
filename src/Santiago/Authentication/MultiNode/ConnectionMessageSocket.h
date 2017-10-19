#ifndef SANTIAGO_AUTHENTICATION_MULTINODE_CONNECTIONMESSAGESOCKET_H
#define SANTIAGO_AUTHENTICATION_MULTINODE_CONNECTIONMESSAGESOCKET_H

/**
 * @file ConnectionMessageSocketNase.h
 *
 * @section DESCRIPTION
 *
 * Contains the ConnectionMessageSocket class 
 */

#include <memory>
#include <string>
#include <cstdlib>
#include <functional>
#include <ostream>

//#include <boost/asio/strand.hpp> //strand disabled for now
#include <boost/asio.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/streambuf.hpp>

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "ConnectionMessage.h"

namespace Santiago{ namespace Authentication { namespace MultiNode
{
    class ConnectionMessageSocket:public std::enable_shared_from_this<ConnectionMessageSocket>
    {
    public:

        static const unsigned BUFFER_INCREMENT_SIZE = 4096;

        typedef std::shared_ptr<ConnectionMessageSocket> Ptr;
        typedef boost::asio::ip::tcp::socket MySocket;
        typedef std::shared_ptr<MySocket> MySocketPtr;

        typedef std::function<void()> OnDisconnectCallbackFn;
        typedef std::function<void(const ConnectionMessage&)> OnMessageCallbackFn;
        typedef boost::shared_ptr<boost::asio::strand> StrandPtr;

        ConnectionMessageSocket(const MySocketPtr& socketPtr_,
                                const OnDisconnectCallbackFn& onDisconnectCallbackFn_,
                                const OnMessageCallbackFn& onMessageCallbackFn_,
                                const StrandPtr& readStrandPtr_=nullptr,
                                const StrandPtr& writeStrandPtr_=nullptr);

        void sendMessage(const ConnectionMessage& message_);
        void close();

    protected:

        void startAsyncRead();
        void sendMessageImpl(const ConnectionMessage& message_);
        void handleRead(const boost::system::error_code& error_,size_t bytesTransferred_);

        MySocketPtr                             _socketPtr;
        boost::asio::io_service&                _ioService;
        OnDisconnectCallbackFn                  _onDisconnectCallbackFn;
        OnMessageCallbackFn                     _onMessageCallbackFn;
        StrandPtr                               _readStrandPtr;
        StrandPtr                               _writeStrandPtr;

        boost::asio::streambuf                  _inputBuffer;
    };



}}} //closing namespace Santiango::Authentication::MultiNode

#endif

