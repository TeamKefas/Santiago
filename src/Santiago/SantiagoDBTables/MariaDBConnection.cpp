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

        ST_ASSERT(_mysql != NULL); //This should be very rare. hence BOOST_ASSERT.

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
                         << host
                         <<" user = " << user
                         <<" db = " << db
                         <<" port = " << _config.get<unsigned>("Santiago.SantiagoDBTables.port") << std::endl);

            error_ = std::error_code(ERR_DATABASE_EXCEPTION, ErrorCategory::GetInstance());
	    ST_ASSERT(false);
        }
        
        ST_LOG_INFO("mysql_real_connect() succeeded. host = " << host
                    <<" user = " << user
                    <<" db = " << db);
                    
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
            error_ = std::error_code(ERR_DATABASE_INVALID_USER_INPUT, ErrorCategory::GetInstance());
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

    void MariaDBConnection::addUsersRec(UsersRec& usersRec_, std::error_code& error_)
    {
        if(!isUserInputClean(usersRec_._userName) ||
           !isUserInputClean(usersRec_._emailAddress) ||
           !isUserInputClean(usersRec_._password))
        {
            error_ = std::error_code(ERR_DATABASE_INVALID_USER_INPUT, ErrorCategory::GetInstance());
            return;
        }

        std::string addUsersRecQuery = "INSERT INTO ST_users(user_name, email_address, password) VALUES('" +
            usersRec_._userName + "', '" + usersRec_._emailAddress + "', '" + usersRec_._password + "')";
        usersRec_._id = runInsertQuery(addUsersRecQuery, error_);
    }

    boost::optional<UsersRec> MariaDBConnection::getUsersRecForUserName(
        const std::string& userName_,
        std::error_code& error_)
    {
        if(!isUserInputClean(userName_))
        {
            error_ = std::error_code(ERR_DATABASE_INVALID_USER_INPUT, ErrorCategory::GetInstance());
            return boost::none;
        }

        std::string getUsersRecQuery = "SELECT id, "
            "user_name, "
            "email_address, "
            "password "
            "FROM ST_users WHERE user_name = '" + userName_ + "'";
        return getUsersRecImpl(getUsersRecQuery,error_); 
    }

    boost::optional<UsersRec> MariaDBConnection::getUsersRecForEmailAddress(
        const std::string& emailAddress_,
        std::error_code& error_)
    {
        if(!isUserInputClean(emailAddress_))
        {
            error_ = std::error_code(ERR_DATABASE_INVALID_USER_INPUT, ErrorCategory::GetInstance());
            return boost::none;
        }

        std::string getUsersRecQuery = "SELECT id, "
            "user_name, "
            "email_address, "
            "password "
            "FROM ST_users WHERE email_address = '" + emailAddress_ + "'";
        return getUsersRecImpl(getUsersRecQuery,error_); 
    }

    boost::optional<UsersRec> MariaDBConnection::getUsersRecImpl(const std::string& queryString_,
                                                                               std::error_code& error_)
    {
        boost::optional<UsersRec> usersRec;
        runSelectQuery(
            queryString_,
            [&usersRec](MYSQL_RES* mysqlResult_, std::error_code& error_)
            {
                if(mysql_num_rows(mysqlResult_) > 1)
                {
                    ST_LOG_DEBUG("More than 1 records with same username in the ST_users table"
                                 << std::endl);
                    error_ = std::error_code(ERR_DATABASE_EXCEPTION, ErrorCategory::GetInstance());
                    ST_ASSERT(false);
                    return;
                }

                if(4 != mysql_num_fields(mysqlResult_))
                {
                    ST_LOG_DEBUG("Mismatch in number of fields in the ST_users table");
                    error_ = std::error_code(ERR_DATABASE_EXCEPTION, ErrorCategory::GetInstance());
                    //  ST_ASSERT(false);
                    return;
                }
                
                MYSQL_ROW row = mysql_fetch_row(mysqlResult_);
                ST_ASSERT(NULL != row);

                usersRec = UsersRec();
                usersRec->_id = atoi(row[0]);
                usersRec->_userName = row[1];
                usersRec->_emailAddress = row[2];
                usersRec->_password = row[3];
                error_ = std::error_code(ERR_SUCCESS, ErrorCategory::GetInstance());
            },
            error_);

        return usersRec;
    }

    void MariaDBConnection::updateUsersRec(UsersRec& newUsersRec_, std::error_code& error_)
    {
        if(!isUserInputClean(newUsersRec_._userName) ||
           !isUserInputClean(newUsersRec_._emailAddress) ||
           !isUserInputClean(newUsersRec_._password))
        {
            error_ = std::error_code(ERR_DATABASE_INVALID_USER_INPUT, ErrorCategory::GetInstance());
            return;
        }

        std::string updateUsersRecQuery = "UPDATE ST_users SET password ='" +
            newUsersRec_._password + "', email_address = '" + newUsersRec_._emailAddress +
            "' WHERE user_name = '" + newUsersRec_._userName +"'";
        runUpdateQuery(updateUsersRecQuery,error_);
    }

    void MariaDBConnection::deleteUsersRec(const std::string& userName_,std::error_code& error_)
    {
        if(!isUserInputClean(userName_))
        {
            error_ = std::error_code(ERR_DATABASE_INVALID_USER_INPUT, ErrorCategory::GetInstance());
            return;
        }

        std::string deleteUsersRecQuery = "DELETE FROM ST_users WHERE user_name = '" +
            userName_ + "'";
        runDeleteQuery(deleteUsersRecQuery, error_);
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
            Utils::ConvertPtimeToString(sessionsRec_._loginTime) + "', " +
            (sessionsRec_._logoutTime? "'" + Utils::ConvertPtimeToString(*(sessionsRec_._logoutTime)) + "'" : "NULL") + ", '" +
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

        std::string getSessionsRecQuery = "SELECT id, "
            "user_name, "
            "cookie_string, "
            "login_time, "
            "logout_time, "
            "last_active_time "
            "FROM ST_sessions WHERE cookie_string = '" + cookieString_ + "'";
        boost::optional<SessionsRec> sessionsRec;

        runSelectQuery(
            getSessionsRecQuery,
            [&sessionsRec](MYSQL_RES* mysqlResult_, std::error_code& error_)
            {
                if(mysql_num_rows(mysqlResult_) > 1)
                {
                    ST_LOG_DEBUG("More than 1 records with same cookie_string in the ST_sessions table "
                                 << std::endl);
                    error_ = std::error_code(ERR_DATABASE_EXCEPTION, ErrorCategory::GetInstance());
                    ST_ASSERT(false);
                    return;
                }

                if(6 != mysql_num_fields(mysqlResult_))
                {
                    ST_LOG_DEBUG("Mismatch in number of fields in the ST_sessions table");
                    error_ = std::error_code(ERR_DATABASE_EXCEPTION, ErrorCategory::GetInstance());
                    ST_ASSERT(false);
                    return;
                }
                
                MYSQL_ROW row = mysql_fetch_row(mysqlResult_);
                ST_ASSERT(NULL != row);

                sessionsRec = SessionsRec();
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

        std::string updateSessionsRecQuery = "UPDATE ST_sessions SET logout_time = " +
            (sessionsRec_._logoutTime? "'" + Utils::ConvertPtimeToString(*(sessionsRec_._logoutTime)) + "'" : "NULL") + ", " + 
            "last_active_time = '" + Utils::ConvertPtimeToString(sessionsRec_._lastActiveTime) +
            "' WHERE cookie_string ='" +
            sessionsRec_._cookieString + "'";
        runUpdateQuery(updateSessionsRecQuery,error_);
    }

    std::vector<SessionsRec> MariaDBConnection::getActiveSessions(std::error_code& error_)
    {
        // ptime logoutTime;//(from_iso_string("00000000T000000"));
        std::string getActiveSessionsQuery =  "SELECT id, "
            "user_name, "
            "cookie_string, "
            "login_time, "
            "logout_time, "
            "last_active_time "
            "FROM ST_sessions WHERE logout_time IS NULL";
        std::vector<SessionsRec> sessionsRecs;
        runSelectQuery(
            getActiveSessionsQuery,
            [&sessionsRecs](MYSQL_RES* mysqlResult_, std::error_code& error_)
            {
                if(6 != mysql_num_fields(mysqlResult_))
                {
                    ST_LOG_DEBUG("Mismatch in number of fields in the ST_sessions table."<< std::endl);
                    error_ = std::error_code(ERR_DATABASE_EXCEPTION, ErrorCategory::GetInstance());
                    ST_ASSERT(false);
                    return;
                }
                
                MYSQL_ROW row;
                while ((row = mysql_fetch_row(mysqlResult_)))
                {
                    ST_ASSERT(NULL != row);
                    SessionsRec sessionsRec;
                    sessionsRec = SessionsRec();
                    sessionsRec._id = atoi(row[0]);
                    sessionsRec._userName = row[1];
                    sessionsRec._cookieString = row[2];
                    sessionsRec._loginTime = Utils::ConvertStringToPtime(row[3]);
                    if(row[4])
                        sessionsRec._logoutTime = Utils::ConvertStringToPtime(row[4]);
                    if(row[5])
                        sessionsRec._lastActiveTime = Utils::ConvertStringToPtime(row[5]);
                    sessionsRecs.push_back(sessionsRec);
                }
                error_ = std::error_code(ERR_SUCCESS, ErrorCategory::GetInstance());
            },
            error_);
        return sessionsRecs;
    }

    MariaDBConnection* CreateMariaDBConnection(const boost::property_tree::ptree& config_)
    {
        return new MariaDBConnection(config_);
    }
}}
