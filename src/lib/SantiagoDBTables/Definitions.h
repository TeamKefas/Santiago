#ifndef SANTIAGO_SANTIAGODBTABLES_DEFINITIONS_H
#define SANTIAGO_SANTIAGODBTABLES_DEFINITIONS_H

#include <string>
#include <map>

const extern std::map<UserPermission, std::string> userPermissionString;
const extern std::map<std::string, UserPermission> stringUserPermission;
const extern std::map<std::string, std::string> alphabetDigit;
const extern std::map<std::string, std::string> digitAlphabet;

namespace Santiago{ namespace SantiagoDBTables
{
    enum UserPermission 
    {
        OWNER,
        READ,
        WRITE,
        READ_WRITE
    };
}}

#endif    
