#include "ConnectionRequestsController.h"

namespace Santiago{ namespace Authentication{ namespace MultiNode
{
    ConnectionRequestsController::
    ConnectionRequestsController(boost::asio::io_service& ioService_,
                                 const StrandPtr& strandPtr_,
                                 const boost::asio::ip::tcp::endpoint& endpoint_,
                                 const OnDisconnectCallbackFn& onDisconnectCallbackFn_,
                                 const OnMessageCallbackFn& onServerRequestCallbackFn_)
        :_ioService(ioService_)
        ,_strandPtr(strandPtr_)
        ,_endpoint(endpoint_)
        ,_onDisconnectCallbackFn(onDisconnectCallbackFn_)
        ,_onServerRequestCallbackFn(onServerRequestCallbackFn_)
    {}

    void ConnectionRequestsController::
    sendMessage(const AuthenticatorRequestMessage& message_,
                const boost::optional<OnMessageCallbackFn>& onMessageCallbackFn_)
    {
        if(_connectionMessageSocketOpt)
        {
            AuthenticatorRequestMessage replyMessage = message_;
            replyMessage._connectionMessageContent = boost::none;
            replyMessage._type = ConnectionMessageType2::CONNECTION_DISCONNECT;
            onMessageCallbackFn_(replyMessage);
        }

        if(message_._type == ConnectionMessageType2::CONNECTION_MESSAGE_NEW)
        {
            ST_ASSERT(_requestIdCallbackFnMap.find(message_._requestId) == _requestIdCallbackFnMap.end());
            _requestIdCallbackFnMap[message_._requestId] = onMessageCallbackFn_;
        }
        else if(message_._type == ConnectionMessageType2::CONNECTION_MESSAGE_REPLY)
        {
//            ST_ASSERT(_requestIdCallbackFnMap.find(message_._requestId) != _requestIdCallbackFnMap.end());
        }
        else
        {
            ST_ASSERT(false);
        }
        _connectionMessageSocketOpt->sendMessage(message_._requestId, message_._connectionMessageContent);
    }

    void ConnectionRequestsController::
    createAndInitializeConnectionMessageSocket(const boost::asio::ip::tcp::endpoint& endpoint_)
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
                                    std::bind(&ConnectionMessageSocket::handleConnectionMessageSocketDisconnect,
                                              this),
                                    std::bind(&ConnectionMessageSocket::handleConnectionMessageSocketMessage,
                                              this, std::placeholders::_1, std::placeholders::_2),
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
    handleRequestMessage(ConnectionMessageType2 messageType_,
                         const RequestId& requestId_,
                         const boost::optional<ConnectionMessageContent>& messageContentOpt_)
    {
        AuthenticatorRequestMessage requestMessage(requestId_,messageType_,messageContentOpt_);
        if(requestMessage._type == ConnectionMessageType2::CONNECTION_MESSAGE_NEW)
        {
            ST_ASSERT(_requestIdCallbackFnMap.find(requestId_) == _requestIdCallbackFnMap.end());
//            _requestIdCallbackFnMap[requestId_] = requestMessage;
            _onServerRequestCallbackFn(requestMessage);
        }
        else if((requestMessage._type == ConnectionMessageType2::CONNECTION_MESSAGE_REPLY) ||
                (requestMessage._type == ConnectionMessageType2::CONNECTION_DISCONNET))
        {
            std::map<RequestId,OnMessageCallbackFn>::iterator iter =
                _requestIdCallbackFnMap.find(requestId_);
            ST_ASSERT(iter != _requestIdCallbackFnMap.end());
            
            _requestIdCallbackFnMap[requestId_](requestMessage);
            _requestIdCallbackFnMap.erase(iter);
        }
        return;
    }
    
    void ConnectionRequestsController::furtherHandleConnectionMessageSocketDisconnect()
    {
        ST_ASSERT(_requestIdCallbackFnMap.empty());
        _connectionMessageSocketOpt = boost::none;
        _onDisconnectCallbackFn();
        createAndInitializeConnectionMessageSocket(_endpoint);
    }
}}}
