\#ifndef SANTIAGO_SANTIAGODBTABLES_MARIADBCONNECTION_H
#define SANTIAGO_SANTIAGODBTABLES_MARIADBCONNECTION_H

#include <mysql.h>
#include <iostream>
#include <vector>
#include <sstream>

#include <boost/optional.hpp>

#include "DatabaseRecords.h"
#include "Definitions.h"
#include "Error/ErrorCategory.h"

#include <boost/property_tree/ptree.hpp>

namespace Santiago{ namespace SantiagoDBTables
{
    class MariaDBConnection
    {
    public:

        //take db ip, port, username, password from config
        MariaDBConnection(const boost::property_tree::config& config_)
        {
            MYSQL *_mysql = config_.get<MYSQL*>("Santiago.SantiagoDBTables.mysql");
            const char *_host = config_.get<const char*>("Santiago.SantiagoDBTables.host");
            const char *_user = config_.get<const char*>("Santiago.SantiagoDBTables.user");
            const char *_passwd = config_.get<const char*>("Santiago.SantiagoDBTables.passwd");
            const char *_db = config_.get<const char*>("Santiago.SantiagoDBTables.db");
            unsigned int _port = config_.get<unsigned int>("Santiago.SantiagoDBTables.port");
            const char * _unixSocket = config_.get<const char*>("Santiago.SantiagoDBTables.unix_socket");
            unsigned long _flags = config_.get<unsigned long>("Santiago.SantiagoDBTables.flags");
        }
        ~MariaDBConnection()
        {
            disconnect();
        }
        

        // possible error_code returns for the following fns
        // SUCCESS, DATABASE_EXCEPTION, DATABASE_QUERY_FAILED
        
        // set userProfilesRec_._id to the auto generated id from db
        std::error_code addUserProfilesRec(UserProfilesRec& userProfilesRec_);
        std::error_code getUserProfileRec(const std::string& userName_,
                                          boost::optional<UserProfilesRec>& userProfilesRec_);
        std::error_code updateUserProfilesRec(const UserProfilesRec& userProfilesRec_, const std::string newPassword_);
        std::error_code deleteUserProfilesRec(const std::string& userName_);

        // set sessionsRec_._id to the auto generated id from db
        std::error_code addSessionsRec(SessionsRec& sessionsRec_);
        std::error_code getSessionsRec(const std::string& cookieString_, boost::optional<SessionsRec>& sessionsRec_);
        std::error_code updateSessionsRec(SessionsRec& userProfilesRec_);

        // set permissionsRec_._id to the auto generated id from db
        std::error_code addPermissionsRec(PermissionsRec& permissionsRec_);
        std::error_code getPermissionsRec(const std::string userName_, boost::optional<Permission>&);
        
    private:
        
        MYSQL *_mysql;
        const char *_host;
        const char *_user;
        const char *_passwd;
        const char *_db;
        unsigned int _port;
        const char *_unixSocket;
        unsigned long _flags;
        
        
        //attempt to connect 5 times before return DATABASE_EXCEPTION
        std::error_code connect();
        bool isConnected();
        std::error_code disconnect();        
    };
}}

#endif
