#include "Definitions.h"



namespace Santiago{ namespace SantiagoDBTables
{
    const std::map<UserPermission, std::string> userPermissionString = 
    {{OWNER, "OWNER"}, 
     {READ, "READ"},
     {WRITE, "WRITE"},
     {READ_WRITE, "READ_WRITE"}};

    const std::map<std::string, UserPermission> stringUserPermission = 
    {{"OWNER", OWNER}, 
     {"READ", READ},
     {"WRITE", WRITE},
     {"READ_WRITE", READ_WRITE}};
    
}}
