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
#ifndef FASTCGI_CONNECTION_DATA_H
#define FASTCGI_CONNECTION_DATA_H

/**
 * @file PacketSocket.h
 * @author  Wilson Sunny Philip
 *
 * @section DESCRIPTION
 *
 * Contains the ConnectionData class.
 */

#include <map>
#include <utility>
#include <stdexcept>

#include <boost/assert.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include "Common.h"
#include "RequestData.h"

namespace Santiago{ namespace Fastcgi
{
/**
 * This class stores and manages all the current alive requests. All its functions are
 * called in the connection's strand. 
 */
    class ConnectionData
    {
    public:

        typedef unsigned char RequestState;
        static const RequestState IN_COMPLETED     = 1;//0b00000001;
        static const RequestState PARAMS_COMPLETED = 2;//0b00000010;

        typedef boost::shared_ptr<RequestData> RequestDataPtr;
        typedef boost::weak_ptr<RequestData> RequestDataWeakPtr;

        typedef std::map<uint,std::pair<RequestState,RequestDataPtr> > RequestMap;

        typedef boost::function<void(uint,RequestDataWeakPtr)> RequestReadyCallbackFn;
        typedef boost::function<void()> EmptyCallbackFn;

        /**
         * The constructor
         * @param requestReadyCallbackFn - for when a request is ready for user
         * processing
         * @param emptyCallbackFn - called when the connection data becomes empty
         */
        ConnectionData(RequestReadyCallbackFn requestReadyCallbackFn_,
                       EmptyCallbackFn emptyCallbackFn_):
            _requestReadyCallbackFn(requestReadyCallbackFn_),
            _emptyCallbackFn(emptyCallbackFn_)   
        {}

        /**
         * The destructor
         */
        ~ConnectionData()
        {}

        /**
         * Called when a new request is received
         * @param requestId
         */
        void handleBeginRequest(uint requestId_)
        {
            if(_requestMap.find(requestId_) != _requestMap.end())
            {
                throw std::runtime_error("requestId already exists");
            }            
            _requestMap[requestId_] = std::pair<RequestState,RequestDataPtr>(0/*0b00000000*/,RequestDataPtr(new RequestData()));            
        }

        /**
         * Called when a request is aborted
         * @param requestId
         */
        void handleAbortRequest(uint requestId_)
        {
            RequestMap::iterator iter = _requestMap.find(requestId_); 
            if(iter == _requestMap.end())
            {
                throw std::runtime_error("abort requestId does not exist");
            }

            _requestMap.erase(iter);
            checkForEmptyRequestMap();
        }

        /**
         * Called when stdin data of a request received at the socket.
         * @param requestId
         */
        void handleStdin(uint requestId_,const char* inBuffer_,uint size_)
        {
            RequestMap::iterator iter = _requestMap.find(requestId_); 
            if(iter == _requestMap.end())
            {
                throw std::runtime_error("stdin requestId does not exist");
            }
            if((iter->second.first & IN_COMPLETED) != 0)
            {
                throw std::runtime_error("stdin already closed");
            }

            if(size_ == 0)
            {
                iter->second.first |= IN_COMPLETED;
            }
            else
            {
                iter->second.second->_in.append(inBuffer_,size_);
            }

            checkForRequestReady(iter); 
        }
            
        /**
         * Called when params data of a request received at the socket.
         * @param requestId
         * @param inBuffer
         * @param size
         */
        void handleParams(uint requestId_,const char* inBuffer_,uint size_)
        {
            RequestMap::iterator iter = _requestMap.find(requestId_); 
            if(iter == _requestMap.end())
            {
                throw std::runtime_error("params requestId does not exist");
            }
            if((iter->second.first & PARAMS_COMPLETED) != 0)
            {
                throw std::runtime_error("params already closed");
            }

            if(size_ == 0)
            {
                iter->second.second->parseFCGIParams();
                iter->second.first |= PARAMS_COMPLETED;                
            }
            else
            {
                iter->second.second->_paramsBuffer.append(inBuffer_,size_);
            }

            checkForRequestReady(iter);
        }

        /**
         * Cleans up a request
         * @param requestId
         */
        void cleanupRequest(uint _requestId)
        {
            RequestMap::iterator iter = _requestMap.find(_requestId);
            if(iter == _requestMap.end())
            {
                throw std::runtime_error("requestId does not exist");
            }

            BOOST_ASSERT((iter->second.first & IN_COMPLETED) != 0 && (iter->second.first & IN_COMPLETED) != 0);

            _requestMap.erase(iter);  
          
            checkForEmptyRequestMap();
        }

        /**
         * Clears all the requests
         */
        void clear() //no need to call emptyCallbackFn here
        {
            _requestMap.clear();
        }

        /**
         * Size of connection data.
         */
        uint size()
        {
            return _requestMap.size();
        }

        /**
         * Checks if a requestId and requestdata pair are valid
         * @param requestId_
         * @param requestDataPtr_
         */
        bool isValidRequest(uint requestId_,RequestDataPtr requestDataPtr_)
        {
            RequestMap::iterator iter = _requestMap.find(requestId_);
            if(iter != _requestMap.end())
            {
                if(iter->second.second == requestDataPtr_)
                {
                    return true;
                }
            }
            return false;
        }

    private:

        /**
         * Checks if the request map has become empty.
         */
        void checkForEmptyRequestMap()
        {
            if(_requestMap.size() == 0)
            {
                _emptyCallbackFn();
            }
        }

        /**
         * Checks if the request is ready for user processing.
         * @param iter_ - iterator of request in request map
         */
        void checkForRequestReady(RequestMap::iterator iter_)
        {
            if((iter_->second.first & PARAMS_COMPLETED) != 0 && (iter_->second.first & IN_COMPLETED) != 0)
            {
                _requestReadyCallbackFn(iter_->first,RequestDataWeakPtr(iter_->second.second));
            }
        }


        RequestReadyCallbackFn           _requestReadyCallbackFn;
        EmptyCallbackFn                  _emptyCallbackFn;
        RequestMap                       _requestMap;
    };
}}

#endif
