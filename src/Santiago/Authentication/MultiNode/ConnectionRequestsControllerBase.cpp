#include "ConnectionRequestsControllerBase.h"

namespace Santiago{ namespace Authentication { namespace MultiNode
{
    ConnectionRequestsControllerBase::
    ConnectionRequestsControllerBase()
    {}
    
    void ConnectionRequestsControllerBase::handleConnectionMessageSocketDisconnect()
    {
        while(_replyPendingRequestList.begin() != _replyPendingRequestList.end())
        {
            RequestId requestId = _replyPendingRequestList.begin()->first;
            _replyPendingRequestList.erase(_replyPendingRequestList.begin());
            
            handleRequestMessage(ConnectionMessageType2::CONNECTION_DISCONNECT,
                                 requestId,
                                 boost::none);
        }
        furtherHandleConnectionMessageSocketDisconnect();
    }

    void ConnectionRequestsControllerBase::
    handleConnectionMessageSocketMessage(const RequestId& requestId_, const ConnectionMessageContent& messageContent_)
    {
        if((ConnectionMessageType::SUCCEEDED == messageContent_._type) ||
           (ConnectionMessageType::FAILED == messageContent_._type))
        {
            std::map<RequestId,unsigned>::iterator iter = _replyPendingRequestList.find(requestId_);
            if(_replyPendingRequestList.end() == iter)
            {
                BOOST_ASSERT(false);
                std::runtime_error("Unexpected requestId received");
            }

            --(iter->second);
            if(0 == iter->second)
            {
                _replyPendingRequestList.erase(iter);
            }

            handleRequestMessage(ConnectionMessageType2::CONNECTION_MESSAGE_REPLY,
                                 requestId_,
                                 messageContent_);
        }
        else
        {
            handleRequestMessage(ConnectionMessageType2::CONNECTION_MESSAGE_NEW,
                                 requestId_,
                                 messageContent_);            
        }
    }
    
    void ConnectionRequestsControllerBase::sendMessageImpl(ConnectionMessageType2 messageType_,
                                                           const RequestId& requestId_,
                                                           const ConnectionMessageContent& messageContent_)
    {
        if(ServerMessageType::CONNECTION_MESSAGE_NEW == messageType_)
        {
            std::map<RequestId,unsigned>::iterator iter = _replyPendingRequestList.find(requestId_);
            if(_replyPendingRequestList.end() == iter)
            {
                _replyPendingRequestList[requestId_] = 1;
            }
            else
            {
                ++(iter->second);
            }
        }
        else
        {
            getConnectionMessageSocket().sendMessage(requestId_,messageContent_);
        }
    }

}}}
