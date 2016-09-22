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
#ifndef FASTCGI_REQUEST_H
#define FASTCGI_REQUEST_H
/**
 * @file Fastcgi/Request.h
 * @author  Wilson Sunny Philip
 *
 * @section DESCRIPTION
 *
 * Contains the Fastcgi::Request class.
 */

#include <stdexcept>

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>

#include "Common.h"
#include "RequestData.h"
#include "Connection.h"

namespace Santiago{namespace Fastcgi
{
/**
 * The class which will be used by the users of the Fastcgi module to read and write to
 * a fastcgi request.It takes the boost asio protocol tags as the template argument. 
 * This class object will be created by the acceptor class and not directly by the user.
 */
    template<class Protocol>
    class Request:private boost::noncopyable
    {
    public:
        typedef boost::shared_ptr<RequestData> RequestDataPtr;
        typedef boost::weak_ptr<RequestData> RequestDataWeakPtr;

        typedef boost::shared_ptr<Connection<Protocol> > ConnectionPtr;
        typedef boost::weak_ptr<Connection<Protocol> > ConnectionWeakPtr;

        typedef std::pair<unsigned,unsigned> RequestId;
        /**
         * The constructor
         * @param ioService_- the ioservice of the acceptor
         * @param requestId - the requestId of the request
         * @param dataWeakptr_- weak ptr to the data
         * @param connectionId - the id given to the connection by the acceptor
         * @param connectionWeakPtr - weak ptr to the connection
         */
        Request(boost::asio::io_service& ioService_,
                uint requestId_,
                RequestDataWeakPtr dataWeakPtr_,
                uint connectionId_,
                ConnectionWeakPtr connectionWeakPtr_):
            _requestId(requestId_),
            _dataWeakPtr(dataWeakPtr_),
            _connectionId(connectionId_),
            _connectionWeakPtr(connectionWeakPtr_),
            _hasReplied(false)
        {}

        /**
         * Returns the requestId which is a pair of connectionId and RequestId.
         */
        RequestId getId()
        {
            return std::pair<uint,uint>(_connectionId,_requestId);
        }

        /**
         * Tells if the request and the connection has not be aborted/disconnected.
         * @return true if the request is still valid
         */
        bool isValid()
        {
            return ((!_hasReplied) && (_dataWeakPtr.lock() != NULL) && (_connectionWeakPtr.lock() != NULL));
        }

        /**
         * Commits the data in the request and closes the request.Can be called from 
         * any thread is thread safe. Please note that this function actually need not
         * be thread safe as it just post's the data in the connection's strand. Here
         * the multiple commits from different threads are weeded out. But making this
         * thread safe using lock will allow for giving error messages to the user on
         * multiple writes from multiple threads.
         */
        void commit()
        {
            boost::lock_guard<boost::mutex> lock(_commitMutex);
            std::pair<ConnectionPtr,RequestDataPtr> connectionRequestPair = getConnectionAndRequestData();
            connectionRequestPair.first->commitReply(_requestId,connectionRequestPair.second);            
            _dataWeakPtr.reset();
            _hasReplied = true;
        }

        /**
         * Closes the request.Can be called from any thread is thread safe. Is similar
         * to commit except that all the data in the out buffer is emptied before
         * flushing. 
         */
        void cancel()
        {
            boost::lock_guard<boost::mutex> lock(_commitMutex);
            std::pair<ConnectionPtr,RequestDataPtr> connectionRequestPair = getConnectionAndRequestData();
            connectionRequestPair.second->_outBuffer.consume(connectionRequestPair.second->_outBuffer.size());
            connectionRequestPair.second->_err<<"Request cancelled by application server";
            connectionRequestPair.first->commitReply(_requestId,connectionRequestPair.second);            
            _dataWeakPtr.reset();
            _hasReplied = true;
        }

        /**
         * Sets the appstatus to be replied
         * @param status_
         */
        void setAppStatus(uint status_)
        {
            getConnectionAndRequestData().second->_appStatus = status_;
        }

        /**
         * returns the stdin buffer
         */
        const std::string& getStdinBuffer()
        {
            return getConnectionAndRequestData().second->_in;
        }

        /**
         * returns the parameter map
         */
        RequestData::ParamsMap& getFCGIParams()
        {
            return getConnectionAndRequestData().second->_paramsMap;
        }

        /**
         * the error buffer of the request
         */
        std::ostream& err()
        {
            return getConnectionAndRequestData().second->_err;
        }

        /**
         * the out buffer of the request
         */
        std::ostream& out()
        {
            return getConnectionAndRequestData().second->_out;
        }

       
        /**
         * The destructor
         */
        ~Request()
        {
            try
            {
                if(isValid())
                {
                    cancel();
                }
            }
            catch(...)
            {
                BOOST_ASSERT(false);
            }
        }

    private:
        /**
         * Converts the connection and data weak ptrs to shared ptrs and returns. 
         * Inability to convert indicates that the request/connection has be
         * aborted/disconnected/flushed. An exception is called then which is passed 
         * unhandled to the user by the public functions which calls this functions.
         */
        std::pair<ConnectionPtr,RequestDataPtr> getConnectionAndRequestData()
        {
            if(_hasReplied)
            {
                throw std::runtime_error("The request has already been committed or cancelled");
            }
            std::pair<ConnectionPtr,RequestDataPtr> ret(_connectionWeakPtr.lock(),_dataWeakPtr.lock());
            if(ret.first == NULL || ret.second == NULL)
            {
                throw std::runtime_error("The request is not active or aborted");
            }
            return ret;
        }

        boost::mutex _commitMutex;
        uint _requestId;
        RequestDataWeakPtr _dataWeakPtr;
        uint _connectionId;
        ConnectionWeakPtr _connectionWeakPtr;
        bool _hasReplied;

    };    
}}

#endif
