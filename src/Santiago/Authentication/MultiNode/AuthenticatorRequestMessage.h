#ifndef SANTIAGO_AUTHENTICATION_MULTINODE_AUTHENTICATORREQUESTMESSAGE_H
#define SANTIAGO_AUTHENTICATION_MULTINODE_AUTHENTICATORREQUESTMESSAGE_H

#include "ConnectionMessage.h"

namespace Santiago{ namespace Authenticaton{ namespace MultiNode
{
    struct AuthenticatorRequestMessage
    {
        AuthenticatorRequestMessage(const RequestId& requestId_,
                                    ConnectionMessageRequest type_,
                                    const boost::optional<ConnectionMessage>& connectionMessageConent_)
            :_requestId(requestId_)
            ,_type(type_)
            ,_connectionMessage(connectionMessage_)
        {}

        RequestId                                    _requestId;
        ConnectionMessageRequest                     _type;
        boost::optional<ConnectionMessageContent>    _connectionMessageContent;
    };

}}}

#endif
