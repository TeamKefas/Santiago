#include"ConnectionMessageSocket.h"

namespace Santiago{ namespace Authentication { namespace MultiNode
{
    ConnectionMessageSocket::
    ConnectionMessageSocket(const MySocketPtr& socketPtr_,
                            const OnDisconnectCallbackFn& onDisconnectCallbackFn_,
                            const OnMessageCallbackFn& onMessageCallbackFn_,
                            const StrandPtr& readStrandPtr_,
                            const StrandPtr& writeStrandPtr_)
        :_socketPtr(socketPtr_)
        ,_ioService(_socketPtr->get_io_service())
        ,_onDisconnectCallbackFn(onDisconnectCallbackFn_)
        ,_onMessageCallbackFn(onMessageCallbackFn_)
        ,_readStrandPtr(readStrandPtr_)
        ,_writeStrandPtr(writeStrandPtr_)
    {
        BOOST_ASSERT(_socketPtr);
    }
    
    void ConnectionMessageSocket::startAsyncRead()
    {
        if(_readStrandPtr)
        {
            _socketPtr->async_read_some(_inputBuffer.prepare(BUFFER_INCREMENT_SIZE),
                                        _readStrandPtr->wrap(boost::bind(&ConnectionMessageSocketBase::handleRead,
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
                unsigned  initiatingConnectionId = *(reinterpret_cast<const unsigned*>
                                                     (boost::asio::buffer_cast<const char*>(_inputBuffer.data())));
                _inputBuffer.consume(sizeof(unsigned));
                
                unsigned  requestNo =  *(reinterpret_cast<const unsigned*>
                                         (boost::asio::buffer_cast<const char*>(_inputBuffer.data())));
                _inputBuffer.consume(sizeof(unsigned));
                
                RequestId requestId(initiatingConnectionId,requestNo);
                 
                const char* inputBufferData = boost::asio::buffer_cast<const char*>(_inputBuffer.data());
                ConnectionMessageContent messageContent(inputBufferData,messageSize-12);
                _inputBuffer.consume(messageSize-12);
                _onMessageCallbackFn(requestId,messageContent);                
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

    void ConnectionMessageSocket::sendMessage(const RequestId& requestId_,
                                              const ConnectionMessageContent& messageContent_)
    {
        if(_writeStrandPtr)
        {
            _writeStrandPtr->dispatch(std::bind(&ConnectionMessageSocket::sendMessageImpl,
                                                this,
                                                requestId_,
                                                messageContent_));
        }
        else
        {
            sendMessageImpl(requestId_,messageContent_);
        }
    }
    
    void ConnectionMessageSocket::sendMessageImpl(const RequestId& requestId_,
                                                  const ConnectionMessageContent& messageContent_)
    {
        BOOST_ASSERT(_socketPtr);
        unsigned bufSize = sizeof(unsigned) + sizeof(unsigned) + sizeof(unsigned)+ messageContent_.getSize();
        boost::asio::streambuf outputBuffer;
        std::ostream outStream(&outputBuffer);
        outStream.write(reinterpret_cast<const char*>(&bufSize), sizeof(bufSize));
        outStream.write(reinterpret_cast<const char*>(&requestId_._initiatingConnectionId),
                         sizeof(requestId_._initiatingConnectionId));
        outStream.write(reinterpret_cast<const char*>(&requestId_._requestNo), sizeof(requestId_._requestNo));
        messageContent_.writeToStream(outStream);

        boost::system::error_code errorCode;
        BOOST_ASSERT(_socketPtr);
        unsigned size = boost::asio::write(*_socketPtr,outputBuffer,errorCode);
        if(!errorCode)
        {
            BOOST_ASSERT(bufSize == size);
        }
        else
        {
            close();
        }
    }

}}}      //closing namespace Santiago::Authentication::MultiNode

