#include "Controller.h"

namespace Santiago{ namespace Authentication{ namespace SingleNode
{
    template<typename ClientIdType>
    Controller<ClientIdType>::Controller(ThreadSpecificDbConnection& databaseConnection_,
                                         boost::asio::io_service& ioService_,
                                         const boost::property_tree::ptree& config_):_dbConnection(databaseConnection_)
    {
        srand ( time(NULL) );
        std::error_code error;
        std::vector<SantiagoDBTables::SessionsRec> activeSessionsRec =
            _dbConnection.get().getActiveSessions(error);
        if(error && (ErrorCode::ERR_DATABASE_GET_RETURNED_ZERO_RESULTS != error.value()))
        {
            ST_LOG_ERROR("Authenticator construction failed. Error message:"<<
                         error.message());
            throw std::runtime_error("Authenticator construction failed. Error message:" +
                                     error.message());
        }
        
        for(std::vector<SantiagoDBTables::SessionsRec>::iterator it = activeSessionsRec.begin();
            it != activeSessionsRec.end();++it)
        {
            boost::optional<SantiagoDBTables::UsersRec> usersRecOpt =
                _dbConnection.get().getUsersRecForUserName(it->_userName,error);
            if(error)
            {
                ST_LOG_ERROR("Authenticator construction failed. Error message:"<<
                             error.message());
                throw std::runtime_error("Authenticator construction failed. Error message:" +
                                         error.message());
            }
            
            ST_ASSERT(usersRecOpt);
            _data.addCookie(usersRecOpt->_userName,usersRecOpt->_emailAddress,*it,boost::none);
            
            if(boost::posix_time::second_clock::universal_time() - it->_lastActiveTime >=
               boost::posix_time::time_duration(MAX_SESSION_DURATION,0,0,0))
            {
                std::error_code error = _data.cleanupLocalCookieDataAndUpdateSessionsRecordImpl(*it);                    
                ST_ASSERT(!error);
            }
        }   
    }

    template<typename ClientIdType>
    std::error_code Controller<ClientIdType>::logoutCookieFromAllClients(const std::string& cookieString_,
                                                                         boost::asio::yield_context yield_)
    {
        std::error_code error;
        error = _data.logoutUserForCookie(cookieString_,yield_);
        if(!error)
        {
            std::set<unsigned> clientIds = _data.getCookieClientIds(cookieString_);
            std::set<unsigned>::iterator it;
            for (it = clientIds.begin(); it != clientIds.end(); ++it)
            {
                _data._clientIdSet.erase(*it);
            }
            return _data.cleanupCookieDataAndUpdateSessionRecord(cookieString_,yield_);
        }
        else
        {
            ST_LOG_INFO("Cookie not valid. cookieString:" <<cookieString_<<std::endl);
            return std::error_code(ErrorCode::ERR_INVALID_SESSION_COOKIE,ErrorCategory::GetInstance());
        }
    }

    template<typename ClientIdType>
    std::error_code Controller<ClientIdType>::logoutUserFromAllClients(const std::string& userName_,
                                                                       boost::asio::yield_context yield_)
    {
        std::error_code error;
        error = _data.logoutUserForAllCookies(userName_,yield_);
        if(!error)
        {
            std::set<unsigned> clientIds = _data.getAllClientIds(userName_);
            std::set<unsigned>::iterator it;
            for (it = clientIds.begin(); it != clientIds.end(); ++it)
            {
                _data._clientIdSet.erase(*it);
            }
            return _data.cleanupCookieDataAndUpdateSessionRecordForAllCookies(userName_,yield_);
        }
        else
        {
            ST_LOG_INFO("No active session for username. username:"<<userName_);
            return std::error_code(ErrorCode::ERR_NO_ACTIVE_SESSION_FOR_USERNAME,
                                   ErrorCategory::GetInstance());
        }
    }
    
}}}
