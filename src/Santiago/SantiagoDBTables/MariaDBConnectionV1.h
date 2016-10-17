#ifndef SANTIAGO_SANTIAGODBTABLES_MARIADBCONNECTION_H
#define SANTIAGO_SANTIAGODBTABLES_MARIADBCONNECTION_H

#include <functional>
#include <iostream>
#include <vector>
#include <sstream>

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

        //take db ip, port, username, password from config
        MariaDBConnection(const boost::property_tree::ptree& config_)
        {
            /*  MYSQL *_mysql = config_.get<MYSQL*>("Santiago.SantiagoDBTables.mysql");
            const char *_host = config_.get<const char*>("Santiago.SantiagoDBTables.host");
            const char *_user = config_.get<const char*>("Santiago.SantiagoDBTables.user");
            const char *_passwd = config_.get<const char*>("Santiago.SantiagoDBTables.passwd");
            const char *_db = config_.get<const char*>("Santiago.SantiagoDBTables.db");
            unsigned int _port = config_.get<unsigned int>("Santiago.SantiagoDBTables.port");
            const char * _unixSocket = config_.get<const char*>("Santiago.SantiagoDBTables.unix_socket");
            unsigned long _flags = config_.get<unsigned long>("Santiago.SantiagoDBTables.flags");*/
	    
            connect();
         }

        virtual ~MariaDBConnection();
        

        // possible error_code returns for the following fns
        // SUCCESS, DATABASE_EXCEPTION, DATABASE_QUERY_FAILED
        
        void addUserProfilesRec(UserProfilesRec& userProfilesRec_,std::error_code& error_);
        boost::optional<UserProfilesRec> getUserProfilesRec(const std::string& userName_,std::error_code& error_);
        void updateUserProfilesRec(UserProfilesRec& newUserProfilesRec_,std::error_code& error_);
        void deleteUserProfilesRec(const std::string& userName_,std::error_code& error_);

        void addSessionsRec(SessionsRec& sessionsRec_,std::error_code& error_);
        boost::optional<SessionsRec> getSessionsRec(const std::string& userName_,std::error_code& error_);
        void updateSessionsRec(SessionsRec& userProfilesRec_,std::error_code& error_);

    protected:
     
        void connect(std::error_code& error_);
        void disconnect(std::error_code& error_);


        void runQueryImpl(const std::string& queryString_,std::error_code& error_);
        int runInsertQuery(const std::string& queryString_,std::error_code& error_);
        void runSelectQuery(const std::string& queryString_,
                            const std::function<void(MYSQL_RES *, std::error_code&)>& postQueryFn_,
                            std::error_code& error_);
        void runUpdateQuery(const std::string& queryString_,std::error_code& error_);
        void runDeleteQuery(const std::string& queryString_,std::error_code& error_);
        
        std::string convertPtimeToString(const boost::posix_time::ptime& ptime_) const;
        boost::posix_time::ptime convertStringToPtime(const std::string& timeStr_) const;

        boost::property_tree::ptree   _config;
        MYSQL                        *_mysql;

    };
}}

#endif
