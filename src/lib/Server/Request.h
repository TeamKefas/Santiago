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

#ifndef SANTIAGO_SERVER_REQUEST_H
#define SANTIAGO_SERVER_REQUEST_H

/**
 * @file Server/Request.h
 * @author  Wilson Sunny Philip
 *
 * @section DESCRIPTION
 *
 * Contains the Server::Request class
 * webserver.
 */

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>

#include "Santiago/Fastcgi/Acceptor.h"

namespace Santiago{ namespace Server
{

/**
 * The class which will be used by the users of the Server module to read and write to
 * a server request.It takes the boost asio protocol tags as the template argument. 
 * This class is more or less a wrapper over the Request in Fastcgi module.
 * This class object will be created by the Server class and not directly by the user.
 */
    template <typename Protocol>
    class Request:public boost::noncopyable
    {
        typedef boost::shared_ptr<Fastcgi::Request<Protocol> > FastcgiRequestPtr;

    public:
        typedef typename Fastcgi::Request<Protocol>::RequestId RequestId;
        typedef typename boost::shared_ptr<Request> Ptr;

        /**
         * The Constructor
         * @param fastcgiRequest_ - the Fastcgi::RequestPtr
         * @param onDeleteCallbackFn - the callback function to call in the destructor.
         * this function queues the request's corresponding requesthandler for removal
         * from the server in the server's strand.  
         */
        Request(FastcgiRequestPtr fastcgiRequest_,boost::function<void()> onDeleteCallbackFn_):
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
        boost::function<void()>      _onDeleteCallbackFn;
    };
}}
#endif
