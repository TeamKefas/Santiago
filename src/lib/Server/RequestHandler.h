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

#ifndef SANTIAGO_SERVER_REQUEST_HANDLER_H
#define SANTIAGO_SERVER_REQUEST_HANDLER_H

/**
 * @file RequestHandler.h
 * @author  Wilson Sunny Philip
 *
 * @section DESCRIPTION
 *
 * Contains the RequestHandler class. 
 */

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>

#include "Request.h"

namespace Santiago{ namespace Server
{
    template<typename Protocol>
    class Server;

/**
 * The class which will be used as the base class of all RequestHandlers. Is an abstract
 * class with the handleRequest fn left to the users for implementation. 
 * It takes the boost asio protocol tags as the template argument. 
 */
    template<typename Protocol>
    struct RequestHandler:public boost::noncopyable
    {
        typedef boost::shared_ptr<boost::asio::strand> StrandPtr;        

    public:
        typedef typename Request<Protocol>::Ptr RequestPtr;
        typedef boost::shared_ptr<RequestHandler<Protocol> > Ptr;

        /**
         * The constructor
         */
        RequestHandler()
        {}
        
        /**
         * The destructor
         */
        virtual ~RequestHandler()
        {}

        /**
         * Returns the strand of the handler. Each of the handler has a strand and the
         * initial requestHandler() is called in this strand. This is provided for the
         * users to implement asynchronous calls using boost asio if the callbacks are
         * to  happen in a single thread (i.e. not happen simultaneously).
         */
        boost::asio::strand& getStrand()
        {
            checkIsInitialized();
            return *_strand;
        }

        /**
         * Returns the underlying io_service
         */
        boost::asio::io_service& getIOService()
        {
            checkIsInitialized();
            return _strand->get_io_service();
        }

        /**
         * Posts a callback function in the RequestHandler's strand.
         * @param callbackFn_- the callback fn for the post request.
         */
        void postInStrand(const boost::function<void()>& callbackFn_)
        {
            checkIsInitialized();
            _strand->post(boost::bind(&RequestHandler::handlePostInStrand,this,callbackFn_));           
        }
        
        /**
         * The function that is to be overloaded by the  user request handler's. This
         * function will be called by the server in the RequestHandler's strand.
         * @param request_ - the info about the request.
         */
        virtual void handleRequest(RequestPtr request_) = 0;       

    private:

        /**
         * The callback function for the postInStrand()
         */
        void handlePostInStrand(const boost::function<void()>& callbackFn_)
        {
            callbackFn_();
        }

        /**
         * Check if the RequestHandler was initialized and throw exception if not
         * initialized.
         */
        void checkIsInitialized()
        {
            if(_strand == NULL)
            {
                throw std::runtime_error("Handler not initialized");
            }
        }

        /**
         * Will be called by the server class to initialize the RequestHandler.
         * @param - ioService.
         */
        void init(boost::asio::io_service& ioService_)
        {
            _strand.reset(new boost::asio::strand(ioService_));
        }

        friend class Server<Protocol>;

        StrandPtr                 _strand;
    };

}}
#endif
