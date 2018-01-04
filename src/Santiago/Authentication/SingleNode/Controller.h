#ifndef SANTIAGO_AUTHENTICATION_SINGLENODE_CONTROLLER_H
#define SANTIAGO_AUTHENTICATION_SINGLENODE_CONTROLLER_H

#include "Santiago/Authentication/ControllerBase.h"

#include "ControllerData.h"

namespace Santiago{ namespace Authentication{ namespace SingleNode
{
    struct ControllerTypes
    {
        struct ClientRequestData
        {
            None getClientId() const {return None();}
        };
        typedef ControllerData LocalData;
    };
    
    class Controller:public ControllerBase<ControllerTypes>
    {
    public:
        typedef ControllerTypes::ClientRequestData ClientRequestData;
        
        Controller(ThreadSpecificDbConnection& databaseConnection_)
            :ControllerBase<ControllerTypes>(databaseConnection_)
        {}

    protected:
        
        virtual std::error_code logoutCookieFromAllClients(const ClientRequestData& requestData_,
                                                           const std::string& cookieString_,
                                                           boost::asio::yield_context yield_)
        {return std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance());}
        
        virtual std::error_code logoutUserFromAllClients(const ClientRequestData& requestData_,
                                                         const std::string& userName_,
                                                         boost::asio::yield_context yield_)
        {return std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance());}

    };

}}}

#endif
