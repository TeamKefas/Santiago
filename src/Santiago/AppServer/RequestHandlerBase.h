#ifndef SANTIAGO_APPSERVER_REQUESTHANDLERBASE_H
#define SANTIAGO_APPSERVER_REQUESTHANDLERBASE_H

/**
 * @file RequestHandler.h
 *
 * @section DESCRIPTION
 *
 * Contains the RequestHandlerBase class. 
 */

#include <functional>

#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>

#include "Request.h"

namespace Santiago{ namespace AppServer
{
    template<typename Protocol>
    class Server;

/**
 * The class which will be used as the base class of all RequestHandlers. Is an abstract
 * class with the handleRequest fn left to the users for implementation. 
 * It takes the boost asio protocol tags as the template argument. 
 */
    template<typename Protocol>
    struct RequestHandlerBase:public std::enable_shared_from_this<RequestHandlerBase<Protocol> >
    {
        typedef std::shared_ptr<AsioStrand> StrandPtr;        

    public:
        typedef typename Request<Protocol>::Ptr RequestPtr;
        typedef std::shared_ptr<RequestHandlerBase<Protocol> > Ptr;

        /**
         * Deleting the copy constructor to make class non copyable
         */
        RequestHandlerBase(const RequestHandlerBase&) = delete;

        /**
         * Deleting the copy assignment error to make class non copyable
         */
        RequestHandlerBase& operator=(const RequestHandlerBase&) = delete;

        /**
         * The constructor
         */
        RequestHandlerBase()
        {}
        
        /**
         * The destructor
         */
        virtual ~RequestHandlerBase()
        {}

        /**
         * The function that is to be overloaded by the  user request handler's. This
         * function will be called by the server in the RequestHandlerBase's strand.
         * @param request_ - the info about the request.
         */
        virtual void handleRequest(const RequestPtr& request_, boost::asio::yield_context yield_) = 0;


        /**
         * Returns the strand of the handler. Each of the handler has a strand and the
         * initial requestHandler() is called in this strand. This is provided for the
         * users to implement asynchronous calls using boost asio if the callbacks are
         * to  happen in a single thread (i.e. not happen simultaneously).
         * Caution: The server stores a shared_ptr to each handler. This shared_ptr is 
         * destroyed when the request_ object received in the handleRequest is destroyed
         * or request_->commit()/cancel() is called. So if you are using the RequestHandler
         * past these 3 situation by posting to strand/io_service please use the 
         * this->shared_from_this() to keep the request alive. If not sure then use
         * postInStrand() fn. postInStrand() fn will keep the handler object alive.
         */
        AsioStrand& getStrand()
        {
            checkIsInitialized();
            return *_strand;
        }

         /**
         * Will be called by the server class to initialize the RequestHandlerBase.
         * @param - ioService.
         */
        void init(boost::asio::io_service& ioService_)
        {
            _strand.reset(new AsioStrand(ioService_));
        }

    protected:

        /**
         * Returns a std::shared_ptr<T> that shares ownership of *this with all existing
         * std::shared_ptr that refer to *this (similar to std::shared_from_this())
         * Rationale: The server stores a shared_ptr<RequestHandlerBase> to each handler.
         * This shared_ptr is destroyed when the request_ object received in the 
         * handleRequest is destroyed or request_->commit()/cancel() is called. So if the
         * RequestHandler is used past these 3 situations for some cleanup by posting to
         * strand/io_service etc. this function can be used similar to how
         * std::shared_from_this() is used in typical boost asio case. (The design does not
         * support the direct use of std::shared_from_this to avoid having to make the
         * entire RequestHandler heirarchy template classes)
         */
        template<typename T>
        std::shared_ptr<T> sharedFromThis()
        {
            return std::static_pointer_cast<T>(this->shared_from_this());
        }

        /**
         * Returns the underlying io_service
         * Caution: The server stores a shared_ptr to each handler. This shared_ptr is 
         * destroyed when the request_ object received in the handleRequest is destroyed
         * or request_->commit()/cancel() is called. So if you are using the RequestHandler
         * past these 3 situations by posting to strand/io_service please use the 
         * this->shared_from_this() to keep the request alive. If not sure then use
         * postInStrand() fn. postInStrand() fn will keep the handler object alive.
         */
        boost::asio::io_service& getIOService()
        {
            checkIsInitialized();
            return _strand->get_io_service();
        }

        /**
         * Posts a callback function in the RequestHandlerBase's strand.
         * @param callbackFn_- the callback fn for the post request.
         */
        void postInStrand(const std::function<void()>& callbackFn_)
        {
            checkIsInitialized();
            _strand->post(std::bind(&RequestHandlerBase::handlePostInStrand,
                                    this->shared_from_this(),
                                    callbackFn_));           
        }

    private:
        /**
         * The callback function for the postInStrand()
         */
        void handlePostInStrand(const std::function<void()>& callbackFn_)
        {
            callbackFn_();
        }

        /**
         * Check if the RequestHandlerBase was initialized and throw exception if not
         * initialized.
         */
        void checkIsInitialized()
        {
            if(_strand == NULL)
            {
                BOOST_ASSERT(false); //this will happen only if user starts using the fns before the init is called
                throw std::runtime_error("Handler not initialized");
            }
        }


        friend class Server<Protocol>;

    protected:

        StrandPtr                 _strand;
    };

}}
#endif
