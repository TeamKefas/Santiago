#ifndef SANTIAGO_APPSERVER_SERVERBASE_H
#define SANTIAGO_APPSERVER_SERVERBASE_H
/**
 * @file RequestHandler.h
 *
 * @section DESCRIPTION
 *
 * Contains the ServerBase class. 
 */

#include <functional>
#include <stdexcept>
#include <thread>

#include <boost/asio.hpp>

#include "../LocalEndpoint.h"
#include "../Fastcgi/Acceptor.h"

#include "RequestHandler.h"
#define NO_OF_ASIO_USER_THREADS 5

namespace Santiago{ namespace AppServer
{
/**
 * The class which will be used as the base class of all Servers. Is an abstract class
 * with the route() fnn left to the users for implementation. 
 * It takes the boost asio protocol tags as the template argument. 
 */
    template<typename Protocol>
    class ServerBase
    {
        typedef std::shared_ptr<RequestHandler<Protocol> > RequestHandlerPtr;
        typedef typename Request<Protocol>::RequestId RequestId;
        typedef std::shared_ptr<Fastcgi::Request<Protocol> > FastcgiRequestPtr;
        typedef std::shared_ptr<Request<Protocol> > RequestPtr;
        typedef std::shared_ptr<std::thread> ThreadPtr;

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
        ServerBase(LocalEndpoint<Protocol> listenEndpoint_):
            _status(STOPPED),
            _acceptor(_ioService,listenEndpoint_,std::bind(&ServerBase::handleNewRequest,this,_1))
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
                _threads[i].reset(new std::thread(std::bind(&boost::asio::io_service::run,&_ioService)));
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
        virtual ~ServerBase()
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

            std::function<void()> onRequestCompleteCallbackFn =
                std::bind(&ServerBase::handleRequestComplete,this,fastcgiRequest_->getId());

            RequestPtr request(new Request<Protocol>(fastcgiRequest_,onRequestCompleteCallbackFn));
            //store the request in the active requests
            typename std::map<RequestId,RequestHandlerPtr>::iterator iter =
                _activeRequestHandlers.find(fastcgiRequest_->getId());
            BOOST_ASSERT(iter == _activeRequestHandlers.end());
            _activeRequestHandlers[fastcgiRequest_->getId()] =  requestHandler;
            
            requestHandler->getStrand().post(std::bind(&RequestHandler<Protocol>::handleRequest,requestHandler,request));
        }

        /**
         * The callback function called by the Server::Request commit/cancel. Queues up the
         * RequestHandler for removal from the activRequestHandlers list in the server's thread.
         * @param requestId_
         */
        void handleRequestComplete(const RequestId& requestId_)
        {            
            if(_status != STOPPED)
            {
                _acceptor.getStrand().post(std::bind(&ServerBase::handleRequestCompleteImpl,this,requestId_));
            }
        }

        /**
         * The server strand implementation that removes a queued up requestHandler from the
         * activeRequestHandlers list.
         */
        void handleRequestCompleteImpl(const RequestId& requestId_)
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
