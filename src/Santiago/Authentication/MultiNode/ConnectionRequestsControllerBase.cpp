#include "ConnectionRequestsControllerBase.h"

namespace Santiago{ namespace Authentication { namespace MultiNode
{
    ConnectionRequestsControllerBase::
    ConnectionRequestsControllerBase()
    {}
    
    void ConnectionRequestsControllerBase::handleConnectionDisconnect()
    {
        while(_replyExpectingRequestList.begin() != _replyExpectingRequestList.end())
        {
            RequestId requestId = _replyExpectingRequestList.begin()->first;
            _replyExpectingRequestList.erase(_replyExpectingRequestList.begin());

            furtherHandleConnectionDisconnectForReplyExpectingRequest(requestId);
        }
        furtherHandleDisconnect();
    }

    void ConnectionRequestsControllerBase::handleConnectionMessage(const ConnectionMessage& message_)
    {
        if((ConnectionMessageType::SUCCEEDED == message_._type) ||
           (ConnectionMessageType::FAILED == message_._type))
        {
            std::map<RequestId,unsigned>::iterator iter = _replyExpectingRequestList.find(message_._requestId);
            if(_replyExpectingRequestList.end() == iter)
            {
                BOOST_ASSERT(false);
                std::runtime_error("Unexpected requestId received");
            }

            --(iter->second);
            if(0 == iter->second)
            {
                _replyExpectingRequestList.erase(iter);
            }

            furtherHandleReplyConnectionMessage(message_);
        }
        else
        {
            furtherHandleNewConnectionMessage(message_);
        }
    }
    
    void ConnectionRequestsControllerBase::sendMessageImpl(const ConnectionMessage& message_,
                                                           bool isReplyExpectingMessage_)
    {
        if(isReplyExpectingMessage_)
        {
            std::map<RequestId,unsigned>::iterator iter = _replyExpectingRequestList.find(message_._requestId);
            if(_replyExpectingRequestList.end() == iter)
            {
                _replyExpectingRequestList[message_._requestId] = 1;
            }
            else
            {
                ++(iter->second);
            }
        }
        getConnectionMessageSocket().sendMessage(message_);
    }

}}}
