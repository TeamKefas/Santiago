#ifndef SANTIAGO_AUTHENTICATION_SINGLENODE_CONTROLLER_H
#define SANTIAGO_AUTHENTICATION_SINGLENODE_CONTROLLER_H

#include <functional>

#include "Santiago/Authentication/ControllerBase.h"

namespace Santiago{ namespace Authentication{ namespace MultiNode{ namespace Server
{
    class Controller:public ControllerBase<ControllerData>
    {
    public:
        typedef std::function<std::error_code(const std::string&,
                                              boost::asio::yield_context)> LogoutBroadcastCallbackFn;
        
        Controller(ThreadSpecificDbConnection& databaseConnection_,
                   const LogoutBroadcastCallbackFn& logoutCookieFromAllClientsCallbackFn_,
                   const LogoutBroadcastCallbackFn& logoutUserFromAllClientsCallbackFn_)
            :ControllerBase<ControllerData>(databaseConnection_)
        {}

    protected:
        
        virtual std::error_code logoutCookieFromAllClients(const std::string& cookieString_,
                                                           boost::asio::yield_context yield_)
        {return logoutUserForCookie(cookieString_,yield_);}
        
        virtual std::error_code logoutUserFromAllClients(const std::string& userName_,
                                                         boost::asio::yield_context yield_)
        {return logoutCookieFromAllClients(userName_,yield_);}

    };

}}}}

#endif
