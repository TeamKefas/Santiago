#ifndef SANTIAGO_AUTHENTICATION_SERVER_CHANGEUSEREMAILADDRESSREQUESTHANDLER_H
#define SANTIAGO_AUTHENTICATION_SERVER_CHANGEUSEREMAILADDRESSREQUESTHANDLER_H

/**
 * @file ChangeUserEmailAddressRequestHandler.h
 *
 * @section DESCRIPTION
 *
 * Contains the ChangeUserEmailAddressRequestHandler class  
 */

#include "RequestHandlerBase.h"

namespace Santiago{namespace Authentication { namespace Server
{

    class ChangeUserEmailAddressRequestHandler:public RequestHandlerBase
    {
    public:
        /**
         * The constructor
         * @param connectionServer_- 
         * @param onCompletedCallbackFn_- ///NEED TO WRITE\\\
         * @param initiatingMessage_ -
         */
        ChangeUserEmailAddressRequestHandler(ConnectionServer& connectionServer_,
                                             ServerData& serverData_,
                                             const OnCompletedCallbackFn& onCompletedCallbackFn_,
                                             const ConnectionMessage& initiatingMessage_);
       /**
        * ///Message\\
        */
        virtual void handleRequest();
    };

}}}

#endif
