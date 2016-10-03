#ifndef SANTIAGO_SANTIAGODBTABLES_MARIADBCONNECTION_H
#define SANTIAGO_SANTIAGODBTABLES_MARIADBCONNECTION_H

#include <mysql.h>
#include <iostream>
#include <vector>
#include <sstream>

#include <boost/optional.hpp>

#include "Records.h"
#include "Maps.h"

namespace Santiago{ namespace SantiagoDBTables
{
    class MariaDBConnection
    {
    public:
               
        MariaDBConnection(){};
        
        bool connect();
        void disconnect();
        
        bool addUserProfileRecord(const std::string userName_,
                                  const std::string password_);
        bool updateUserPassword(const std::string userId_,
                                const std::string oldPassword_, const std::string newPassword_);
        bool addSessionRecord(const std::string userName_,
                              const std::string cookieId_, ptime loginTime_);
        bool updateSessionRecord(const std::string userId_, ptime logoutTime_);
        bool addPermissionRecord(int resId_,
                                 const std::string userName_, UserPermission permission_);
        bool getUserProfileRecord(const std::string userName_, boost::optional<UserProfile>&);
        bool getSessionRecord(const std::string userName_, boost::optional<Session>&);
        bool getPermissionRecord(const std::string userName_, boost::optional<Permission>&);

/*
    public:

        //take db ip, port, username, password from config
        MariaDBConnection(const boost::property_tree::config& config_){};

        // possible error_code returns for the following fns
        // SUCCESS, DATABASE_EXCEPTION, DATABASE_QUERY_FAILED
        
        // set userProfilesRec_._id to the auto generated id from db
        std::error_code addUserProfilesRec(UserProfilesRec& userProfilesRec_);
        std::error_code getUserProfileRec(const std::string& userName_,
                                          boost::optional<UserProfilesRec>& userProfilesRec_);
        std::error_code updateUserProfilesRec(const UserProfilesRec& userProfilesRec_);
        std::error_code deleteUserProfilesRec(const std::string& userName_);

        // set sessionsRec_._id to the auto generated id from db
        std::error_code addSessionsRec(SessionsRec& sessionsRec_);
        std::error_code getSessionsRec(const std::string& cookieString_, boost::optional<SessionsRec>& sessionsRec_);
        std::error_code updateSessionsRec(SessionsRec& userProfilesRec_);
        
    private:
        
        //attempt to connect 5 times before return DATABASE_EXCEPTION
        std::error_code connect();
        bool isConnected();
        std::error_code disconnect();
*/       


    private:
        MYSQL *con;
        Maps _map;
    };
}}

#endif
