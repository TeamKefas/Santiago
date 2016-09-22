#include "ServerMessage.h"

namespace Santiago{ namespace Authentication
{
    RequestId::RequestId(unsigned initiatingConnectionId_,unsigned requestNo_)
        :_initiatingConnectionId(initiatingConnectionId_)
        ,_requestNo(requestNo_)
    {}

    bool RequestId::operator<(const RequestId& rhs_) const
    {
        if(this->_initiatingConnectionId < rhs_._initiatingConnectionId)
        {
            return true;
        }
        else if(this->_initiatingConnectionId > rhs_._initiatingConnectionId)
        {
            return false;
        }
        else
        {
            if(this->_requestNo < rhs_._requestNo)
            {
                return true;
            }
            else if(this->_requestNo < rhs_._requestNo)
            {
                return false;
            }
            else
            {
                BOOST_ASSERT(false);
            }
        }
    }
   
    ServerMessage::ServerMessage(unsigned connectionId_,
                                 const RequestId& requestId_,
                                 ServerMessageType type_,
                                 const boost::optional<ConnectionMessage>& connectionMessage_)
        :_connectionId(connectionId_)
        ,_requestId(requestId_)
        ,_type(type_)
        ,_connectionMessage(connectionMessage_)
    {}

}}//closing Santiago::Authentication 
