#ifndef SANTIAGO_AUTHENTICATION_SERVER_LOGOUTUSERFORALLCOOKIESREQUESTHANDLER_H
#define SANTIAGO_AUTHENTICATION_SERVER_LOGOUTUSERFORALLCOOKIESREQUESTHANDLER_H

/**
 * @file LogoutUserForAllCookiesRequestHandler.h
 *
 * @section DESCRIPTION
 *
 * Contains the LogoutUserForAllCookiesRequestHandler class  
 */

#include "RequestHandlerBase.h"


namespace Santiago{ namespace Authentication { namespace MultiNode { namespace Server
{
    class LogoutUserForAllCookiesRequestHandler:public RequestHandlerBase
    {
    public:
        /**
         * The constructor
         * @param connectionServer_- 
         * @param onCompletedCallbackFn_ -
         * @param initiatingMessage_ -
         */
        LogoutUserForAllCookiesRequestHandler(ConnectionServer& connectionServer_,
                                              ServerData& serverData_,
                                              const OnCompletedCallbackFn& onCompletedCallbackFn_,
                                              const ConnectionMessage& initiatingMessage_);
       /**
        * ///Message\\
        */
        virtual void handleRequest();
    };

}}}}

#endif
