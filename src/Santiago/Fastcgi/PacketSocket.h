#ifndef FASTCGI_PACKET_SOCKET_H
#define FASTCGI_PACKET_SOCKET_H

/**
 * @file PacketSocket.h
 *
 * @section DESCRIPTION
 *
 * Contains the PacketSocket class.
 */

#include <functional>

#include <boost/asio.hpp>
#include <boost/asio/streambuf.hpp>

#include "../fastcgi_devkit/fastcgi.h"

#include "Common.h"

namespace Santiago{ namespace Fastcgi
{
/**
 * This class reads/writes full fastcgi packets from the underlying socket thus
 * providing the abstraction of a socket that reads and writes fastcgi packets. It
 * takes the boost asio protocol tags as the template argument. 
 */
    template<typename Protocol>
    class PacketSocket:public std::enable_shared_from_this<PacketSocket<Protocol> >
    {

    private:
        static const uint BUFFER_INCREMENT_SIZE = 4096;

        enum State
        {
            OPEN,
            CLOSED
        };

    public:
        typedef typename Protocol::socket ProtocolSocket;
        typedef std::shared_ptr<ProtocolSocket> ProtocolSocketPtr;
        typedef std::shared_ptr<boost::asio::strand> StrandPtr;

        typedef std::function<void(boost::system::error_code,uint,uchar,uint,const char*)> NewPacketCallbackFn;

        /**
         * Deleting the copy constructor to make the class non copyable
         */
        PacketSocket(const PacketSocket&) = delete;
        
        /**
         * Deleting the copy assignment operator to make the class non copyable
         */
        PacketSocket& operator=(const PacketSocket&) = delete;

        /**
         * The constructor
         * @param connectionStrand_ - the strand of the connection 
         * @param protocolSocketPtr - the underlying socket
         * @param newpacketCallbackFn - callback function on receiving a new packet
         */
        PacketSocket(StrandPtr& connectionStrandPtr_,
                     ProtocolSocketPtr protocolSocketPtr_,
                     NewPacketCallbackFn newPacketCallbackFn_):
            _connectionStrandPtr(connectionStrandPtr_),
            _protocolSocketPtr(protocolSocketPtr_),
            _newPacketCallbackFn(newPacketCallbackFn_),
            _state(OPEN)
        {}

        /**
         * Start reading
         */
        void startAsyncPacketRead()
        {
            _protocolSocketPtr->async_read_some(
                _inputBuffer.prepare(BUFFER_INCREMENT_SIZE),
                (*_connectionStrandPtr).wrap(std::bind(&PacketSocket::handleRead,
                                                       this->shared_from_this(),
                                                       std::placeholders::_1,
                                                       std::placeholders::_2)));

        }

        /**
         * Synchronously writes a packet to the socket.
         * @param requestId
         * @param headerType
         * @param contentLength
         * @param content
         * @param ec- error code to be returned as reference
         */
        void writePacket(uint requestId_,uchar headerType_,unsigned int contentLength_,const char* content_,boost::system::error_code &ec_)
        {
            BOOST_ASSERT(_state != CLOSED);

            std::string padding;
            padding.append(256,0);

            FCGI_Header header;
            bzero(&header, sizeof(header));
            header.version = FCGI_VERSION_1;
            header.type = headerType_;
            header.requestIdB1 = (requestId_ >> 8) & 0xff;
            header.requestIdB0 = requestId_ & 0xff;
            
            for (std::string::size_type n = 0;;)
            {
                std::string::size_type written = std::min(contentLength_ - n,(std::string::size_type)0xffffu);

                header.contentLengthB1 = written >> 8;
                header.contentLengthB0 = written & 0xff;
                header.paddingLength = (8 - (written % 8)) % 8;
                
                std::vector<boost::asio::const_buffer> bufferSequence;
                bufferSequence.push_back(boost::asio::buffer(reinterpret_cast<const char*>(&header), sizeof(header)));
                bufferSequence.push_back(boost::asio::buffer(content_+n,written));
                bufferSequence.push_back(boost::asio::buffer(padding.data(),header.paddingLength));
                boost::asio::write(*_protocolSocketPtr,bufferSequence,ec_);
                if(ec_)
                {
                    std::cout<<"Connection disconnected"<<std::endl;
                    close();
                    return;
                }

                n += written;
                if (n == contentLength_)
                {
                    break;
                }
            }
        }

        /**
         * Closes the socket
         */        
        void close()
        {
            if(_state == CLOSED)
            {
                //already closed do nothing
                return;                
            }
            else
            {
                _protocolSocketPtr->cancel(); //cancel all remaining events
                boost::system::error_code ec;
                _protocolSocketPtr->shutdown(ProtocolSocket::shutdown_both,ec);
//                BOOST_ASSERT(!ec); //not always - can get disconnected in between. so remove this line later
                _connectionStrandPtr->post(std::bind(&PacketSocket::flushProtocolSocket,this->shared_from_this()));
                _state = CLOSED;
            }
        }

        /**
         * The destructor
         */
        virtual ~PacketSocket()
        {
            _protocolSocketPtr->close();
        }


    private:
        /**
         * The read handler to be called by boost asio. Calls the newPacketCallbackFn 
         * when a complete packet is received. This function is always called in the
         * connection strand.
         * @param error
         * @param bytesTransferred
         */
        void handleRead(const boost::system::error_code& error_,size_t bytesTransferred_)
        {
            _inputBuffer.commit(bytesTransferred_);
            if(_state == CLOSED)
            {
                return;
            }

            if(error_)
            {
                std::cout<<"Connection disconnected"<<std::endl;
                close();
                _newPacketCallbackFn(error_,0,0,0,NULL);
                return;
            }

            while (_inputBuffer.size() >= FCGI_HEADER_LEN)
            {
                const char* inputBufferData = boost::asio::buffer_cast<const char*>(_inputBuffer.data());
                
                const FCGI_Header& header = *reinterpret_cast<const FCGI_Header*>(inputBufferData);
                uint requestId = (header.requestIdB1 << 8) + header.requestIdB0;
                unsigned contentLength = (header.contentLengthB1 << 8) + header.contentLengthB0;
                try
                {
                    if (header.version != FCGI_VERSION_1)
                    {
                        throw std::runtime_error("Unsupported FCGI verison");
                        break;
                    }

                    if (_inputBuffer.size() < FCGI_HEADER_LEN + contentLength + header.paddingLength)
                    {
                        break;
                    }
                    
                    const char* content = inputBufferData + FCGI_HEADER_LEN;
                    _newPacketCallbackFn(error_,requestId,header.type,contentLength,content); 
                    _inputBuffer.consume(FCGI_HEADER_LEN + contentLength + header.paddingLength);                    
                    if(_state == CLOSED)
                    {
                        break;
                    }
                }
                catch(std::exception &e)
                {
                    std::cerr<<e.what();
                    _inputBuffer.consume(FCGI_HEADER_LEN + contentLength + header.paddingLength);
                }                
                //_inputBuffer.consume(FCGI_HEADER_LEN + contentLength + header.paddingLength);          
            }
        }

        /**
         * This function is posted by the close() and Outputs the extra data in the 
         * buffer after the close is called.
         */
        void flushProtocolSocket()
        {
            BOOST_ASSERT(_state == CLOSED);
            boost::system::error_code ec;
            std::cout<<"extra unused input data in socket:<";
            do
            {
                //--------Debugging --------------------
                const char* tempData = boost::asio::buffer_cast<const char*>(_inputBuffer.data());
                for(uint temp = 0;temp<_inputBuffer.size();temp++)
                {
                    std::cout<<tempData[temp];
                }
                //---------------------------
                //consume off already read data
;
                _inputBuffer.consume(_inputBuffer.size());
                size_t bytesRead = _protocolSocketPtr->read_some(_inputBuffer.prepare(BUFFER_INCREMENT_SIZE),ec);
                _inputBuffer.commit(bytesRead);

            }while(!ec);
            std::cout<<">"<<std::endl;
        }

        StrandPtr                    _connectionStrandPtr;
        ProtocolSocketPtr            _protocolSocketPtr;
        NewPacketCallbackFn          _newPacketCallbackFn;
        //DisconnectEventCallbackFn  _disconnectEventCallbackFn;

        State                        _state;
        boost::asio::streambuf       _inputBuffer;
    };
}}

#endif
