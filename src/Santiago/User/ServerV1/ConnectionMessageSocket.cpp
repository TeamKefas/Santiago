#include"ConnectionMessageSocket.h"

namespace Santiago{ namespace User { namespace Server
{

    ConnectionMessageSocket::ConnectionMessageSocket(const MySocketPtr& socketPtr_,
                                                     const OnDisconnectCallbackFn& onDisconnectCallbackFn_,
                                                     const OnMessageCallbackFn& onMessageCallbackFn_,
                                                     unsigned connectionId_)
        :_socketPtr(socketPtr_)
        ,_ioService(_socketPtr->get_io_service())
//        ,_strandPtr(new boost::asio::strand(_ioService))
        ,_onDisconnectCallbackFn(onDisconnectCallbackFn_)
        ,_onMessageCallbackFn(onMessageCallbackFn_)
        ,_connectionId(connectionId_)
    {
        ST_ASSERT(_socketPtr);
    }
    
    void ConnectionMessageSocket::start()
    {
        ST_LOG_DEBUG("Starting ConnectionMessageSocket listening. connectionId = "<<_connectionId<<std::endl);
        _socketPtr->async_read_some(_inputBuffer.prepare(BUFFER_INCREMENT_SIZE),
                                    /*_strandPtr->wrap(*/boost::bind(&ConnectionMessageSocket::handleRead,
                                                                     this->shared_from_this(),
                                                                     boost::asio::placeholders::error,
                                                                     boost::asio::placeholders::bytes_transferred)/*)*/);
    }

    void ConnectionMessageSocket::parseMessage(size_t bytesTransferred_)
    {
        ST_LOG_DEBUG("Starting parseMessage. connectionId = "<<_connectionId<<std::endl);

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
                try
                {
                    ConnectionMessage message(inputBufferData,messageSize-12);
                    _inputBuffer.consume(messageSize-12);
                    ST_LOG_DEBUG("Successfully parsed ConnectionMessage. connectionId = "<<_connectionId<<std::endl);
                    _onMessageCallbackFn(requestId,message);
                }
                catch(std::exception& e)
                {
                    ST_LOG_ERROR("Exception received. connectionId ="<<_connectionId
                                 <<", initiatingConnectionId = "<<initiatingConnectionId
                                 <<", requestNo " <<requestNo
                                 <<" Message = "<<e.what()<<std::endl);                    
                    _inputBuffer.consume(messageSize-12);
                }
            }
            else
            {
                break;
            }            
        }
    }
    
    
    void ConnectionMessageSocket::handleRead(const boost::system::error_code& error_,size_t bytesTransferred_)
    {
        ST_LOG_DEBUG("Starting ConnectionMessageSocket handleRead. connectionId = "<<_connectionId<<std::endl);
        if(error_)
        {
            ST_LOG_INFO("Connection disconnected. connectionId = "<<_connectionId
                        <<" Error:"<<error_.message()<<std::endl
                        <<"closing ConnectionMessageSocket"<<std::endl);
//            parseMessage(bytesTransferred_);
            close();
            return;
        }
        else
        {
            parseMessage(bytesTransferred_);
        }
        start();
        ST_LOG_DEBUG("handleRead completed. connectionId ="<<_connectionId<<std::endl);
    }
    
    void ConnectionMessageSocket::close()
    {
        ST_LOG_INFO("Closing socket. connectionId="<<_connectionId<<std::endl);
        _socketPtr.reset();
        _onDisconnectCallbackFn();
    }

    void ConnectionMessageSocket::sendMessage(const RequestId& requestId_,const ConnectionMessage& message_)
    {
//        _strandPtr->dispatch(boost::bind(&TCPConnection::sendMessageImpl,this,message_));
        sendMessageImpl(requestId_,message_);
    }

    void ConnectionMessageSocket::sendMessageImpl(const RequestId& requestId_, const ConnectionMessage& message_)
    {
        ST_LOG_DEBUG("Starting sendMessageImpl. connectionId = "<<_connectionId
                     << ", initiatingConnectionId = "<<requestId_._initiatingConnectionId
                     <<", requestNo = "<<requestId_._requestNo
                     <<std::endl);

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
        ST_ASSERT(_socketPtr);
        unsigned size = boost::asio::write(*_socketPtr,outputBuffer,errorCode);
        if(!errorCode)
        {
            ST_LOG_DEBUG("Successfully sent replied. connectionId = "<<_connectionId<<std::endl);
            ST_ASSERT(message_.getSize()+12 == size);
        }
        else
        {
            ST_LOG_INFO("Send message failed. Closing connection. connectionId = "<<_connectionId
                     << ", initiatingConnectionId = "<<requestId_._initiatingConnectionId
                     <<", requestNo = "<<requestId_._requestNo
                     <<std::endl);
            close();
        }
    }
    
}}}      //closing namespace Santiago::User::Server

