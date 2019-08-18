#include "ConnectionRequestsController.h"

#include<boost/asio/deadline_timer.hpp>

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
                const boost::optional<OnReplyMessageCallbackFn>& onReplyMessageCallbackFn_)
    {
        OnReplyMessageCallbackFn onReplyMessageCallbackFn;
        if(onReplyMessageCallbackFn_)
        {
            onReplyMessageCallbackFn = *onReplyMessageCallbackFn_;
        }
        
        if(!_connectionMessageSocketPtr && isReplyExpectingMessage_)
        {
            onReplyMessageCallbackFn(std::error_code(ErrorCode::ERR_AUTH_SERVER_CONNECTION_ERROR),
                                      boost::none);
            return;
        }

        if(isReplyExpectingMessage_)
        {
            ST_ASSERT(_requestIdCallbackFnMap.find(message_._requestId) == _requestIdCallbackFnMap.end());
            _requestIdCallbackFnMap.insert(std::pair<RequestId,OnReplyMessageCallbackFn>(message_._requestId,onReplyMessageCallbackFn));
        }
        
        _connectionMessageSocketPtr->sendMessage(message_);
    }
    
    unsigned ConnectionRequestsController::getConnectionId()
    {
        return _connectionId;
    }
    
    void ConnectionRequestsController::
    createAndInitializeConnectionMessageSocket()
    {
        ST_ASSERT(_connectionMessageSocketPtr);
        std::shared_ptr<boost::asio::ip::tcp::socket> socketPtr(new boost::asio::ip::tcp::socket(_ioService));
        
        boost::system::error_code error;
        socketPtr->connect(_endpoint,error);
        if(error)
        {
            ST_LOG_ERROR("Unable to connect to Auth server\n");
            queueConnectAfterDelay();
            return;
        }

        char inputArray[4];
        boost::asio::read(*socketPtr, boost::asio::buffer(inputArray, 4), error); 
        if(error)
        {
            ST_LOG_ERROR("Unable to connect to Auth server\n");
            queueConnectAfterDelay();           
            return;
        }
        
        _connectionId = *(reinterpret_cast<unsigned*>(inputArray));
        _connectionMessageSocketPtr =
            new ConnectionMessageSocket(socketPtr,
                                        std::bind(&ConnectionRequestsController::handleConnectionDisconnect,
                                                  this),
                                        std::bind(&ConnectionRequestsController::handleConnectionMessage,
                                                  this, std::placeholders::_1),
                                        _strandPtr,
                                        _strandPtr);
        
        return;
    }

    void ConnectionRequestsController::queueConnectAfterDelay()
    {
        std::shared_ptr<boost::asio::deadline_timer> timerPtr = std::make_shared<boost::asio::deadline_timer>(_ioService,boost::posix_time::seconds(120)); //TODO: make sure the duration is 2 mins.
        timerPtr->async_wait([timerPtr,this](const boost::system::error_code&)  //TODO: Confirm timerPtr won't be destroyed till it reaches here
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
        std::map<RequestId,OnReplyMessageCallbackFn>::iterator iter =
            _requestIdCallbackFnMap.find(message_._requestId);
        ST_ASSERT(iter != _requestIdCallbackFnMap.end());
            
        _requestIdCallbackFnMap[message_._requestId](std::error_code(ErrorCode::ERR_SUCCESS),
                                            message_);
        _requestIdCallbackFnMap.erase(iter);//TODO: For client there will always be only 1 reply..so directly delete        
        return;
    }

    void ConnectionRequestsController::
    furtherHandleConnectionDisconnectForReplyExpectingRequest(const RequestId& requestId_)
    {
        std::map<RequestId,OnReplyMessageCallbackFn>::iterator iter =
            _requestIdCallbackFnMap.find(requestId_);
        ST_ASSERT(iter != _requestIdCallbackFnMap.end());
            
        _requestIdCallbackFnMap[requestId_](std::error_code(ErrorCode::ERR_AUTH_SERVER_CONNECTION_ERROR),
                                            boost::none);
        _requestIdCallbackFnMap.erase(iter);
        return;        
    }
    
    void ConnectionRequestsController::furtherHandleConnectionDisconnect()
    {
        ST_ASSERT(_requestIdCallbackFnMap.empty());
        _connectionMessageSocketPtr = nullptr;
        _onDisconnectCallbackFn();
        createAndInitializeConnectionMessageSocket();
    }

    ConnectionMessageSocket& ConnectionRequestsController::getConnectionMessageSocket()
    {
        return *_connectionMessageSocketPtr;
    }

}}}
