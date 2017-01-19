#ifndef SANTIAGO_SANTIAGODBTABLES_DEFINITIONS_H
#define SANTIAGO_SANTIAGODBTABLES_DEFINITIONS_H

/**
 * @file Definitions.h
 *
 * @section DESCRIPTION
 *
 * Contains an enum class UserPermission with different permission types. 
 */


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
    
    static const int INVALID_DATABASE_ID = -1;
}}

#endif    
