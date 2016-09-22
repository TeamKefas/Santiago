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

#ifndef SANTIAGO_SERVER_SERVER_H
#define SANTIAGO_SERVER_SERVER_H
/**
 * @file RequestHandler.h
 * @author  Wilson Sunny Philip
 *
 * @section DESCRIPTION
 *
 * Contains the Server class. 
 */

#include <stdexcept>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include "Santiago/LocalEndpoint.h"
#include "Santiago/Fastcgi/Acceptor.h"

#include "RequestHandler.h"
#define NO_OF_ASIO_USER_THREADS 5

namespace Santiago{ namespace Server
{
/**
 * The class which will be used as the base class of all Servers. Is an abstract class
 * with the route() fnn left to the users for implementation. 
 * It takes the boost asio protocol tags as the template argument. 
 */
    template<typename Protocol>
    class Server
    {
        typedef boost::shared_ptr<RequestHandler<Protocol> > RequestHandlerPtr;
        typedef typename Request<Protocol>::RequestId RequestId;
        typedef boost::shared_ptr<Fastcgi::Request<Protocol> > FastcgiRequestPtr;
        typedef boost::shared_ptr<Request<Protocol> > RequestPtr;
        typedef boost::shared_ptr<boost::thread> ThreadPtr;

        enum Status
        {
            STARTED,
            STOPPED
        };

    public:

        /**
         * The contructor.
         * @param localEndpoint_- the endpoint to listen to.
         */
        Server(LocalEndpoint<Protocol> listenEndpoint_):
            _status(STOPPED),
            _acceptor(_ioService,listenEndpoint_,boost::bind(&Server::handleNewRequest,this,_1))
        {
            for(unsigned i=0;i<NO_OF_ASIO_USER_THREADS;i++)
            {
                _threads.push_back(ThreadPtr());
            }
            
        }

        /**
         * The route function. Called by the server on receiving a request. The child 
         * class must implement this function.
         * @param requestParams- the fastcgi params passed by the webserver.
         * @return  A RequestHandlerPtr. 
         */
        virtual RequestHandlerPtr route(std::map<std::string,std::string>& requestParams_) = 0;

        /**
         * Starts the server and its threads.
         */
        void start()
        {
            if(_status == STARTED)
            {
                throw std::runtime_error("Server already started");
            }

            _status = STARTED;
            for(unsigned i=0;i<NO_OF_ASIO_USER_THREADS;i++)
            {
                _threads[i].reset(new boost::thread(boost::bind(&boost::asio::io_service::run,&_ioService)));
            }
//            _ioService.run();
        }

        /**
         * Stops the server and its threads
         */
        void stop()
        {
            if(_status == STOPPED)
            {
                throw std::runtime_error("Server already stopped");
            }            
            _status = STOPPED;
            _ioService.stop();
            for(unsigned i=0;i<NO_OF_ASIO_USER_THREADS;i++)
            {
                _threads[i]->join();
            }
        }

        /**
         * The destructor
         */
        virtual ~Server()
        {
            if(_status == STOPPED)
            {
                return;
            }
            else
            {
                stop();
            }
        }

    protected:

        /**
         * Gets the underlying boost io_service.
         */
        boost::asio::io_service& getIOService()
        {
            return _ioService;
        }

    private:

        /**
         * The callback function for Fastcgi::Acceptor class. Called in the Acceptor's
         * thread.
         * @param fastcgiRequest_
         */
        void handleNewRequest(FastcgiRequestPtr fastcgiRequest_)
        {
            //init the request handler
            RequestHandlerPtr requestHandler = route(fastcgiRequest_->getFCGIParams());
            if(requestHandler == NULL)
            {
                return;
            }
            requestHandler->init(_ioService);

            boost::function<void()> onRequestDeleteCallbackFn = boost::bind(&Server::handleRequestDelete,this,fastcgiRequest_->getId());

            RequestPtr request(new Request<Protocol>(fastcgiRequest_,onRequestDeleteCallbackFn));
            //store the request in the active requests
            typename std::map<RequestId,RequestHandlerPtr>::iterator iter = _activeRequestHandlers.find(fastcgiRequest_->getId());
            BOOST_ASSERT(iter == _activeRequestHandlers.end());
            _activeRequestHandlers[fastcgiRequest_->getId()] =  requestHandler;
            
            requestHandler->getStrand().post(boost::bind(&RequestHandler<Protocol>::handleRequest,requestHandler,request));
        }

        /**
         * The callback function called by the Server::Request destuctor. Queues up the
         * RequestHandler for destruction in the server's thread.
         * @param requestId_
         */
        void handleRequestDelete(const RequestId& requestId_)
        {            
            if(_status != STOPPED)
            {
                _acceptor.getStrand().post(boost::bind(&Server::handleRequestDeleteImpl,this,requestId_));
            }
        }

        /**
         * The server strand implementation that deletes a queued up requestHandler.
         */
        void handleRequestDeleteImpl(const RequestId& requestId_)
        {
            typename std::map<RequestId,RequestHandlerPtr>::iterator iter = _activeRequestHandlers.find(requestId_);
            BOOST_ASSERT(iter != _activeRequestHandlers.end());
            _activeRequestHandlers.erase(iter);
        }

        Status                                          _status;
        boost::asio::io_service                         _ioService;
        Fastcgi::Acceptor<Protocol>                     _acceptor;
        typename std::map<RequestId,RequestHandlerPtr>  _activeRequestHandlers;
        std::vector<ThreadPtr>                          _threads;

    };

}}

#endif
