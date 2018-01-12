#include "ConnectionRequestsController.h"

#include<boost/asio/deadline_timer.hpp>

namespace Santiago{ namespace Authentication{ namespace MultiNode{ namespace Server
{
    ConnectionRequestsController::
    ConnectionRequestsController(boost::asio::io_service& ioService_,
                                 const MySocketPtr& socketPtr_,
                                 unsigned connectionId_,
                                 const StrandPtr& strandPtr_,
                                 const OnDisconnectCallbackFn& onDisconnectCallbackFn_,
                                 const OnNewRequestMessageCallbackFn& onNewRequestMessageCallbackFn_)
        :_ioService(ioService_)
        ,_connectionMessageSocket(socketPtr_,
                                  std::bind(&ConnectionRequestsController::handleConnectionDisconnect,
                                            this),
                                  std::bind(&ConnectionRequestsController::handleConnectionMessage,
                                            this, std::placeholders::_1),
                                  strandPtr_,
                                  strandPtr_)
        ,_connectionId(connectionId_)
        ,_onDisconnectCallbackFn(onDisconnectCallbackFn_)
        ,_onNewRequestMessageCallbackFn(onNewRequestMessageCallbackFn_)
    {}

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
/*        
        if(!_connectionMessageSocketPtr && isReplyExpectingMessage_)
        {
            onReplyMessageCallbackFn(std::error_code(ErrorCode::ERR_AUTH_SERVER_CONNECTION_ERROR,
                                                      ErrorCategory::GetInstance()),
                                      boost::none);
            return;
        }
*/
        if(isReplyExpectingMessage_)
        {
            ST_ASSERT(_requestIdCallbackFnMap.find(message_._requestId) == _requestIdCallbackFnMap.end());
            _requestIdCallbackFnMap.insert(std::pair<RequestId,OnReplyMessageCallbackFn>(message_._requestId,onReplyMessageCallbackFn));
        }
        
        _connectionMessageSocket.sendMessage(message_);
    }
    
    unsigned ConnectionRequestsController::getConnectionId()
    {
        return _connectionId;
    }

    void ConnectionRequestsController::
    furtherHandleNewConnectionMessage(const ConnectionMessage& message_)
    {
        _onNewRequestMessageCallbackFn(message_);
        return;
    }

    void ConnectionRequestsController::
    furtherHandleReplyConnectionMessage(const ConnectionMessage& message_)
    {
        std::map<RequestId,OnReplyMessageCallbackFn>::iterator iter =
            _requestIdCallbackFnMap.find(message_._requestId);
        ST_ASSERT(iter != _requestIdCallbackFnMap.end());
            
        _requestIdCallbackFnMap[message_._requestId](std::error_code(ErrorCode::ERR_SUCCESS,
                                                            ErrorCategory::GetInstance()),
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
            
        _requestIdCallbackFnMap[requestId_](std::error_code(ErrorCode::ERR_AUTH_SERVER_CONNECTION_ERROR,
                                                            ErrorCategory::GetInstance()),
                                            boost::none);
        _requestIdCallbackFnMap.erase(iter);
        return;        
    }
    
    void ConnectionRequestsController::furtherHandleConnectionDisconnect()
    {
        ST_ASSERT(_requestIdCallbackFnMap.empty());
        _onDisconnectCallbackFn();
    }

    ConnectionMessageSocket& ConnectionRequestsController::getConnectionMessageSocket()
    {
        return _connectionMessageSocket;
    }

}}}}
