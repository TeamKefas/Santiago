#ifndef SANTIAGO_APPSERVER_REQUEST_H
#define SANTIAGO_APPSERVER_REQUEST_H

/**
 * @file Server/Request.h
 *
 * @section DESCRIPTION
 *
 * Contains the Server::Request class
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
        Request& operator(const Request&) = delete;

        /**
         * The Constructor
         * @param fastcgiRequest_ - the Fastcgi::RequestPtr
         * @param onDeleteCallbackFn - the callback function to call in the destructor.
         * this function queues the request's corresponding requesthandler for removal
         * from the server in the server's strand.  
         */
        Request(FastcgiRequestPtr fastcgiRequest_,std::function<void()> onDeleteCallbackFn_):
            _fastcgiRequest(fastcgiRequest_),
            _onDeleteCallbackFn(onDeleteCallbackFn_)
        {}

        /**
         * The Destructor
         */
        ~Request()
        {
            _onDeleteCallbackFn();
        }

        /**
         * returns the stdin buffer
         */
        const std::string& getStdinBuffer()
        {
            return _fastcgiRequest->getStdinBuffer();
        }

        /**
         * returns the parameter map
         */
        std::map<std::string,std::string>& getFCGIParams()
        {
            return _fastcgiRequest->getFCGIParams();
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
         */
        void commit()
        {
            _fastcgiRequest->commit();
        }

        /**
         * Cancels the data in the request and closes the request.Can be called from 
         * any thread as Fastcgi::Request::Cancel is threadsafe.
         */
        void cancel()
        {
            _fastcgiRequest->cancel();
        }

        /**
         * Returns the requestId.
         */
        std::pair<unsigned,unsigned> getID()
        {
            return _fastcgiRequest->getID();
        }

    private:

        FastcgiRequestPtr            _fastcgiRequest;
        std::function<void()>        _onDeleteCallbackFn;
    };
}}
#endif
