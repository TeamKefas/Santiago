/*
    Copyright 2012, 2013 Wilson Sunny Philip [wil.pazhorickal<at>gmail<dot>com]

    This file is part of the Santiago library.

    Santiago is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Santiago is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with Santiago.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef FASTCGI_RECORD_SOCKET_H
#define FASTCGI_RECORD_SOCKET_H

/**
 * @file RecordSocket.h
 * @author  Wilson Sunny Philip
 *
 * @section DESCRIPTION
 *
 * Contains the RecordSocket class.
 */

#include <boost/shared_ptr.hpp>
#include <boost/assert.hpp>
#include <boost/bind.hpp>

#include "fastcgi_devkit/fastcgi.h"

#include "PacketSocket.h"

namespace Santiago{ namespace Fastcgi
{
/**
 * This class reads/writes full fastcgi 'records'(requests ready to be processed) from
 * the underlying socket thus providing the abstraction of a socket that reads and 
 * writes fastcgi packets. It takes the boost asio protocol tags as the template
 * argument. 
 */
    template<typename Protocol>
    class RecordSocket
    {

    public:       
        typedef boost::shared_ptr<boost::asio::strand> StrandPtr;
        typedef typename Protocol::socket ProtocolSocket;
        typedef boost::shared_ptr<ProtocolSocket> ProtocolSocketPtr;      
        typedef boost::shared_ptr<PacketSocket<Protocol> > PacketSocketPtr; 

        typedef boost::function<void(uint,const char*,uint)> StdinCallbackFn;
        typedef boost::function<void(uint,const char*,uint)> ParamsCallbackFn;
        typedef boost::function<void(uint)> BeginRequestCallbackFn;
        typedef boost::function<void(uint)> AbortRequestCallbackFn;
        typedef boost::function<void(TransceiverEventInfo)> TransceiverEventCallbackFn;

        /**
         * The constructor
         * @param connectionStrandPtr
         * @param protocolSocketPtr
         * @param beginRequestCallbackFn
         * @param abortRequestCallbackFn
         * @param stdinCallbackFn
         * @param paramsCallbackFn
         * @param tranceivereventCallbackFn - for close/wind down
         */
        RecordSocket(StrandPtr connectionStrandPtr_,
                     ProtocolSocketPtr protocolSocketPtr_,
                     BeginRequestCallbackFn beginRequestCallbackFn_,
                     AbortRequestCallbackFn abortRequestCallbackFn_,
                     StdinCallbackFn stdinCallbackFn_,
                     ParamsCallbackFn paramsCallbackFn_,
                     TransceiverEventCallbackFn transceiverEventCallbackFn_):
            _connectionStrandPtr(connectionStrandPtr_),
            _packetSocketPtr(new PacketSocket<Protocol>(connectionStrandPtr_,
                                                        protocolSocketPtr_,
                                                        boost::bind(&RecordSocket::handleNewPacket,this,_1,_2,_3,_4,_5))),
            _beginRequestCallbackFn(beginRequestCallbackFn_),
            _abortRequestCallbackFn(abortRequestCallbackFn_),
            _stdinCallbackFn(stdinCallbackFn_),
            _paramsCallbackFn(paramsCallbackFn_),
            _transceiverEventCallbackFn(transceiverEventCallbackFn_)
        {}

        /**
         * Synchronously send the reply. This function is called by the connection and
         * is always called in the connection's strand.
         * @param requestId
         * @param outBuffer
         * @param errBuffer
         * @param appStatus - set by the user
         * @param error code
         */
        void sendReply(uint requestId_,boost::asio::streambuf &outBuffer_,boost::asio::streambuf& errBuffer_,int appStatus_,boost::system::error_code &ec_)
        {
//            BOOST_ASSERT((this->_state & SocketBase<Protocol,Socket>::INTERNAL_CLOSED)== 0);

            std::string outputBuffer;

            if(outBuffer_.size() != 0)
            {
                const char* outBufferArray = boost::asio::buffer_cast<const char*>(outBuffer_.data());
                _packetSocketPtr->writePacket(requestId_,FCGI_STDOUT,outBuffer_.size(),outBufferArray,ec_);
                if(ec_)
                {
                    return;
                }
                outBuffer_.consume(outBuffer_.size());
            }

            if(errBuffer_.size() != 0)
            {
                const char* errBufferArray = boost::asio::buffer_cast<const char*>(errBuffer_.data());
                _packetSocketPtr->writePacket(requestId_,FCGI_STDERR,errBuffer_.size(),errBufferArray,ec_);
                if(ec_)
                {
                    return;
                }
                errBuffer_.consume(errBuffer_.size());
            }

            _packetSocketPtr->writePacket(requestId_,FCGI_STDOUT,0,"",ec_); 
            if(ec_)
            {
                return;
            }
            _packetSocketPtr->writePacket(requestId_,FCGI_STDERR,0,"",ec_);
            if(ec_)
            {
                return;
            }


            FCGI_EndRequestBody complete;
            bzero(&complete, sizeof(complete));
            complete.appStatusB3 = (appStatus_ >> 24) & 0xff;
            complete.appStatusB2 = (appStatus_ >> 16) & 0xff;
            complete.appStatusB1 = (appStatus_ >> 8) & 0xff;
            complete.appStatusB0 = appStatus_ & 0xff;
            complete.protocolStatus = FCGI_REQUEST_COMPLETE;

            _packetSocketPtr->writePacket(requestId_,FCGI_END_REQUEST,sizeof(complete),reinterpret_cast<const char*>(&complete),ec_);
            if(ec_)
            {
                return;
            }
            return;
        }

        /**
         * Start reading the socket
         */
        void start()
        {
            _packetSocketPtr->startAsyncPacketRead();
        }

        /**
         * Close the underlying socket
         */
        void close()
        {
            _packetSocketPtr->close();
        }
    private:

        /**
         * Handles new packet.This function is called by the packetsocket and is always
         * called in the connection's strand.
         * @param error_code
         * @param requestid
         * @param headerType
         * @param contentLength
         * @param content
         */
        void handleNewPacket(boost::system::error_code ec_,uint requestId,unsigned char headerType,uint contentLength,const char* content)
        {
            if(ec_)
            {
                close();
                _transceiverEventCallbackFn(SOCKET_CLOSED);
                return;
            }

            switch (headerType)
            {
            case FCGI_GET_VALUES:
            {
                typedef std::map<std::string,std::string> Pairs;
                std::map<std::string,std::string> pairs;
                ParsePairs(content, contentLength,pairs);
                          
                std::string outputBuffer;
                for (Pairs::iterator it = pairs.begin(); it != pairs.end(); ++it)
                {
                    if (it->first == FCGI_MAX_CONNS)
                    {
                        WritePair(outputBuffer,it->first, std::string("100"));
                    }
                    else if (it->first == FCGI_MAX_REQS)
                    {
                        WritePair(outputBuffer,it->first, std::string("1000"));
                    }
                    else if (it->first == FCGI_MPXS_CONNS)
                    {
                        WritePair(outputBuffer,it->first, std::string("1"));
                    }
                }

                boost::system::error_code ec;
                _packetSocketPtr->writePacket(0,FCGI_GET_VALUES_RESULT,outputBuffer.size(),outputBuffer.data(),ec);
                if(ec)
                {
                    _transceiverEventCallbackFn(SOCKET_CLOSED);                    
                }
                break;
            }
            case FCGI_BEGIN_REQUEST:
            {
                if (contentLength < sizeof(FCGI_BeginRequestBody))
                {
                    throw std::runtime_error("invalid begin request body");
                }
                const FCGI_BeginRequestBody& body = *reinterpret_cast<const FCGI_BeginRequestBody*>(content);                        
                unsigned role = (body.roleB1 << 8) + body.roleB0;
                
                if (role != FCGI_RESPONDER)
                {
                    std::string outputBuffer;
                    FCGI_EndRequestBody unknown;
                    bzero(&unknown, sizeof(unknown));
                    unknown.protocolStatus = FCGI_UNKNOWN_ROLE;
                    outputBuffer.append( reinterpret_cast<const char*>(&unknown), sizeof(unknown));
                    boost::system::error_code ec;
                    _packetSocketPtr->writePacket(requestId,FCGI_END_REQUEST,outputBuffer.size(),outputBuffer.data(),ec);
                    if(ec)
                    {
                        _transceiverEventCallbackFn(SOCKET_CLOSED);
                    }
                    std::cout<<"Unknown role"<<std::endl;
                    return;
                }
                _beginRequestCallbackFn(requestId);
                
                if (!(body.flags & FCGI_KEEP_CONN))
                {
                    this->_transceiverEventCallbackFn(CONNECTION_WIND_DOWN);
                }
                break;
                
            }
            case FCGI_ABORT_REQUEST: 
            {
                std::string outputBuffer;                            
                FCGI_EndRequestBody aborted;
                bzero(&aborted, sizeof(aborted));
                aborted.appStatusB0 = 1;
                aborted.protocolStatus = FCGI_REQUEST_COMPLETE;
                outputBuffer.append(reinterpret_cast<const char*>(&aborted), sizeof(aborted));
                boost::system::error_code ec;
                _packetSocketPtr->writePacket(requestId,FCGI_REQUEST_COMPLETE,outputBuffer.size(),outputBuffer.data(),ec);
                if(ec)
                {
                    _transceiverEventCallbackFn(SOCKET_CLOSED);
                }
                _abortRequestCallbackFn(requestId);
                
                break;
            }
            case FCGI_PARAMS:
            {
                _paramsCallbackFn(requestId,content,contentLength);
                break;
            }                        
            case FCGI_STDIN:
            {
                _stdinCallbackFn(requestId,content,contentLength);
                break;                            
            }                        
            case FCGI_DATA:
            {
                throw std::runtime_error("FCGI_DATA ");
                break;
            }                        
            default:
            {
                std::string outputBuffer;
                FCGI_UnknownTypeBody unknown;
                bzero(&unknown, sizeof(unknown));
                unknown.type = headerType;
                outputBuffer.append(reinterpret_cast<const char*>(&unknown), sizeof(unknown));
                boost::system::error_code ec;
                _packetSocketPtr->writePacket(0,FCGI_UNKNOWN_TYPE,outputBuffer.size(),outputBuffer.data(),ec);
                if(ec)
                {
                    _transceiverEventCallbackFn(SOCKET_CLOSED);
                }

            }
            }
            
        }

        StrandPtr                      _connectionStrandPtr;
        PacketSocketPtr                _packetSocketPtr;
        BeginRequestCallbackFn         _beginRequestCallbackFn;
        AbortRequestCallbackFn         _abortRequestCallbackFn;
        StdinCallbackFn                _stdinCallbackFn;
        ParamsCallbackFn               _paramsCallbackFn;
        TransceiverEventCallbackFn     _transceiverEventCallbackFn;
    };


}}


#endif
