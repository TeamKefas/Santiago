#ifndef SANTIAGO_AUTHENTICATION_MULTINODE_SERVER_CONTROLLER_H
#define SANTIAGO_AUTHENTICATION_MULTINODE_SERVER_CONTROLLER_H

#include <functional>

#include "Santiago/Authentication/ControllerBase.h"
#include "Santiago/Authentication/MultiNode/ConnectionMessage.h"

#include "ControllerData.h"

namespace Santiago{ namespace Authentication{ namespace MultiNode{ namespace Server
{
    struct ControllerTypes
    {
        struct ClientRequestData
        {
            ClientRequestData(const RequestId& requestId_)
                :_requestId(requestId_)
            {}
            
            unsigned getClientId() const {return _requestId._initiatingConnectionId;}

            RequestId   _requestId;
        };
        
        typedef ControllerData LocalData;
    };
        
    class Controller:public ControllerBase<ControllerData>
    {
    public:
        typedef ControllerTypes::ClientRequestData ClientRequestData;
        
        Controller(ThreadSpecificDbConnection& databaseConnection_)
            :ControllerBase<ControllerData>(databaseConnection_)
        {}

    protected:
        
        virtual std::error_code logoutCookieFromAllClients(const ClientRequestData& requestData_,
                                                           const std::string& cookieString_,
                                                           boost::asio::yield_context yield_)
        {return std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance());}//TODO
        
        virtual std::error_code logoutUserFromAllClients(const ClientRequestData& requestData_,
                                                         const std::string& userName_,
                                                         boost::asio::yield_context yield_)
        {return std::error_code(ErrorCode::ERR_SUCCESS,ErrorCategory::GetInstance());}//TODO

    };

}}}}

#endif
