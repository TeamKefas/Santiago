#ifndef SANTIAGO_USER_SERVER_CONNECTIONMESSAGESOCKET_H
#define SANTIAGO_USER_SERVER_CONNECTIONMESSAGESOCKET_H

/**
 * @file ConnectionMessageSocket.h
 *
 * @section DESCRIPTION
 *
 * Contains the ConnectionMessageSocket class and ConnectionMessageType class 
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

#include "ServerMessage.h"
#include "../../Utils/STLog.h"

namespace Santiago{ namespace User { namespace Server
{
    class ConnectionMessageSocket:public std::enable_shared_from_this<ConnectionMessageSocket>
    {
    public:

        static const unsigned BUFFER_INCREMENT_SIZE = 4096;
//        typedef boost::shared_ptr<boost::asio::strand> StrandPtr;
        typedef std::shared_ptr<ConnectionMessageSocket> Ptr;
        typedef boost::asio::ip::tcp::socket MySocket;
        typedef std::shared_ptr<MySocket> MySocketPtr;

        typedef std::function<void(unsigned)> OnDisconnectCallbackFn;
        typedef std::function<void(const RequestId&,const ConnectionMessage&)> OnMessageCallbackFn;
        /**
         * The constructor
         * @param socketPtr_- 
         * @param onDisconnectCallbackFn_- ///NEED TO WRITE\\\
         * @param onMessageCallbackFn_ - 
         */
        ConnectionMessageSocket(const MySocketPtr& socketPtr_,
                                const OnDisconnectCallbackFn& onDisconnectCallbackFn_,
                                const OnMessageCallbackFn& onMessageCallbackFn_);
        
       /**
        * ///Message\\
        * @param requestId_ -
        * @param message_ - 
        */
        void sendMessage(const RequestId& requestId_,const ConnectionMessage& message_);
        /**
         * ///Message\\
         */
        void start();
        /**
         * ///Message\\
         */
        void close();

    private:
        /**
         * ///Message\\
         * @param error_ -
         * @param bytesTransferred_ - 
         */
        void handleRead(const boost::system::error_code& error_,size_t bytesTransferred_);
        /**
         * ///Message\\
         * @param bytesTransferred_ -
         */
        void parseMessage(size_t bytesTransferred_);
         /**
         * ///Message\\
         * @param requestId_ -
         * @param message_ - 
         */
        void sendMessageImpl(const RequestId& requestId_,const ConnectionMessage& message_);

        MySocketPtr                             _socketPtr;
        boost::asio::io_service&                _ioService;
//        StrandPtr                               _strandPtr;
        OnDisconnectCallbackFn                  _onDisconnectCallbackFn;
        OnMessageCallbackFn                     _onMessageCallbackFn;

        boost::asio::streambuf                  _inputBuffer;
    };



}}} //closing namespace Santiango::User::Server

#endif

