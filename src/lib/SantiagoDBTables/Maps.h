#ifndef SANTIAGO_SANTIAGODBTABLES_MAPS_H
#define SANTIAGO_SANTIAGODBTABLES_MAPS_H

#include <string>
#include <map>

#include "UserPermissions.h"

namespace Santiago{ namespace SantiagoDBTables
{
    class Maps
    {
    public:
        Maps();
        
        std::map<UserPermission, std::string> userPermissionString;
        std::map<std::string, UserPermission> stringUserPermission;
        std::map<std::string, std::string> alphabetDigit;
        std::map<std::string, std::string> digitAlphabet;
    };
}}

#endif
