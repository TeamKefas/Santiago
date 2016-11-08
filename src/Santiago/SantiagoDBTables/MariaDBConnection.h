#ifndef SANTIAGO_SANTIAGODBTABLES_MARIADBCONNECTION_H
#define SANTIAGO_SANTIAGODBTABLES_MARIADBCONNECTION_H

#include <functional>
#include <iostream>
#include <vector>
#include <sstream>
#include <cstring>
#include <algorithm>

#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>

#include <mysql.h>

#include "../ErrorCategory.h"

#include "DatabaseRecords.h"
#include "Definitions.h"

namespace Santiago{ namespace SantiagoDBTables
{
    class MariaDBConnection
    {
    public:

        MariaDBConnection(const boost::property_tree::ptree& config_);

        virtual ~MariaDBConnection();
        

        // possible error_code returns for the following fns
        // SUCCESS, DATABASE_EXCEPTION, DATABASE_QUERY_FAILED

        void addUsersRec(UsersRec& usersRec_, std::error_code& error_);
        boost::optional<UsersRec> getUsersRecForUserName(const std::string& userName_, std::error_code& error_);
        boost::optional<UsersRec> getUsersRecForEmailAddress(const std::string& userName_, std::error_code& error_);
        void updateUsersRec(UsersRec& newUsersRec_, std::error_code& error_);
        void deleteUsersRec(const std::string& userName_, std::error_code& error_);
        
        void addSessionsRec(SessionsRec& sessionsRec_, std::error_code& error_);
        boost::optional<SessionsRec> getSessionsRec(const std::string& cookieString_, std::error_code& error_);
        void updateSessionsRec(SessionsRec& sessionsRec_, std::error_code& error_);

    protected:
     
        void connect(std::error_code& error_);
        void disconnect(std::error_code& error_);

        boost::optional<UsersRec> getUsersRecImpl(
            const std::string& queryString_, std::error_code& error_);

        std::string getEscapedString(const std::string& nonEscapedString_);
        bool isUserInputClean(const std::string& userInput_);

        void runQueryImpl(const std::string& queryString_, std::error_code& error_);
        int runInsertQuery(const std::string& queryString_, std::error_code& error_);
        void runSelectQuery(const std::string& queryString_,
                            const std::function<void(MYSQL_RES *, std::error_code&)>& postQueryFn_,
                            std::error_code& error_);
        void runUpdateQuery(const std::string& queryString_, std::error_code& error_);
        void runDeleteQuery(const std::string& queryString_, std::error_code& error_);        

        boost::property_tree::ptree   _config;
        MYSQL                        *_mysql;

    };

    //Factory function for use in ThreadSpecificVar
    MariaDBConnection* CreateMariaDBConnection(const boost::property_tree::ptree& config_);

}}

#endif
