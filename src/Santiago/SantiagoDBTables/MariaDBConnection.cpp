#include "../Utils/PTimeUtils.h"
#include "../Utils/STLog.h"

#include "MariaDBConnection.h"

namespace Santiago{ namespace SantiagoDBTables
{
        //take db ip, port, username, password from config
    MariaDBConnection::MariaDBConnection(const boost::property_tree::ptree& config_):
        _config(config_)
    {
        std::error_code error;
        connect(error);
    }

    MariaDBConnection::~MariaDBConnection()
    {
        std::error_code error;
        disconnect(error);
    }

    void MariaDBConnection::connect(std::error_code& error_)
    {
        _mysql = mysql_init(NULL);

        BOOST_ASSERT(_mysql != NULL); //This should be very rare. hence BOOST_ASSERT.

        mysql_options(_mysql, MYSQL_OPT_RECONNECT, (void *)"1"); //set auto reconnect.
        std::string host = _config.get<std::string>("Santiago.SantiagoDBTables.host");
        std::string user = _config.get<std::string>("Santiago.SantiagoDBTables.user");
        std::string password = _config.get<std::string>("Santiago.SantiagoDBTables.password");
        std::string db = _config.get<std::string>("Santiago.SantiagoDBTables.db");

        if(mysql_real_connect(_mysql,
                              host.c_str(),
                              user.c_str(),
                              password.c_str(),
                              db.c_str(),
                              _config.get<unsigned>("Santiago.SantiagoDBTables.port"),
                              NULL,
                              0) == NULL)
        {
            ST_LOG_ERROR("mysql_real_connect() failed. host = " 
                         << config_.get<const char*>("Santiago.SantiagoDBTables.host")
                         <<" user = " << config_.get<const char*>("Santiago.SantiagoDBTables.user")
                         <<" db = " << config_.get<const char*>("Santiago.SantiagoDBTables.db")
                         <<" port = " << config_.get<unsigned>("Santiago.SantiagoDBTables.port") << std::endl);

            error_ = std::error_code(ERR_DATABASE_EXCEPTION, ErrorCategory::GetInstance());
	    BOOST_ASSERT(false);
        }
        
        ST_LOG_INFO("mysql_real_connect() succeeded." << config_.get<const char*>("Santiago.SantiagoDBTables.host")
                    <<" user = " << config_.get<const char*>("Santiago.SantiagoDBTables.user")
                    <<" db = " << config_.get<const char*>("Santiago.SantiagoDBTables.db") << std::endl);
       
        error_ = std::error_code(ERR_SUCCESS, ErrorCategory::GetInstance());
    }

    void MariaDBConnection::disconnect(std::error_code& error_)
    {
        ST_LOG_INFO("Closing db connection..." << std::endl);
        mysql_close(_mysql);
        error_ = std::error_code(ERR_SUCCESS, ErrorCategory::GetInstance());
    }

    std::string MariaDBConnection::getEscapedString(const std::string& nonEscapedString_)
    {
        if(4096 > ((nonEscapedString_.size()*2) + 1))
        {//so as to avoid calling new for small strings
            char escapedUserInput[4096] = "\0";
            mysql_real_escape_string(_mysql,escapedUserInput,nonEscapedString_.c_str(),nonEscapedString_.size());
            return std::string(escapedUserInput);
        }
        else
        {
            std::shared_ptr<char> escapedUserInput(new char[nonEscapedString_.size()*2 + 1]);
            mysql_real_escape_string(_mysql,escapedUserInput.get(),nonEscapedString_.c_str(),nonEscapedString_.size());
            return std::string(escapedUserInput.get());
        }
    }

    bool MariaDBConnection::isUserInputClean(const std::string& userInput_)
    {
        return (userInput_ == getEscapedString(userInput_));
    }

    void MariaDBConnection::runQueryImpl(const std::string& queryString_, std::error_code& error_)
    {
        ST_LOG_INFO("Running query:" << std::endl
                    << queryString_ << std::endl);
        
        if(mysql_query(_mysql, queryString_.c_str()) ||
           (0 != mysql_errno(_mysql)))
        {
            ST_LOG_DEBUG("Db error:"<< mysql_error(_mysql) << std::endl);
            error_ = std::error_code(ERR_DATABASE_EXCEPTION, ErrorCategory::GetInstance());
            return;
        }

        error_ = std::error_code(ERR_SUCCESS, ErrorCategory::GetInstance());
        return;
    }

    int MariaDBConnection::runInsertQuery(const std::string& queryString_, std::error_code& error_)
    {    
        ST_LOG_INFO("Running insert query:" << std::endl);
        runQueryImpl(queryString_, error_);
        if(error_)
        {
            return INVALID_DATABASE_ID;
        }

        if(0 == mysql_affected_rows(_mysql))
        {
            ST_LOG_DEBUG("mysql_affected_rows() returns 0.");
            error_ = std::error_code(ERR_DATABASE_EXCEPTION, ErrorCategory::GetInstance());
            return INVALID_DATABASE_ID;
        }

        int ret = mysql_insert_id(_mysql);
        error_ = std::error_code(ERR_SUCCESS, ErrorCategory::GetInstance());
        ST_LOG_INFO("Insert query successful." << std::endl);

        return ret;
    }

    void MariaDBConnection::runSelectQuery(const std::string& queryString_,
                                           const std::function<void(MYSQL_RES *, std::error_code&)>& postQueryFn_,
                                           std::error_code& error_)
    {
        ST_LOG_INFO("Running select query:" << std::endl);
        runQueryImpl(queryString_, error_);
        if(error_)
        {
            return;
        }

        MYSQL_RES *result = mysql_store_result(_mysql);
        if(result == NULL)
        {
            ST_LOG_DEBUG("mysql_store_result() returned null." << std::endl);
            error_ = std::error_code(ERR_DATABASE_EXCEPTION, ErrorCategory::GetInstance());
            return;
        }

        if(0 != mysql_num_rows(result))
        {
            ST_LOG_INFO("Results found for select query." << std::endl);
            postQueryFn_(result, error_); //error code to be set inside the postQueryFn
        }
        else
        {
            ST_LOG_DEBUG("mysql_num_rows() returned 0." << std::endl);
            error_ = std::error_code(ERR_SUCCESS, ErrorCategory::GetInstance());
        }
        mysql_free_result(result);
        return;
    }

    void MariaDBConnection::runUpdateQuery(const std::string& queryString_, std::error_code& error_)
    {
        ST_LOG_INFO("Running update query:" << std::endl);
        runQueryImpl(queryString_, error_);
        if(error_)
        {
            return;
        }

        if(0 == mysql_affected_rows(_mysql))
        {
            ST_LOG_DEBUG("mysql_affected_rows() returns 0." << std::endl);
            error_ = std::error_code(ERR_DATABASE_EXCEPTION, ErrorCategory::GetInstance());
            return;
        }

        error_ = std::error_code(ERR_SUCCESS, ErrorCategory::GetInstance());
        ST_LOG_INFO("Update query successful." << std::endl);
        return;
    }

    void MariaDBConnection::runDeleteQuery(const std::string& queryString_, std::error_code& error_)
    {
        ST_LOG_INFO("Running delete query:" << std::endl);
        runQueryImpl(queryString_, error_);
        if(error_)
        {
            return;
        }

        if(0 == mysql_affected_rows(_mysql))
        {
            ST_LOG_DEBUG("mysql_affected_rows() returns 0." << std::endl);
            error_ = std::error_code(ERR_DATABASE_EXCEPTION, ErrorCategory::GetInstance());
            return;
        }

        error_ = std::error_code(ERR_SUCCESS, ErrorCategory::GetInstance());
        ST_LOG_INFO("Delete query successful." << std::endl);
        return;
    }

    void MariaDBConnection::addUserProfilesRec(UserProfilesRec& userProfilesRec_, std::error_code& error_)
    {
        if(!isUserInputClean(userProfilesRec_._userName) ||
           !isUserInputClean(userProfilesRec_._emailAddress) ||
           !isUserInputClean(userProfilesRec_._password))
        {
            error_ = std::error_code(ERR_DATABASE_INVALID_USER_INPUT, ErrorCategory::GetInstance());
            return;
        }

        std::string addUserProfilesRecQuery = "INSERT INTO ST_users(user_name, email_address, password) VALUES('" +
            userProfilesRec_._userName + "', '" + userProfilesRec_._emailAddress + "', '" + userProfilesRec_._password + "')";
        userProfilesRec_._id = runInsertQuery(addUserProfilesRecQuery, error_);
    }

    boost::optional<UserProfilesRec> MariaDBConnection::getUserProfilesRecForUserName(
        const std::string& userName_,
        std::error_code& error_)
    {
        if(!isUserInputClean(userName_))
        {
            error_ = std::error_code(ERR_DATABASE_INVALID_USER_INPUT, ErrorCategory::GetInstance());
            return boost::none;
        }

        std::string getUserProfilesRecQuery = "SELECT * FROM ST_users WHERE user_name = '" + userName_ + "'";
        return getUserProfilesRecImpl(getUserProfilesRecQuery,error_); 
    }

    boost::optional<UserProfilesRec> MariaDBConnection::getUserProfilesRecForEmailAddress(
        const std::string& emailAddress_,
        std::error_code& error_)
    {
        if(!isUserInputClean(emailAddress_))
        {
            error_ = std::error_code(ERR_DATABASE_INVALID_USER_INPUT, ErrorCategory::GetInstance());
            return boost::none;
        }

        std::string getUserProfilesRecQuery = "SELECT * FROM ST_users WHERE email_address = '" + emailAddress_ + "'";
        return getUserProfilesRecImpl(getUserProfilesRecQuery,error_); 
    }

    boost::optional<UserProfilesRec> MariaDBConnection::getUserProfilesRecImpl(const std::string& queryString_,
                                                                               std::error_code& error_)
    {
        boost::optional<UserProfilesRec> userProfilesRec = UserProfilesRec();
        runSelectQuery(
            queryString_,
            [&userProfilesRec](MYSQL_RES* mysqlResult_, std::error_code& error_)
            {
                if(mysql_num_rows(mysqlResult_) > 1)
                {
                    ST_LOG_DEBUG("More than 1 records with same username in the ST_users table"
                                 << std::endl);
                    error_ = std::error_code(ERR_DATABASE_EXCEPTION, ErrorCategory::GetInstance());
                    BOOST_ASSERT(false);                    
                }
                
                MYSQL_ROW row = mysql_fetch_row(mysqlResult_);
                BOOST_ASSERT(NULL != row);
                
                userProfilesRec->_id = atoi(row[0]);
                userProfilesRec->_userName = row[1];
                userProfilesRec->_emailAddress = row[2];
                userProfilesRec->_password = row[3];
                error_ = std::error_code(ERR_SUCCESS, ErrorCategory::GetInstance());
            },
            error_);

        return userProfilesRec;
    }

    void MariaDBConnection::updateUserProfilesRec(UserProfilesRec& newUserProfilesRec_, std::error_code& error_)
    {
        if(!isUserInputClean(newUserProfilesRec_._userName) ||
           !isUserInputClean(newUserProfilesRec_._emailAddress) ||
           !isUserInputClean(newUserProfilesRec_._password))
        {
            error_ = std::error_code(ERR_DATABASE_INVALID_USER_INPUT, ErrorCategory::GetInstance());
            return;
        }

        std::string updateUserProfilesRecQuery = "UPDATE ST_users SET password ='" +
            newUserProfilesRec_._password + "', email_address = '" + newUserProfilesRec_._emailAddress +
            "' WHERE user_name = '" + newUserProfilesRec_._userName +"'";
        runUpdateQuery(updateUserProfilesRecQuery,error_);
    }

    void MariaDBConnection::deleteUserProfilesRec(const std::string& userName_,std::error_code& error_)
    {
        if(!isUserInputClean(userName_))
        {
            error_ = std::error_code(ERR_DATABASE_INVALID_USER_INPUT, ErrorCategory::GetInstance());
            return;
        }

        std::string deleteUserProfilesRecQuery = "DELETE FROM ST_users WHERE user_name = '" +
            userName_ + "'";
        runDeleteQuery(deleteUserProfilesRecQuery, error_);
    }

    void MariaDBConnection::addSessionsRec(SessionsRec& sessionsRec_, std::error_code& error_)
    {
        if(!isUserInputClean(sessionsRec_._userName) ||
           !isUserInputClean(sessionsRec_._cookieString))
        {
            error_ = std::error_code(ERR_DATABASE_INVALID_USER_INPUT, ErrorCategory::GetInstance());
            return;
        }

        std::string addSessionsRecQuery =
            "INSERT INTO ST_sessions(user_name, cookie_string, login_time, logout_time, last_active_time) values('" +
            sessionsRec_._userName + "', '" +
            sessionsRec_._cookieString + "', '" +
            Utils::ConvertPtimeToString(sessionsRec_._loginTime) + "', '" +
            (sessionsRec_._logoutTime? Utils::ConvertPtimeToString(*(sessionsRec_._logoutTime)) : "NULL") + "', '" +
            Utils::ConvertPtimeToString(sessionsRec_._lastActiveTime) + "')";
            
        sessionsRec_._id = runInsertQuery(addSessionsRecQuery,error_);
    }

    boost::optional<SessionsRec> MariaDBConnection::getSessionsRec(const std::string& cookieString_,
                                                                   std::error_code& error_)
    {
        if(!isUserInputClean(cookieString_))
        {
            error_ = std::error_code(ERR_DATABASE_INVALID_USER_INPUT, ErrorCategory::GetInstance());
            return boost::none;
        }

        std::string getSessionsRecQuery = "SELECT * FROM ST_sessions WHERE cookie_string = '" + cookieString_ + "'";
        boost::optional<SessionsRec> sessionsRec = SessionsRec();

        runSelectQuery(
            getSessionsRecQuery,
            [&sessionsRec](MYSQL_RES* mysqlResult_, std::error_code& error_)
            {
                if(mysql_num_rows(mysqlResult_) > 1)
                {
                    ST_LOG_DEBUG("More than 1 records with same cookie_string in the ST_sessions table "
                                 << std::endl);
                    error_ = std::error_code(ERR_DATABASE_EXCEPTION, ErrorCategory::GetInstance());
                    BOOST_ASSERT(false);
                }
                
                MYSQL_ROW row = mysql_fetch_row(mysqlResult_);
                BOOST_ASSERT(NULL != row);

                sessionsRec->_id = atoi(row[0]);
                sessionsRec->_userName = row[1];
                sessionsRec->_cookieString = row[2];
                sessionsRec->_loginTime = Utils::ConvertStringToPtime(row[3]);

                if(NULL != row[4])
                {
                    sessionsRec->_logoutTime = Utils::ConvertStringToPtime(row[4]);
                }

                sessionsRec->_lastActiveTime = Utils::ConvertStringToPtime(row[5]);

                error_ = std::error_code(ERR_SUCCESS, ErrorCategory::GetInstance());
            },
            error_);

        return sessionsRec;
    }
    void MariaDBConnection::updateSessionsRec(SessionsRec& sessionsRec_, std::error_code& error_)
    {
        if(!isUserInputClean(sessionsRec_._cookieString))
        {
            error_ = std::error_code(ERR_DATABASE_INVALID_USER_INPUT, ErrorCategory::GetInstance());
            return;
        }

        std::string updateSessionsRecQuery = "UPDATE ST_sessions SET logout_time = '" +
            (sessionsRec_._logoutTime? Utils::ConvertPtimeToString(*(sessionsRec_._logoutTime)) : "NULL") + "', " + 
            "last_active_time = '" + Utils::ConvertPtimeToString(sessionsRec_._lastActiveTime) +
            "' WHERE cookie_string ='" +
            sessionsRec_._cookieString + "'";
        runUpdateQuery(updateSessionsRecQuery,error_);
    }

    MariaDBConnection* CreateMariaDBConnection(const boost::property_tree::ptree& config_)
    {
        return new MariaDBConnection(config_);
    }
}}
