#include "ConnectionRequestsController.h"

namespace Santiago{ namespace Authentication{ namespace MultiNode
{
    ConnectionRequestsController::
    ConnectionRequestsController(boost::asio::io_service& ioService_,
                                 const StrandPtr& strandPtr_,
                                 const boost::asio::ip::tcp::endpoint& endpoint_,
                                 const OnDisconnectCallbackFn& onDisconnectCallbackFn_,
                                 const OnServerRequestMessageCallbackFn& onServerRequestMessageCallbackFn_)
        :_ioService(ioService_)
        ,_strandPtr(strandPtr_)
        ,_endpoint(endpoint_)
        ,_onDisconnectCallbackFn(onDisconnectCallbackFn_)
        ,_onServerRequestMessageCallbackFn(onServerRequestMessageCallbackFn_)
    {
        createAndInitializeConnectionMessageSocket();
    }

    void ConnectionRequestsController::
    sendMessage(const ConnectionMessage& message_,
                bool isReplyExpectingMessage_,
                const boost::optional<OnMessageCallbackFn>& onReplyMessageCallbackFn_)
    {
        if(!_connectionMessageSocketOpt && isReplyExpectingMessage_)
        {
            onReplyMessageCallbackFn_(std::error_code(ErrorCode::ERR_AUTH_SERVER_CONNECTION_ERROR,
                                                      ErrorCategory::GetInstance()),
                                      boost::none);
            return;
        }

        if(isReplyExpectingMessage_)
        {
            ST_ASSERT(_requestIdCallbackFnMap.find(message_._requestId) == _requestIdCallbackFnMap.end());
            _requestIdCallbackFnMap[message_._requestId] = onReplyMessageCallbackFn_;
        }
        
        _connectionMessageSocketOpt->sendMessage(message_);
    }

    void ConnectionRequestsController::
    createAndInitializeConnectionMessageSocket()
    {
        ST_ASSERT(_connectionMessageSocketOpt);
        std::shared_ptr<boost::asio::ip::tcp::socket> socketPtr(new boost::asio::ip::tcp::socket(_ioService));
        
        boost::system::error_code error;
        socketPtr->connect(_endpoint,error);
        if(error)
        {
            ST_ERROR("Unable to connect to Auth server\n");
            asyncConnectAgainAfterDelay();
            return;
        }

        char inputArray[4];
        boost::asio::read(*socketPtr, boost::asio::buffer(inputArray, 4), error); 
        if(error)
        {
            ST_ERROR("Unable to connect to Auth server\n");
            asyncConnectAgainAfterDelay();           
            return;
        }
        
        _connectionId = reinterpret_cast<unsigned>(inputArray);
        _connectionMessageSocketOpt.reset(
            ConnectionMessageSocket(socketPtr,
                                    std::bind(&ConnectionMessageSocket::handleConnectionDisconnect,
                                              this),
                                    std::bind(&ConnectionMessageSocket::handleConnectionMessage,
                                              this, std::placeholders::_1),
                                    _strandPtr,
                                    _strandPtr));

        return;
    }

    void ConnectionRequestsController::queueConnectAfterDelay()
    {
        std::shared_ptr<boost::asio::basic_deadline_timer> timerPtr(_ioService, boost::duration_time(120)); //TODO: make sure the duration is 2 mins.
        timerPtr->async_wait([timerPtr,this]() //TODO: Confirm timerPtr won't be destroyed till it reaches here
                             {
                                 this->createAndInitializeConnectionMessageSocket();
                             });
    }

    void ConnectionRequestsController::
    furtherHandleNewConnectionMessage(const ConnectionMessage& message_)
    {
        _onServerRequestMessageCallbackFn(message_);
        return;
    }

    void ConnectionRequestsController::
    furtherHandleReplyConnectionMessage(const ConnectionMessage& message_)
    {
        std::map<RequestId,OnMessageCallbackFn>::iterator iter =
            _requestIdCallbackFnMap.find(message_._requestId);
        ST_ASSERT(iter != _requestIdCallbackFnMap.end());
            
        _requestIdCallbackFnMap[requestId_](std::error_code(ErrorCode::ERR_SUCCESS,
                                                            ErrorCategory::GetInstance()),
                                            message_);
        _requestIdCallbackFnMap.erase(iter);//TODO: For client there will always be only 1 reply..so directly delete        
        return;
    }

    void ConnectionRequestsController::
    furtherHandleConnectionDisconnectForReplyExpectingRequest(const RequestId& requestId_)
    {
        std::map<RequestId,OnMessageCallbackFn>::iterator iter =
            _requestIdCallbackFnMap.find(message_._requestId);
        ST_ASSERT(iter != _requestIdCallbackFnMap.end());
            
        _requestIdCallbackFnMap[requestId_](std::error_code(ErrorCode::ERR_AUTH_SERVER_CONNECTION_ERROR,
                                                            ErrorCategory::GetInstance()),
                                            boost::none);
        _requestIdCallbackFnMap.erase(iter);
        return;        
    }
    
    void ConnectionRequestsController::furtherHandleConnectionMessageSocketDisconnect()
    {
        ST_ASSERT(_requestIdCallbackFnMap.empty());
        _connectionMessageSocketOpt = boost::none;
        _onDisconnectCallbackFn();
        createAndInitializeConnectionMessageSocket(_endpoint);
    }

    ConnectionMessageSocket& ConnectionRequestsController::getConnectionMessageSocket()
    {
        return _connectionMessageSocket;
    }
}}}
