#ifndef SANTIAGO_SANTIAGODBTABLES_DEFINITIONS_H
#define SANTIAGO_SANTIAGODBTABLES_DEFINITIONS_H

#include <string>
#include <map>

namespace Santiago{ namespace SantiagoDBTables
{
    enum UserPermission 
    {
        OWNER,
        READ,
        WRITE,
        READ_WRITE
    };

    extern const std::map<UserPermission, std::string> userPermissionString;
    extern const std::map<std::string, UserPermission> stringUserPermission;
    extern const std::map<std::string, std::string> alphabetDigit;
    extern const std::map<std::string, std::string> digitAlphabet;

}}

#endif    
