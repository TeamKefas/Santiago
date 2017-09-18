#ifndef SANTIAGO_AUTHENTICATION_SINGLENODE_CONTROLLER_H
#define SANTIAGO_AUTHENTICATION_SINGLENODE_CONTROLLER_H

#include "Santiago/Authentication/ControllerBase.h"

#include "ControllerData.h"

namespace Santiago{ namespace Authentication{ namespace SingleNode
{
    class Controller:public ControllerBase<ControllerData>
    {
    public:

        ControllerBase(ThreadSpecificDbConnection& databaseConnection_)
            :ControllerBase<ControllerData>(databaseConnection_)
        {}

    protected:
        
        virtual std::error_code logoutCookieFromAllClients(const std::string& cookieString_,
                                                           boost::asio::yield_context yield_)
        {return std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance());}
        
        virtual std::error_code logoutUserFromAllClients(const std::string& userName_,
                                                         boost::asio::yield_context yield_)
        {return std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance());}

    };

}}}

#endif
