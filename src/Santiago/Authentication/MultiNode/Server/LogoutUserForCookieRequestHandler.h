#ifndef SANTIAGO_AUTHENTICATION_SERVER_LOGOUTUSERFORCOOKIEREQUESTHANDLER_H
#define SANTIAGO_AUTHENTICATION_SERVER_LOGOUTUSERFORCOOKIEREQUESTHANDLER_H

/**
 * @file LogoutUserForCookieRequestHandler.h
 *
 * @section DESCRIPTION
 *
 * Contains the LogoutUserForCookieRequestHandler class  
 */

#include "RequestHandlerBase.h"


namespace Santiago{ namespace Authentication { namespace MultiNode { namespace Server
{
    class LogoutUserForCookieRequestHandler:public RequestHandlerBase
    {
    public:
        /**
         * The constructor
         * @param connectionServer_- 
         * @param onCompletedCallbackFn_ -
         * @param initiatingMessage_ -
         */
        LogoutUserForCookieRequestHandler(ConnectionServer& connectionServer_,
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
