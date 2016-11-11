#ifndef SANTIAGO_APPSERVER_REQUEST_H
#define SANTIAGO_APPSERVER_REQUEST_H

/**
 * @file AppServer/Request.h
 *
 * @section DESCRIPTION
 *
 * Contains the AppServer::Request class
 * webserver.
 */

#include <functional>

#include "../Fastcgi/Acceptor.h"

namespace Santiago{ namespace AppServer
{

/**
 * The class which will be used by the users of the Server module to read and write to
 * a server request.It takes the boost asio protocol tags as the template argument. 
 * This class is more or less a wrapper over the Request in Fastcgi module.
 * This class object will be created by the Server class and not directly by the user.
 */
    template <typename Protocol>
    class Request
    {
        typedef std::shared_ptr<Fastcgi::Request<Protocol> > FastcgiRequestPtr;

    public:
        typedef typename Fastcgi::Request<Protocol>::RequestId RequestId;
        typedef typename std::shared_ptr<Request> Ptr;

        /**
         * Deleting the copy constructor to make class non copyable
         */
        Request(const Request&) = delete;

        /**
         * Deleting the copy assignment operator to make class non copyable
         */
        Request& operator = (const Request&) = delete;

        /**
         * The Constructor
         * @param fastcgiRequest_ - the Fastcgi::RequestPtr
         * @param onDeleteCallbackFn - the callback function to call in the destructor.
         * this function queues the request's corresponding requesthandler for removal
         * from the server in the server's strand.  
         */
        Request(const FastcgiRequestPtr& fastcgiRequest_,const std::function<void()>& onCompletedCallbackFn_):
            _fastcgiRequest(fastcgiRequest_),
            _onCompletedCallbackFn(onCompletedCallbackFn_)
        {}

        /**
         * Returns the requestId which is a pair of connectionId and RequestId.
         */
        RequestId getId() const
        {
            return _fastcgiRequest->getId();
        }


        /**
         * Tells if the request is still active.
         * @return true if the request is still valid
         */
        bool isValid() const
        {
            return _fastcgiRequest->isValid();
        }

        /**
         * returns the stdin buffer
         */
        const std::string& getStdinBuffer() const
        {
            return _fastcgiRequest->getStdinBuffer();
        }

        /**
         * returns the parameter map
         */
        const std::map<std::string,std::string>& getFCGIParams() const
        {
            return _fastcgiRequest->getFCGIParams();
        }

        /**
         * returns the get name value pairs.
         */
        const std::map<std::string,std::string>& getGetData() const
        {
            return _fastcgiRequest->getGetData();
        }
        

        /**
         * returns the post name value pairs.
         */
        const std::map<std::string,std::string>& getPostData() const
        {
            return _fastcgiRequest->getPostData();
        }

        /**
         * returns the cookie name value params received from server.
         */
        const std::map<std::string,std::string>& getHTTPCookiesReceived() const
        {
            return _fastcgiRequest->getHTTPCookiesReceived();
        }

        /**
         * sets the content type to be sent to the user. Default Html
         */
        void setContentMIMEType(MIMEType contentType_)
        {
            _fastcgiRequest->setContentMIMEType(contentType_);
        }

        /**
         * returns cookies to be sent to the user.
         */
        std::set<HTTPCookieData>& responseHTTPCookies()
        {
            return _fastcgiRequest->responseHTTPCookies();
        }

        /**
         * the error buffer of the request
         */
        std::ostream& err()
        {
            return _fastcgiRequest->err();
        }

        /**
         * the out buffer of the request
         */
        std::ostream& out()
        {
            return _fastcgiRequest->out();
        }

        /**
         * Sets the appstatus to be replied
         * @param status_
         */
        void setAppStatus(uint status_)
        {
            _fastcgiRequest->setAppStatus(status_);
        }

        /**
         * Commits the data in the request and closes the request.Can be called from 
         * any thread as Fastcgi::Request::Commit is threadsafe.
         * @param error code- sets to INVALID_FASTCGI_REQUEST if the request not valid
         */
        void commit(std::error_code& error_)
        {
            error_ = std::error_code(ERR_SUCCESS,ErrorCategory::GetInstance());
            _fastcgiRequest->commit(error_);
            _onCompletedCallbackFn();
        }

        /**
         * Cancels the data in the request and closes the request.Can be called from 
         * any thread as Fastcgi::Request::Cancel is threadsafe.
         * @param error code- sets to INVALID_FASTCGI_REQUEST if the request not valid
         */
        void cancel(std::error_code& error_)
        {
            error_ = std::error_code(ERR_SUCCESS,ErrorCategory::GetInstance());
            _fastcgiRequest->cancel(error_);
            _onCompletedCallbackFn();
        }

        ~Request()
        {
            if(isValid())
            {
                std::error_code error;
                cancel(error);
            }
        }


    private:

        FastcgiRequestPtr            _fastcgiRequest;
        std::function<void()>        _onCompletedCallbackFn;
    };
}}
#endif
