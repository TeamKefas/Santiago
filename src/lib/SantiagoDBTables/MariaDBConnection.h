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

namespace Santiago{ namespace SantiagoDBTables
{
    class MariaDBConnection
    {
    public:

        //take db ip, port, username, password from config
        MariaDBConnection(const boost::property_tree::config& config_){};

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
        MYSQL *con;

        //attempt to connect 5 times before return DATABASE_EXCEPTION
        std::error_code connect();
        bool isConnected();
        std::error_code disconnect();
        
    };
}}

#endif
