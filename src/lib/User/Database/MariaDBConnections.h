#ifndef MARIADBCONNECTIONS_H
#define MARIADBCONNECTIONS_H

#include "Records.h"
#include <mysql.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <map>

namespace Santiago{ namespace Database
{
    class MariaDBConnections
    {
    public:
        std::map<UserPermission, std::string> _permission;
        
        MariaDBConnections();
        bool addUserProfileRecord(const std::string, const std::string);
        bool checkUserProfileRecord(const std::string, const std::string);
        bool updateUserPassword(const std::string, const std::string, const std::string);
        bool addSessionRecord(const std::string, const std::string, ptime);
        bool updateSessionRecord(const std::string, ptime);
        bool addPermissionRecord(int, const std::string, UserPermission);
        std::vector<UserProfile> getUserProfileRecord();
        std::vector<Session> getSessionRecord();
        std::vector<Permission> getPermissionRecord();

    private:
        int _userProfileId, _sessionId, _permissionId;
    };
}}

#endif
