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
        typedef ControllerTypes::ClientRequestData   ClientRequestData;
        typedef std::shared_ptr<boost::asio::strand> StrandPtr;
        
        Controller(ThreadSpecificDbConnection& databaseConnection_,
                   const StrandPtr& strandPtr_,
                   ConnectionServer &connectionServer_)
            :ControllerBase<ControllerData>(databaseConnection_)
            ,_strandPtr(_strandPtr)
            ,_connectionServer(connectionServer_)
        {}

    protected:
        
        virtual std::error_code logoutCookieFromAllClients(const ClientRequestData& requestData_,
                                                           const std::string& cookieString_,
                                                           boost::asio::yield_context yield_)
        {
            return sendMessageAndGetReplyFromClients(
                _localData.getCookieClientIds(cookieString_),
                ConnectionMessage(requestData_._requestId,
                                  ConnectionMessageType::SR_LOGOUT_USER_FOR_COOKIE,
                                  std::vector<std::string>(1,cookieString_)),
                                  yield_);
        }

        virtual std::error_code logoutUserFromAllClients(const ClientRequestData& requestData_,
                                                         const std::string& userName_,
                                                         boost::asio::yield_context yield_)
        {
            return sendMessageAndGetReplyFromClients(
                _localData.getAllClientIdsForUser(userName_),
                ConnectionMessage(requestData_._requestId,
                                  ConnectionMessageType::SR_LOGOUT_USER_FOR_ALL_COOKIES,
                                  std::vector<std::string>(1,userName_)),
                                  yield_);
        }

        struct ClientLogoutRequestsData
        {
            std::function<void(const std::error_code&)> _onCompletedCallbackFn;
            std::set<unsigned>&                         _replyPendingClientIds;
        };
        typedef std::shared_ptr<ClientLogoutRequestsData>  ClientLogoutRequestsDataPtr;

        void sendMessageAndGetReplyFromClients(const std::set<unsigned>& clientIds_,
                                               const ConnectionMessage& connectionMessage_,
                                               boost::asio::yield_context yield_)
        {
            typename boost::asio::handler_type<boost::asio::yield_context, void()>::type
                handler(std::forward<boost::asio::yield_context>(yield_));
        
            boost::asio::async_result<decltype(handler)> result(handler);
            std::error_code ret;
        
            ClientLogoutRequestsDataPtr clientRequestsDataPtr(
                new ClientLogoutRequestsData
                {
                    [ret&](const std::error_code& error_)
                    {
                        ret = error_;
                    },
                    clientIds_
                });
            
            for(auto iter = clientIds_.begin();
                iter != clientIds_.end();
                iter++)
            {
                _connectionServer.sendMessage(*iter,
                                              connectionMessage_,
                                              true,
                                              std::bind(&Controller::handleClientReplyMessageOutsideStrand,
                                                        this,
                                                        clientRequestsDataPtr,
                                                        *iter,
                                                        std::placeholders::_1,
                                                        std::placeholders::_2));
            }
            
            result.get();
            return ret;
        }

        void handleClientReplyMessageOutsideStrand(const ClientLogoutRequestsDataPtr& logoutRequestsDataPtr_,
                                                   unsigned clientId_,
                                                   const std::error_code& errorCode_,
                                                   const boost::optional<ConnectionMessage>& connectionMessageOpt_)
        {
            _strandPtr->dispatch(std::bind(&Controller::handleClientReplyMessage,
                                           this,
                                           logoutRequestsDataPtr_,
                                           clientId_,
                                           errorCode_,
                                           connectionMessageOpt_));
        }

        void handleClientReplyMessage(const ClientLogoutRequestsDataPtr& logoutRequestsDataPtr_,
                                      unsigned clientId_,
                                      const std::error_code& errorCode_,
                                      const boost::optional<ConnectionMessage>& connectionMessageOpt_)
        {
            ST_ASSERT(logoutRequestsDataPtr_);
            ST_ASSERT(ErrorCode::ERR_AUTH_SERVER_CONNECTION_ERROR == errorCode_.value() ||
                      ErrorCode::ERR_SUCCESS == errorCode_.value());
            
            auto replyPendingClientIdsIter = std::find(logoutRequestsDataPtr_->_replyPendingClientIds.begin(),
                                                       logoutRequestsDataPtr_->_replyPendingClientIds.end(),
                                                       clientId_);
            ST_ASSERT(replyPendingClientIdsIter != logoutRequestsDataPtr_->_replyPendingClientIds.end());
            logoutRequestsDataPtr_->_replyPendingClientIds.erase(replyPendingClientIdsIter);

            if(logoutRequestsDataPtr_->_replyPendingClientIds.empty())
            {
                logoutRequestsDataPtr_->_onCompletedCallbackFn(std::error_code(ErrorCode::ERR_SUCCESS,
                                                                               ErrorCategory::GetInstance()));
            }
            return;
        }

        StrandPtr         _strandPtr;
        ConnectionServer &_connectionServer;
    };

}}}}

#endif
