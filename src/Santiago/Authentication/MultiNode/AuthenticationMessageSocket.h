#ifndef SANTIAGO_AUTHENTICATION_MULTINODE_AUTHENTICATORMESSAGESOCKET_H
#define SANTIAGO_AUTHENTICATION_MULTINODE_AUTHENTICATORMESSAGESOCKET_H

namespace Santiago{ namespace Authentication{ namespace MultiNode
{

    class AuthenticationMessageSocket
    {
    public:

        void sendMessage(const ConnectionMessage& connectionMessage_, CallbackFn callbackFn_);

    };

}}}


#endif
