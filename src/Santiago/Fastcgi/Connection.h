#ifndef FASTCGI_CONNECTION_H
#define FASTCGI_CONNECTION_H

/**
 * @file Connection.h
 *
 * @section DESCRIPTION
 *
 * Contains the Connection class.
 */

#include <functional>

#include <boost/asio.hpp>

#include "Common.h"
#include "ConnectionData.h"
#include "RecordSocket.h"

namespace Santiago{ namespace Fastcgi
{

/**
 * Represents 1 connection. Handles all the requests multiplexed across a connection
 * It takes the boost asio protocol tags as the template 
 * arguments(eg Acceptor<boost::asio::ip::tcp>)
 */
    template<typename Protocol>
    class Connection:public std::enable_shared_from_this<Connection<Protocol> >
    {
        typedef std::shared_ptr<boost::asio::strand> StrandPtr;
        typedef typename Protocol::socket ProtocolSocket;
        typedef std::shared_ptr<ProtocolSocket> ProtocolSocketPtr;

        typedef std::shared_ptr<RequestData> RequestDataPtr;

        typedef std::function<void(unsigned,RequestDataPtr)> NewRequestCallbackFn;
        typedef std::function<void()> CloseCallbackFn;

        enum State
        {
            OPEN,
            CLOSING,
            CLOSED
        };

    public:
        /**
         * The constructor
         * @param ioService_- the ioservice to use
         * @param endpoint_- the endpoint to listen for new connection
         * @param newRequestCallbackFn - the function to call on receiving new request
         * @param closeCallbackFn
         */
        Connection(boost::asio::io_service &ioService_,
                   ProtocolSocketPtr protocolSocketPtr_,
                   NewRequestCallbackFn newRequestCallbackFn_,
                   CloseCallbackFn closeCallbackFn_):
            _newRequestCallbackFn(newRequestCallbackFn_),
            _closeCallbackFn(closeCallbackFn_),
            _data(std::bind(&Connection::handleRequestReady,this,std::placeholders::_1,std::placeholders::_2),
                  std::bind(&Connection::handleOnEmpty,this)),
            _ioService(ioService_),
            _strandPtr(new boost::asio::strand(ioService_)),
            _recordSocket(_strandPtr,
                          protocolSocketPtr_,
                          std::bind(&ConnectionData::handleBeginRequest,&_data,std::placeholders::_1),
                          std::bind(&ConnectionData::handleAbortRequest,&_data,std::placeholders::_1),
                          std::bind(&ConnectionData::handleStdin,
                                    &_data,std::placeholders::_1,
                                    std::placeholders::_2,
                                    std::placeholders::_3),
                          std::bind(&ConnectionData::handleParams,
                                    &_data,
                                    std::placeholders::_1,
                                    std::placeholders::_2,
                                    std::placeholders::_3),
                          std::bind(&Connection::handleTransceiverEvent,this,std::placeholders::_1)),
            _state(OPEN)
        {}

        
        /**
         * Called by the request to commit reply. Called in the request's strand.
         * So chain it with the commitReplyImpl
         * @param - requestId
         * @param - requestData
         */
        void commitReply(unsigned requestId_,RequestDataPtr requestData_)
        {
            ST_LOG_DEBUG("commitReply called."<<std::endl);
            _strandPtr->post(std::bind(&Connection::commitReplyImpl,
                                       this->shared_from_this(),
                                       requestId_,
                                       requestData_));
        }

        /**
         * start the socket-called by the acceptor class in acceptor strand.
         */
        void start()
        {
            _recordSocket.start();
        }

        /**
         * The destructor
         */
        ~Connection()
        {}

    private:

        /**
         * Called by connectiondata when a request becomes ready for user processing.
         * Always called in the connection's strand
         * @param requestId_
         * @param newRequest_
         */
        void handleRequestReady(unsigned requestId_,RequestDataPtr newRequest_)
        {
//            BOOST_ASSERT(_state == OPEN);
            ST_LOG_DEBUG("New request ready to be processed. requestId_ ="<<requestId_<<std::endl);
            _newRequestCallbackFn(requestId_,newRequest_);
//            _ioService.post(boost::bind(_newRequestCallbackFn,requestId_,newRequest_)); 
        }

        /**
         * For handling events like disconnect, last request.
         * Always called in the connection's strand
         * @param eventInfo_
         */
        void handleTransceiverEvent(TransceiverEventInfo eventInfo_)
        {
            ST_ASSERT(_state != CLOSED);

            if(eventInfo_ == SOCKET_CLOSED)
            {
                ST_LOG_DEBUG("Received SOCKET_CLOSED transceiverEvent."<<std::endl);
                closeAndCleanup(); 
            }
            else if(eventInfo_ == CONNECTION_WIND_DOWN)
            {
                ST_LOG_DEBUG("Received CONNECTION_WIND_DOWN transceiverEvent."<<std::endl);
                _state = CLOSING;
                if(_data.size() == 0)
                {
                    ST_LOG_DEBUG("Since no more requests..closing connection."<<std::endl);
                    closeAndCleanup();
                }
            }
            else
            {
                ST_ASSERT(false);
            }
        }

        /**
         * Called when the connectiondata becomes empty
         * Always called in the connection's strand 
         */
        void handleOnEmpty()
        {
            ST_LOG_DEBUG("In handleOnEmpty callback."<<std::endl);
            if(_state == CLOSING)
            {
                ST_LOG_DEBUG("Closing since CONNECTION_WIND_DOWN already received."<<std::endl);
                closeAndCleanup();
            }
        }

        /**
         * Closes the socket,clears the data and calls close callbackfn
         */
        void closeAndCleanup()
        {
            ST_LOG_DEBUG("In closeAndCleanup()"<<std::endl);
            _data.clear(); //note: clear() does not callback the dataEmptyHandler.but cleanupRequest() does.            
            _recordSocket.close();
            _state = CLOSED;
            _closeCallbackFn();
        }

        /**
         * Chained by  commitReply fn. Called in the connections's strand.
         * @param - requestId
         * @param - requestData
         */
        void commitReplyImpl(unsigned requestId_,RequestDataPtr requestData_)
        {
            ST_LOG_DEBUG("In commitReplyImpl"<<std::endl);
            if(!_data.isValidRequest(requestId_,requestData_))
            {
                ST_LOG_INFO("Unable to commitReply. Request already closed in between."<<std::endl);
                return;
            }

            boost::asio::streambuf httpHeaderOutBuffer;
            std::ostream httpHeaderOut(&httpHeaderOutBuffer);
            requestData_->fillHTTPHeaderData(httpHeaderOut);

            std::error_code ec;
           _recordSocket.sendReply(requestId_,
                                   httpHeaderOutBuffer,
                                   requestData_->_outBuffer,
                                   requestData_->_errBuffer,
                                   requestData_->_appStatus,
                                    ec);
            if(!ec) 
            {
                ST_LOG_DEBUG("sendReply succeeded. Cleaning up request. requestId_="<<requestId_<<std::endl);
                _data.cleanupRequest(requestId_);
            }
            else
            {
                ST_LOG_INFO("sendReply failed. Cleaning up all requests and closing connection"<<std::endl);
                closeAndCleanup();
            }
        }  
      
        NewRequestCallbackFn           _newRequestCallbackFn;
        CloseCallbackFn                _closeCallbackFn;

        ConnectionData                 _data;
        boost::asio::io_service       &_ioService;
        StrandPtr                      _strandPtr;
        RecordSocket<Protocol>         _recordSocket;

        State                          _state;
    };

}}

#endif
