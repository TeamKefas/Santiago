#include "Controller.h"

namespace Santiago{ namespace Authentication{ namespace MultiNode{ namespace Server
{
    Controller::Controller(ThreadSpecificDbConnection& databaseConnection_,
                           const StrandPtr& strandPtr_,
                           ConnectionServer &connectionServer_)
        :ControllerBase<ControllerData>(databaseConnection_)
        ,_strandPtr(strandPtr_)
        ,_connectionServer(connectionServer_)
    {}

    std::error_code Controller::logoutCookieFromAllClients(const ClientRequestData& requestData_,
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

    std::error_code Controller::logoutUserFromAllClients(const ClientRequestData& requestData_,
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

    std::error_code Controller::sendMessageAndGetReplyFromClients(const std::set<unsigned>& clientIds_,
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
                [&ret](const std::error_code& error_)
                {
                    ret = error_;
                },
                clientIds_
            });
            
        for(auto iter = clientIds_.begin();
            iter != clientIds_.end();
            iter++)
        {
            std::function<void(const std::error_code&,
                               const boost::optional<Santiago::Authentication::MultiNode::ConnectionMessage>&)>
                               onReplyMessageCallbackFn =
                               std::bind(&Controller::handleClientReplyMessageOutsideStrand,
                                         this,
                                         clientRequestsDataPtr,
                                         *iter,
                                         std::placeholders::_1,
                                         std::placeholders::_2);
            _connectionServer.sendMessage(*iter,
                                          connectionMessage_,
                                          true,
                                          onReplyMessageCallbackFn);
        }
            
        result.get();
        return ret;
    }

    void Controller::handleClientReplyMessageOutsideStrand(const ClientLogoutRequestsDataPtr& logoutRequestsDataPtr_,
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
    
    void Controller::handleClientReplyMessage(const ClientLogoutRequestsDataPtr& logoutRequestsDataPtr_,
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
        //logoutRequestsDataPtr_->_replyPendingClientIds.erase(replyPendingClientIdsIter);
        
        if(logoutRequestsDataPtr_->_replyPendingClientIds.empty())
        {
            logoutRequestsDataPtr_->_onCompletedCallbackFn(std::error_code(ErrorCode::ERR_SUCCESS,
                                                                           ErrorCategory::GetInstance()));
        }
        return;
    }
    
}}}}
