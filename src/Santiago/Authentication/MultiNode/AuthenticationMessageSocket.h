#ifndef SANTIAGO_AUTHENTICATION_MULTINODE_AUTHENTICATORMESSAGESOCKET_H
#define SANTIAGO_AUTHENTICATION_MULTINODE_AUTHENTICATORMESSAGESOCKET_H

#include "ConnectionMessage.h"
namespace Santiago{ namespace Authentication{ namespace MultiNode
{

    class AuthenticatorMessageSocket
    {
    public:
        typedef std::function<void(const std::error_code&,const ConnectionMessage&)> CallbackFn;

        void sendMessage(const ConnectionMessage& connectionMessage_, CallbackFn callbackFn_);

    };

}}}


#endif
