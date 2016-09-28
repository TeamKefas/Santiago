#ifndef SANTIAGO_SANTIAGODBTABLES_MARIADBCONNECTION_H
#define SANTIAGO_SANTIAGODBTABLES_MARIADBCONNECTION_H

#include "Records.h"
#include <mysql.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <map>

#include <boost/optional.hpp>

namespace Santiago{ namespace SantiagoDBTables
{
    class MariaDBConnection
    {
    public:
        std::map<UserPermission, std::string> _permission;
        
        MariaDBConnection();

        bool connect();
        bool disconnect();
        
        bool addUserProfileRecord(const std::string userName_, const std::string password_);
        bool updateUserPassword(const std::string userId_,
                                const std::string oldPassword_, const std::string newPassword_);
        bool addSessionRecord(const std::string userName_,
                              const std::string cookieId_, ptime loginTime_);
        bool updateSessionRecord(const std::string userId_, ptime logoutTime_);
        bool addPermissionRecord(int resId_,std::string userName_, UserPermission permission_);
        bool getUserProfileRecord(std::string userName_, boost::optional<UserProfile>&);
        bool getSessionRecord(std::string userName_, boost::optional<Session>&);
        bool getPermissionRecord(std::string userName_, boost::optional<Permission>&);
        
    private:
        MYSQL* con;
        int _userProfileId, _sessionId, _permissionId;
    };
}}

#endif
