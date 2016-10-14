#ifndef FASTCGI_REQUEST_H
#define FASTCGI_REQUEST_H
/**
 * @file Fastcgi/Request.h
 *
 * @section DESCRIPTION
 *
 * Contains the Fastcgi::Request class.
 */

#include <mutex>
#include <stdexcept>
#include <thread>

#include <boost/asio.hpp>

#include "Error/ErrorCategory.h"

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
    class Request
    {
    public:
        typedef std::shared_ptr<RequestData> RequestDataPtr;

        typedef std::shared_ptr<Connection<Protocol> > ConnectionPtr;
        typedef std::weak_ptr<Connection<Protocol> > ConnectionWeakPtr;

        typedef std::shared_ptr<IsRequestAlive> IsRequestAlivePtr;
        typedef std::weak_ptr<IsRequestAlive> IsRequestAliveWeakPtr;

        typedef std::pair<unsigned,unsigned> RequestId;

        /**
         * Delete the copy constructor to make the class private
         */
        Request(const Request&) = delete;

        /**
         * Delete the copy assignment operator to make the class private
         */
        Request& operator=(const Request&) = delete;

        /**
         * The constructor
         * @param ioService_- the ioservice of the acceptor
         * @param requestId - the requestId of the request
         * @param dataPtr_- shared ptr to the data
         * @param connectionId - the id given to the connection by the acceptor
         * @param connectionWeakPtr - weak ptr to the connection
         */
        Request(boost::asio::io_service& ioService_,
                uint requestId_,
                RequestDataPtr dataPtr_,
                uint connectionId_,
                ConnectionWeakPtr connectionWeakPtr_):
            _requestId(requestId_),
            _dataPtr(dataPtr_),
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
            return ((!_hasReplied) && (_dataPtr->_isRequestAliveWeakPtr.lock() != NULL) && (_connectionWeakPtr.lock() != NULL));
        }

        /**
         * Commits the data in the request and closes the request.Can be called from 
         * any thread is thread safe. Please note that this function actually need not
         * be thread safe as it just post's the data in the connection's strand. Here
         * the multiple commits from different threads are weeded out. But making this
         * thread safe using lock will allow for giving error messages to the user on
         * multiple writes from multiple threads.
         * @param error code- sets to INVALID_FASTCGI_REQUEST if the request not valid
         */
        void commit(std::error_code& error_)
        {
            std::lock_guard<std::mutex> lock(_commitMutex);
            ConnectionPtr connectionPtr = checkRequestValidityAndGetConnectionPtr(error_);

            if(!error_)
            {
                connectionPtr->commitReply(_requestId,_dataPtr);
            }

            _dataPtr.reset();
            _hasReplied = true;
        }

        /**
         * Closes the request.Can be called from any thread is thread safe. Is similar
         * to commit except that all the data in the out buffer is emptied before
         * flushing. 
         */
        void cancel(std::error_code& error_)
        {
            std::lock_guard<std::mutex> lock(_commitMutex);
            ConnectionPtr connectionPtr = checkRequestValidityAndGetConnectionPtr(error_);

            if(!error_)
            {
                _dataPtr->_outBuffer.consume(_dataPtr->_outBuffer.size());
                _dataPtr->_err<<"Request cancelled by application server";
                connectionPtr->commitReply(_requestId,_dataPtr);
            }

            _dataPtr.reset();
            _hasReplied = true;
        }

        /**
         * Sets the appstatus to be replied
         * @param status_
         */
        void setAppStatus(uint status_)
        {
            _dataPtr->_appStatus = status_;
        }

        /**
         * returns the stdin buffer
         */
        const std::string& getStdinBuffer()
        {
            return _dataPtr->_in;
        }

        /**
         * returns the parameter map
         */
        RequestData::ParamsMap& getFCGIParams()
        {
            return _dataPtr->_paramsMap;
        }

        /**
         * returns the get name value pairs. //TODO---
         */
        const std::map<std::string,std::string>& getGetData() const
        {
            return _dataPtr->_requestGetData;
        }
        

        /**
         * returns the post name value pairs. //TODO
         */
        const std::map<std::string,std::string>& getPostData() const
        {
            return _dataPtr->_requestPostData;
        }

        /**
         * returns the cookie name value params received from server. //TODO
         */
        const std::map<std::string,std::string>& getHTTPCookiesReceived() const
        {
            return _dataPtr->_requestHTTPCookies;
        }

        /**
         * sets the content type to be sent to the user. Default Html
         */
        void setContentMIMEType(MIMEType contentType_)
        {
            _dataPtr->_responseContentType = contentType_;
        }

        /**
         * returns cookies to be sent to the user //TODO
         */
        std::set<HTTPCookieData>& responseHTTPCookies()
        {
            return _dataPtr->_responseHTTPCookies;
        }

        /**
         * the error buffer of the request
         */
        std::ostream& err()
        {
            return _dataPtr->_err;
        }

        /**
         * the out buffer of the request
         */
        std::ostream& out()
        {
            return _dataPtr->_out;
        }

       
        /**
         * The destructor
         */
        ~Request()
        {
            if(isValid())
            {
                std::error_code error;
                cancel(error);
            }
        }

    private:
        /**
         * Checks the validity of the request and Cnverts the connection ptrs to shared ptrs and returns. 
         * Inability to convert indicates that the connection has been disconnected.
         * @param error_ - can be INVALID_FASTCGI_REQUEST or FASTCGI_REQUEST_ALREADY_REPLIED
         * @return - the connection shared_ptr
         */
        ConnectionPtr checkRequestValidityAndGetConnectionPtr(std::error_code& error_)
        {
            if(_hasReplied)
            {
                error_ = std::error_code(Error::FASTCGI_REQUEST_ALREADY_REPLIED, Error::ErrorCategory::GetInstance());
                return ConnectionPtr();
            }

            ConnectionPtr ret(_connectionWeakPtr.lock());
            if(ret == NULL)
            {
                error_ = std::error_code(Error::INVALID_FASTCGI_REQUEST, Error::ErrorCategory::GetInstance());
            }
            return ret;
        }

        std::mutex               _commitMutex;
        uint                     _requestId;
        RequestDataPtr           _dataPtr;
        uint                     _connectionId;
        ConnectionWeakPtr        _connectionWeakPtr;
        bool                     _hasReplied;

    };    
}}

#endif
