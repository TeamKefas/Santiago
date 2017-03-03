#include "ConnectionRequestsController.h"

namespace Santiago{ namespace User { namespace Server
{

    ConnectionRequestsController::ConnectionRequestsController(const ConnectionMessageSocket::MySocketPtr& socketPtr_,
                                                               const OnDisconnectCallbackFn& onDisconnectCallbackFn_,
                                                               const OnNewRequestCallbackFn& onNewRequestCallbackFn_,
                                                               const OnRequestReplyCallbackFn&
                                                               onRequestReplyCallbackFn_,
                                                               unsigned connectionId_)
        :_onDisconnectCallbackFn(onDisconnectCallbackFn_)
        ,_onNewRequestCallbackFn(onNewRequestCallbackFn_)
        ,_onRequestReplyCallbackFn(onRequestReplyCallbackFn_)
        ,_connectionId(connectionId_)
        ,_connectionMessageSocketPtr(new ConnectionMessageSocket(
                                         socketPtr_,
                                         std::bind(
                                             &ConnectionRequestsController::handleConnectionMessageSocketDisconnect,
                                             this),
                                         std::bind(
                                             &ConnectionRequestsController::handleConnectionMessageSocketMessage,
                                             this,
                                             std::placeholders::_1,
                                             std::placeholders::_2),
                                         connectionId_))
    {
        /* std::function<void(const ConnectionMessage)> onMessageCallbackFn = 
            std::bind(&ConnectionRequestsController::handleConnectionMessageSocketMessage,this,
                      std::placeholders::_1,std::placeholders::_2);
        ConnectionMessageSocketPtr newConnectionMessageSocket(new ConnectionMessageSocket
                                                              (socketPtr_,_onDisconnectCallbackFn,
                                                               onMessageCallbackFn));
                                                               newConnectionMessageSocket->start();*/
        _connectionMessageSocketPtr->startRead();
    } 

    ConnectionRequestsController::~ConnectionRequestsController()
    {
        ST_ASSERT( _replyPendingRequestList.size() == 0);
    }
    
    void ConnectionRequestsController::handleConnectionMessageSocketDisconnect()
    {
        ST_LOG_DEBUG("In handleConnectionSocketDisconnect. connectionId = "<<_connectionId<<std::endl);

        while(_replyPendingRequestList.begin() != _replyPendingRequestList.end())
        {
            RequestId requestId = _replyPendingRequestList.begin()->first;
            _replyPendingRequestList.erase(_replyPendingRequestList.begin());
            
            ServerMessage serverMessage(_connectionId, requestId,
                                        ServerMessageType::CONNECTION_DISCONNECT,boost::none);
            _onRequestReplyCallbackFn(serverMessage); //Note: Ensure that _onRequestReplyCallbackFn does not throw. 
        }
        _onDisconnectCallbackFn(_connectionId); //Note: Ensure that _onDisconnectCallbackFn does not throw.

        ST_LOG_DEBUG("handleConnectionSocketDisconnect completed. connectionId = "<<_connectionId<<std::endl);
    }

    void ConnectionRequestsController::
    handleConnectionMessageSocketMessage(const RequestId& requestId_, const ConnectionMessage& message_)
    {
        ST_LOG_DEBUG("In handleConnectionMessageSocketMessage. connectionId = "<<_connectionId
                     <<", initiatingConnectionId ="<<requestId_._initiatingConnectionId
                     <<", requestNo ="<<requestId_._requestNo<<std::endl);

        if((ConnectionMessageType::SUCCEEDED == message_._type) ||
           (ConnectionMessageType::FAILED == message_._type))
        {
            std::map<RequestId,unsigned>::iterator iter = _replyPendingRequestList.find(requestId_);
            if(_replyPendingRequestList.end() == iter)
            {
                ST_LOG_ERROR("Unexpected requestId received. connectionId ="<<_connectionId
                             <<", initialingConnectionId = "<< requestId_._initiatingConnectionId
                             <<", requestNo ="<<requestId_._requestNo<<std::endl
                             << std::endl);
                ST_ASSERT(false);
                return;
            }
            
            --(iter->second);
            if(0 == iter->second)
            {
                _replyPendingRequestList.erase(iter);
            }

            ServerMessage serverMessage(_connectionId,
                                        requestId_,
                                        ServerMessageType::CONNECTION_MESSAGE_REPLY,
                                        message_);

            _onRequestReplyCallbackFn(serverMessage); //Note: Ensure that onRequestReplyCallbackFn does not throw
        }
        else
        {
            ServerMessage serverMessage(_connectionId,
                                        requestId_,
                                        ServerMessageType::CONNECTION_MESSAGE_NEW,
                                        message_);

            if(_connectionId != requestId_._initiatingConnectionId)
            {
                ST_LOG_ERROR("Invalid requestId given for new request. The connectionIds do not match."
                             <<" connectionId = "<<_connectionId
                             <<", initiatingConnectionId = "<<requestId_._initiatingConnectionId
                             <<", requestNo = "<<requestId_._requestNo <<std::endl);
                ST_ASSERT(false);
                return;
            }

            _onNewRequestCallbackFn(serverMessage); //Note: Ensure that onRequestReplyCallbackFn does not throw
        }
        ST_LOG_DEBUG("handleConnectionMessageSocketMessage ended."<<std::endl);
    }
    
    void ConnectionRequestsController::sendMessage(const ServerMessage& message_)
    {
        ST_LOG_DEBUG("in sendMessage. connectionId ="<<_connectionId
                     <<", initiatingConnectionId ="<<message_._requestId._initiatingConnectionId
                     <<", requestNo ="<<message_._requestId._requestNo<<std::endl);

        ST_ASSERT(message_._connectionMessage);
        if(ServerMessageType::CONNECTION_MESSAGE_NEW == message_._type)
        {
            ST_LOG_DEBUG("ServerMessageType = CONNECTION_MESSAGE_NEW"<<std::endl);

            std::map<RequestId,unsigned>::iterator iter = _replyPendingRequestList.find(message_._requestId);
            if(_replyPendingRequestList.end() == iter)
            {
                _replyPendingRequestList[message_._requestId] = 1;
            }
            else
            {
                ++(iter->second);
            }
        }
        else
        {
            ST_LOG_DEBUG("ServerMessageType NOT CONNECTION_MESSAGE_NEW"<<std::endl);
            ST_ASSERT(_connectionId == message_._requestId._initiatingConnectionId);
        }
        _connectionMessageSocketPtr->sendMessage(message_._requestId,*message_._connectionMessage);
        ST_LOG_DEBUG("sendMessage ended"<<std::endl);
    }

}}}
