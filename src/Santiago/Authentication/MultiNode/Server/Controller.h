#ifndef SANTIAGO_AUTHENTICATION_MULTINODE_SERVER_CONTROLLER_H
#define SANTIAGO_AUTHENTICATION_MULTINODE_SERVER_CONTROLLER_H

#include <functional>
#include <memory>
#include <set>

#include <boost/asio/strand.hpp>

#include "Santiago/Authentication/ControllerBase.h"
#include "Santiago/Authentication/MultiNode/ConnectionMessage.h"

#include "ControllerData.h"
#include "ConnectionServer.h"

#include "ControllerData.h"

namespace Santiago{ namespace Authentication{ namespace MultiNode{ namespace Server
{
    class Controller:public ControllerBase<ControllerData>
    {
    public:
        typedef ControllerTypes::ClientRequestData   ClientRequestData;
        typedef std::shared_ptr<boost::asio::strand> StrandPtr;
        
        Controller(ThreadSpecificDbConnection& databaseConnection_,
                   const StrandPtr& strandPtr_,
                   ConnectionServer &connectionServer_);

    protected:
        
        virtual std::error_code logoutCookieFromAllClients(const ClientRequestData& requestData_,
                                                           const std::string& cookieString_,
                                                           boost::asio::yield_context yield_);

        virtual std::error_code logoutUserFromAllClients(const ClientRequestData& requestData_,
                                                         const std::string& userName_,
                                                         boost::asio::yield_context yield_);

        struct ClientLogoutRequestsData
        {
            std::function<void(const std::error_code&)> _onCompletedCallbackFn;
            const std::set<unsigned>&                         _replyPendingClientIds;
        };
        typedef std::shared_ptr<ClientLogoutRequestsData>  ClientLogoutRequestsDataPtr;

        std::error_code sendMessageAndGetReplyFromClients(const std::set<unsigned>& clientIds_,
                                               const ConnectionMessage& connectionMessage_,
                                               boost::asio::yield_context yield_);

        void handleClientReplyMessageOutsideStrand(const ClientLogoutRequestsDataPtr& logoutRequestsDataPtr_,
                                                   unsigned clientId_,
                                                   const std::error_code& errorCode_,
                                                   const boost::optional<ConnectionMessage>& connectionMessageOpt_);

        void handleClientReplyMessage(const ClientLogoutRequestsDataPtr& logoutRequestsDataPtr_,
                                      unsigned clientId_,
                                      const std::error_code& errorCode_,
                                      const boost::optional<ConnectionMessage>& connectionMessageOpt_);

        StrandPtr         _strandPtr;
        ConnectionServer &_connectionServer;
    };

}}}}

#endif
