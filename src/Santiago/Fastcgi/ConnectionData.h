#ifndef FASTCGI_CONNECTION_DATA_H
#define FASTCGI_CONNECTION_DATA_H

/**
 * @file PacketSocket.h
 *
 * @section DESCRIPTION
 *
 * Contains the ConnectionData class.
 */

#include <functional>
#include <map>
#include <utility>
#include <stdexcept>

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

        typedef unsigned char RequestStateFlags;
        static const RequestStateFlags IN_COMPLETED     = 1;//0b00000001;
        static const RequestStateFlags PARAMS_COMPLETED = 2;//0b00000010;

        typedef std::shared_ptr<RequestData> RequestDataPtr;

        typedef std::shared_ptr<IsRequestAlive> IsRequestAlivePtr;
        typedef std::weak_ptr<IsRequestAlive> IsRequestAliveWeakPtr;

        struct RequestStateInfo
        {
            RequestStateInfo(RequestStateFlags requestStateFlags_, const IsRequestAlivePtr& isRequestAlivePtr_):
                _requestStateFlags(requestStateFlags_),
                _isRequestAlivePtr(isRequestAlivePtr_)
            {}
            
            RequestStateFlags _requestStateFlags;
            IsRequestAlivePtr _isRequestAlivePtr;
        };

        typedef std::map<uint,std::pair<RequestStateInfo,RequestDataPtr> > RequestMap;

        typedef std::function<void(uint,RequestDataPtr)> RequestReadyCallbackFn;
        typedef std::function<void()> EmptyCallbackFn;

        /**
         * The constructor
         * @param requestReadyCallbackFn - for when a request is ready for user
         * processing
         * @param emptyCallbackFn - called when the connection data becomes empty
         */
        ConnectionData(const RequestReadyCallbackFn& requestReadyCallbackFn_,
                       const EmptyCallbackFn& emptyCallbackFn_):
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

            RequestStateInfo requestStateInfo(0/*0b00000000*/,IsRequestAlivePtr(new IsRequestAlive()));
            RequestDataPtr requestDataPtr(new RequestData(IsRequestAliveWeakPtr(requestStateInfo._isRequestAlivePtr)));

            bool insertFlag = _requestMap.insert(
                std::make_pair(requestId_,
                               std::make_pair(requestStateInfo,requestDataPtr))).second;
            BOOST_ASSERT(insertFlag);
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
            if((iter->second.first._requestStateFlags & IN_COMPLETED) != 0)
            {
                throw std::runtime_error("stdin already closed");
            }

            if(size_ == 0)
            {
                iter->second.first._requestStateFlags |= IN_COMPLETED;
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
            if((iter->second.first._requestStateFlags & PARAMS_COMPLETED) != 0)
            {
                throw std::runtime_error("params already closed");
            }

            if(size_ == 0)
            {
                iter->second.second->parseFCGIParams();
                iter->second.first._requestStateFlags |= PARAMS_COMPLETED;                
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

            BOOST_ASSERT((iter->second.first._requestStateFlags & IN_COMPLETED) != 0 && 
                         (iter->second.first._requestStateFlags & IN_COMPLETED) != 0);

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
            if((iter_->second.first._requestStateFlags & PARAMS_COMPLETED) != 0 && 
               (iter_->second.first._requestStateFlags & IN_COMPLETED) != 0)
            {
                _requestReadyCallbackFn(iter_->first,iter_->second.second);
            }
        }


        RequestReadyCallbackFn           _requestReadyCallbackFn;
        EmptyCallbackFn                  _emptyCallbackFn;
        RequestMap                       _requestMap;
    };
}}

#endif
