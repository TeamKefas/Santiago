#ifndef SANTIAGO_AUTHENTICATION_MULTINODE_AUTHENTICATORREQUESTMESSAGE_H
#define SANTIAGO_AUTHENTICATION_MULTINODE_AUTHENTICATORREQUESTMESSAGE_H

#include "ConnectionMessage.h"

namespace Santiago{ namespace Authenticaton{ namespace MultiNode
{
    struct AuthenticatorRequestMessage
    {
        AuthenticatorRequestMessage(const RequestId& requestId_,
                                    ConnectionMessageType2 type_,
                                    const Optional<ConnectionMessage>& connectionMessage_)
            :_requestId(requestId_)
            ,_type(type_)
            ,_connectionMessage(connectionMessage_)
        {}

        RequestId                             _requestId;
        ConnectionMessageType2                _type;
        Optional<ConnectionMessageContent>    _connectionMessageContent;
    };

}}}

#endif
