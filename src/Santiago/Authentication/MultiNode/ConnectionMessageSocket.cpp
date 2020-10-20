#include"ConnectionMessageSocket.h"

namespace Santiago{ namespace Authentication { namespace MultiNode
{
    ConnectionMessageSocket::
    ConnectionMessageSocket(const MySocketPtr& socketPtr_,
                            boost::asio::io_service& ioService_,
                            const OnDisconnectCallbackFn& onDisconnectCallbackFn_,
                            const OnMessageCallbackFn& onMessageCallbackFn_,
                            const StrandPtr& readStrandPtr_,
                            const StrandPtr& writeStrandPtr_)
        :_socketPtr(socketPtr_)
        ,_ioService(ioService_)
        ,_onDisconnectCallbackFn(onDisconnectCallbackFn_)
        ,_onMessageCallbackFn(onMessageCallbackFn_)
        ,_readStrandPtr(readStrandPtr_)
        ,_writeStrandPtr(writeStrandPtr_)
    {
        BOOST_ASSERT(_socketPtr);
        startAsyncRead();
    }
    
    void ConnectionMessageSocket::startAsyncRead()
    {
        if(_readStrandPtr)
        {
            _socketPtr->async_read_some(_inputBuffer.prepare(BUFFER_INCREMENT_SIZE),
                                        _readStrandPtr->wrap(boost::bind(&ConnectionMessageSocket::handleRead,
                                                                         this->shared_from_this(),
                                                                         boost::asio::placeholders::error,
                                                                         boost::asio::placeholders::bytes_transferred)));

        }
        else
        {
            _socketPtr->async_read_some(_inputBuffer.prepare(BUFFER_INCREMENT_SIZE),
                                        boost::bind(&ConnectionMessageSocket::handleRead,
                                                    this->shared_from_this(),
                                                    boost::asio::placeholders::error,
                                                    boost::asio::placeholders::bytes_transferred));
        }
    }

    void ConnectionMessageSocket::handleRead(const boost::system::error_code& error_,size_t bytesTransferred_)
    {
        if(error_)
        {
            close();
            _onDisconnectCallbackFn();
            return;
        }

        _inputBuffer.commit(bytesTransferred_);
        while (_inputBuffer.size())
        {
            unsigned messageSize = *(reinterpret_cast<const unsigned*>
                                     (boost::asio::buffer_cast<const char*>(_inputBuffer.data())));
            
            if(_inputBuffer.size() >= messageSize)
            {
                _inputBuffer.consume(sizeof(unsigned));

                const char* inputBufferData = boost::asio::buffer_cast<const char*>(_inputBuffer.data());
                ConnectionMessage message(inputBufferData,messageSize-sizeof(unsigned));
                _inputBuffer.consume(messageSize-sizeof(unsigned));
                _onMessageCallbackFn(message);
            }
            else
            {
                break;
            }
        }
        
        startAsyncRead();
    }
    
    void ConnectionMessageSocket::close()
    {
        _socketPtr.reset();
    }

    void ConnectionMessageSocket::sendMessage(const ConnectionMessage& message_)
    {
        if(_writeStrandPtr)
        {
            _writeStrandPtr->dispatch(std::bind(&ConnectionMessageSocket::sendMessageImpl,
                                            this,
                                            message_));
        }
        else
        {
            sendMessageImpl(message_);
        }
    }
    
    void ConnectionMessageSocket::sendMessageImpl(const ConnectionMessage& message_)
    {
        BOOST_ASSERT(_socketPtr);
        unsigned bufSize = sizeof(unsigned)+ message_.getSize();
        boost::asio::streambuf outputBuffer;
        std::ostream outStream(&outputBuffer);
        outStream.write(reinterpret_cast<const char*>(&bufSize), sizeof(bufSize));
        message_.writeToStream(outStream);

        boost::system::error_code errorCode;
        BOOST_ASSERT(_socketPtr);
        unsigned size = boost::asio::write(*_socketPtr,outputBuffer,errorCode);
        if(!errorCode)
        {
            BOOST_ASSERT(bufSize == size);
        }
        else
        {
            //Suppressing the error here as read handler expected to call the
            //onDisconnectCallbackFn and do the cleanup. Few unnecessary read calls may be
            //made due to this.
        }
    }

}}}      //closing namespace Santiago::Authentication::MultiNode

