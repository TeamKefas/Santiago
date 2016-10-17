#include "MariaDBConnection.h"

namespace Santiago{ namespace SantiagoDBTables
{
        //take db ip, port, username, password from config
    MariaDBConnection::MariaDBConnection(const boost::property_tree::ptree& config_):
        _config(config_)
    {
        std::error_code& error;
        connect(error);
    }

    MariaDBConnection::~MariaDBConnection()
    {
        std::error_code& error;
        disconnect(error);
    }

    void MariaDBConnection::connect(std::error_code& error_)
    {
        _mysql = mysql_init(NULL);

        BOOST_ASSERT(_mysql != NULL); //This should be very rare. hence BOOST_ASSERT.

        mysql_optionsv(mysql, MYSQL_OPT_RECONNECT, (void *)"1"); //set auto reconnect.
                
        if(mysql_real_connect(_mysql,
                              config_.get<const char*>("SantiagoDBTables.host"),
                              config_.get<const char*>("SantiagoDBTables.user"),
                              config_.get<const char*>("SantiagoDBTables.password"),
                              config_.get<const char*>("SantiagoDBTables.db"),
                              config_.get<unsigned>("SantiagoDBTables.port"),
                              NULL,
                              0) == NULL)
        {
            LOG_DEBUG("mysql_real_connect() failed. host = "<<config_.get<const char*>("SantiagoDBTables.host")
                      <<" user = "<<config_.get<const char*>("SantiagoDBTables.user")
                      <<" db = "<<config_.get<const char*>("SantiagoDBTables.db")
                      <<" port = "<<config_.get<unsigned>("SantiagoDBTables.port")<<std::endl);

            error_ = std::error_code(ERR_DATABASE_EXCEPTION, ErrorCategory::GetInstance());
        }
        
        LOG_INFO("mysql_real_connect() succeeded." <<config_.get<const char*>("SantiagoDBTables.host")
                 <<" user = "<<config_.get<const char*>("SantiagoDBTables.user")
                 <<" db = "<<config_.get<const char*>("SantiagoDBTables.db")<<std::endl);
       
        error = std::error_code(ERR_SUCCESS, ErrorCategory::GetInstance());
    }

    void MariaDBConnection::disconnect(std::error_code& error_)
    {
        LOG_INFO("Closing db connection..."<<std::endl);
        mysql_close(_mysql);
        error = std::error_code(ERR_SUCCESS, ErrorCategory::GetInstance());
    }

    void MariaDBConnection::runQueryImpl(const std::string& queryString_,std::error_code& error_)
    {
        LOG_INFO("Running query:"<<std::endl
                 <<queryString_<<std::endl);

        if(mysql_query(_mysql, queryString_.c_str()) ||
           (0 != mysql_errno(_mysql)))
        {
            LOG_DEBUG("Db error:"<< mysql_error(_mysql)<<std::endl);
            error_ = std::error_code(ERR_DATABASE_EXCEPTION, ErrorCategory::GetInstance());
        }

        error = std::error_code(ERR_SUCCESS, ErrorCategory::GetInstance());
    }

    int MariaDBConnection::runInsertQuery(const std::string& queryString_,std::error_code& error_)
    {    
        LOG_INFO("Running insert query:");
        runQueryImpl(queryString_,error_)
        if(error_)
        {
            return INVALID_DATABASE_ID;
        }

        if(0 == mysql_affected_rows(_mysql))
        {
            LOG_DEGUB("mysql_affected_rows() returns 0.")
            error_ = std::error_code(ERR_DATABASE_QUERY_FAILED, ErrorCategory::GetInstance());
            return INVALID_DATABASE_ID;
        }

        int ret = mysql_insert_id(_mysql);
        error = std::error_code(ERR_SUCCESS, ErrorCategory::GetInstance());
        LOG_INFO("Insert query successful."<<std::endl);

        return ret;
    }

    void MariaDBConnection::runSelectQuery(const std::string& queryString_,
                                           const std::function<void(std::error_code&)>& postQueryFn_,
                                           std::error_code& error_)
    {
        LOG_INFO("Running select query:");
        runQueryImpl(queryString_,error_)
        if(error_)
        {
            return;
        }

        MYSQL_RES *result = mysql_store_result(_mysql);
        if(result == NULL)
        {
            LOG_DEBUG("mysql_store_result() returned null."<<std::endl);
            error_ = std::error_code(ERR_DATABASE_EXCEPTION, ErrorCategory::GetInstance());
            return;
        }

        if(0 == mysql_num_rows(result))
        {
            LOG_DEBUG("mysql_num_rows() returned 0."<<std::endl);
            error_ = std::error_code(ERR_DATABASE_QUERY_FAILED, ErrorCategory::GetInstance());
            mysql_free_result(result);
            return;
        }
        
        postQueryFn_(result,error_);
        mysql_free_result(result);
        LOG_INFO("Select query successful."<<std::endl);
        return;
    }

    void MariaDBConnection::runUpdateQuery(const std::string& queryString_,std::error_code& error_)
    {
        LOG_INFO("Running update query:");
        runQueryImpl(queryString_,error_)
        if(error_)
        {
            return;
        }

        if(0 == mysql_affected_rows(_mysql))
        {
            LOG_DEGUB("mysql_affected_rows() returns 0.");
            error_ = std::error_code(ERR_DATABASE_QUERY_FAILED, ErrorCategory::GetInstance());
            return INVALID_DATABASE_ID;
        }

        error = std::error_code(ERR_SUCCESS, ErrorCategory::GetInstance());
        LOG_INFO("Update query successful."<<std::endl);
        return;
    }

    void MariaDBConnection::runDeleteQuery(const std::string& queryString_,std::error_code& error_)
    {
        LOG_INFO("Running update query:");
        runQueryImpl(queryString_,error_)
        if(error_)
        {
            return;
        }

        if(0 == mysql_affected_rows(_mysql))
        {
            LOG_DEGUB("mysql_affected_rows() returns 0.");
            error_ = std::error_code(ERR_DATABASE_QUERY_FAILED, ErrorCategory::GetInstance());
            return INVALID_DATABASE_ID;
        }

        error = std::error_code(ERR_SUCCESS, ErrorCategory::GetInstance());
        LOG_INFO("Delete query successful."<<std::endl);
        return;
    }

    void MariaDBConnection::addUserProfilesRec(UserProfilesRec& userProfilesRec_,std::error_code& error_)
    {
        std::string addUserProfilesRecQuery = "INSERT INTO user_profiles(username,password) VALUES('" +
            userProfilesRec_._userName + "', '" + userProfilesRec_._password + "')";

        userProfilesRec_._id = runInsertQuery(addUserProfilesRecQuery,error_);
    }

    boost::optional<UserProfilesRec> getUserProfilesRec(const std::string& userName_,std::error_code& error_)
    {
        std::string getUserProfilesRecQuery = "SELECT * FROM user_profiles WHERE username = '" + userName_ + "'";
        boost::optional<UserProfilesRec> userProfilesRec;
        runSelectQuery(
            getUserProfilesRecQuery,
            [&ret](MYSQL_RES* mysqlResult_, std::error_code& error_)
            {
                if(mysql_num_rows(mysqlResult_) > 1)
                {
                    LOG_DEBUG("More than 1 records with same username in the user_profiles table"
                              <<std::endl);
                    error_ = std::error_code(ERR_DATABASE_EXCEPTION, ErrorCategory::GetInstance());
                    BOOST_ASSERT(false);
                }
                
                MYSQL_ROW row = mysql_fetch_row(result);
                BOOST_ASSERT(NULL != row);
                
                userProfilesRec->_id = atoi(row[0]);
                userProfilesRec->_userName = row[1];
                userProfilesRec->_password = row[2];
                error = std::error_code(ERR_SUCCESS, ErrorCategory::GetInstance());
            }
            error_);

        return ret;
    }

    void MariaDBConnection::updateUserProfilesRec(UserProfilesRec& newUserProfilesRec_,std::error_code& error_)
    {
        std::string updateUserProfilesRecQuery = "UPDATE user_profiles SET password ='" +
            userProfilesRec_._password + "' WHERE username = '" + userProfilesRec_._userName +"'";
        runUpdateQuery(updateUserProfilesRecQuery,error_);
    }

    void MariaDBConnection::deleteUserProfilesRec(const std::string& userName_,std::error_code& error_)
    {
        std::string deleteUserProfilesRecQuery = "DELETE FROM user_profiles WHERE username = '" +
                    userName_ + "'";
        runUpdateQuery(updateUserProfilesRecQuery,error_);
    }

    void MariaDBConnection::addSessionsRec(SessionsRec& sessionsRec_,std::error_code& error_)
    {
        std::string addSessionsRecQuery = "INSERT INTO sessions(username,cookie_string,login_time,logout_time, last_update_time) values('" +
            sessionsRec_._userName + "', '" +
            sessionsRec_._cookieId + "', '" +
            convertPtimeToString(sessionsRec._loginTime) + "'," +
            sessionsRec._logoutTime? "'" + convertPtimeToString(sessionsRec._logoutTime) + "'": "NULL" + ", '" +
            convertPtimeToString(sessionsRec._lastUpdateTime) +
            "')";

        sessionsRec_._id = runInsertQuery(addUserProfilesRecQuery,error_);
    }

    boost::optional<SessionsRec> MariaDBConnection::getSessionsRec(const std::string& cookieString_,
                                                                   std::error_code& error_)
    {
        std::string getSessionsRecQuery = "SELECT * FROM sessions WHERE cookie_string = '" + cookieString_ + "'";
        boost::optional<SessionsRec> sessionsRec;

        runSelectQuery(
            getSessionsRecQuery,
            [&ret](MYSQL_RES* mysqlResult_, std::error_code& error_)
            {
                if(mysql_num_rows(mysqlResult_) > 1)
                {
                    LOG_DEBUG("More than 1 records with same cookie_string in the sessions table "
                              <<std::endl);
                    error_ = std::error_code(ERR_DATABASE_EXCEPTION, ErrorCategory::GetInstance());
                    BOOST_ASSERT(false);
                }
                
                MYSQL_ROW row = mysql_fetch_row(result);
                BOOST_ASSERT(NULL != row);

                sessionsRec->_id = atoi(row[0]);
                sessionsRec->_userName = row[1];
                sessionsRec->_cookieString = row[2];
                sessionsRec->_loginTime = convertStringToPtime(row[3]);

                if(NULL != row[4])
                {
                    sessionsRec->_logoutTime = convertStringToPtime(row[4]);
                }

                sessionsRec->_lastUpdateTime = convertStringToPtime(row[5]);

                error = std::error_code(ERR_SUCCESS, ErrorCategory::GetInstance());
            }
            error_);

    }
    void MariaDBConnection::updateSessionsRec(SessionsRec& userProfilesRec_,std::error_code& error_)
    {
        std::string updateSessionsRecQuery = "UPDATE sessions SET logout_time ='" +
            convertPtimeToString(userProfilesRec_._logoutTime) + "' WHERE cookie_string ='" +
            sessionsRec_._cookieString + "'";
        runUpdateQuery(updateSessionsRecQuery,error_);
    }

    boost::posix_time::ptime MariaDBConnection::convertStringToPtime(const std::string& timeStr_) const
    {
        return boost::posix_time::time_from_string(timeStr_);
    }

    std::string MariaDBConnection::convertPtimeToString(const boost::posix_time::ptime& ptime_) const
    {
        std::string timeStr = boost::gregorian::to_iso_extended_string(ptime_.date()) + ' ' +
            boost::posix_time::to_simple_string(ptime_.time_of_day());

        return timeStr;
    }
}}
