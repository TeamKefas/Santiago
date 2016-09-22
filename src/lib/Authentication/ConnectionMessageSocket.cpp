#include"ConnectionMessageSocket.h"

namespace Santiago{ namespace Authentication
{

    ConnectionMessageSocket::ConnectionMessageSocket(const MySocketPtr& socketPtr_,
                                                     const OnDisconnectCallbackFn& onDisconnectCallbackFn_,
                                                     const OnMessageCallbackFn& onMessageCallbackFn_)
        :_socketPtr(socketPtr_)
        ,_ioService(_socketPtr->get_io_service())
//        ,_strandPtr(new boost::asio::strand(_ioService))
        ,_onDisconnectCallbackFn(onDisconnectCallbackFn_)
        ,_onMessageCallbackFn(onMessageCallbackFn_)
    {
        BOOST_ASSERT(_socketPtr);
    }
    
    void ConnectionMessageSocket::start()
    {

        _socketPtr->async_read_some(_inputBuffer.prepare(BUFFER_INCREMENT_SIZE),
                                    /*_strandPtr->wrap(*/boost::bind(&ConnectionMessageSocket::handleRead,
                                                                     this->shared_from_this(),
                                                                     boost::asio::placeholders::error,
                                                                     boost::asio::placeholders::bytes_transferred)/*)*/);
    }

    void ConnectionMessageSocket::parseMessage(size_t bytesTransferred_)
    {
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
                ConnectionMessage message(inputBufferData,messageSize-12);
                _inputBuffer.consume(messageSize-12);
                _onMessageCallbackFn(requestId,message);
            }
            else
            {
                break;
            }
            
        }
    }
    
    
    void ConnectionMessageSocket::handleRead(const boost::system::error_code& error_,size_t bytesTransferred_)
    {
       
        if(error_)
        {
            parseMessage(bytesTransferred_);
            close();   //check for error and do cleanup.
            return;
        }
        else
        {
            parseMessage(bytesTransferred_);
        }
        start();
    }
    
    void ConnectionMessageSocket::close()
    {
        _socketPtr.reset();
        // _onDisconnectCallbackFn(1);
    }

    void ConnectionMessageSocket::sendMessage(const RequestId& requestId_,const ConnectionMessage& message_)
    {
//        _strandPtr->dispatch(boost::bind(&TCPConnection::sendMessageImpl,this,message_));
        sendMessageImpl(requestId_,message_);
    }

    void ConnectionMessageSocket::sendMessageImpl(const RequestId& requestId_, const ConnectionMessage& message_)
    {
        unsigned bufSize = sizeof(unsigned) + sizeof(unsigned) + sizeof(unsigned)+ message_.getSize();
        boost::asio::streambuf outputBuffer;
        std::ostream outStream(&outputBuffer);
        outStream.write(reinterpret_cast<const char*>(&bufSize), sizeof(bufSize));
        outStream.write(reinterpret_cast<const char*>(&requestId_._initiatingConnectionId),
                         sizeof(requestId_._initiatingConnectionId));
        outStream.write(reinterpret_cast<const char*>(&requestId_._requestNo), sizeof(requestId_._requestNo));
        // outStream<<message_;
        message_.writeToStream(outStream);

        boost::system::error_code errorCode;
        BOOST_ASSERT(_socketPtr);
        unsigned size = boost::asio::write(*_socketPtr,outputBuffer,errorCode);
        if(!errorCode)
        {
            BOOST_ASSERT(message_.getSize() == size);
        }
        else
        {
            close();
        }
    }
    
}}      //closing namespace Santiago::Authentication

