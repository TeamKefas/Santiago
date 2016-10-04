#include "Definitions.h"

namespace Santiago{ namespace SantiagoDBTables
{
    const std::map<UserPermission, std::string> userPermissionString = 
    {{UserPermission::OWNER, "OWNER"}, 
     {UserPermission::READ, "READ"},
     {UserPermission::WRITE, "WRITE"},
     {UserPermission::READ_WRITE, "READ_WRITE"}};

    const std::map<std::string, UserPermission> stringUserPermission = 
    {{"OWNER", UserPermission::OWNER}, 
     {"READ", UserPermission::READ},
     {"WRITE", UserPermission::WRITE},
     {"READ_WRITE", UserPermission::READ_WRITE}};
    
    const std::map<std::string, std::string> alphabetDigit = 
    {{"Jan", "01"},
     {"Feb", "02"},
     {"Mar", "03"},
     {"Apr", "04"},
     {"May", "05"},
     {"Jun", "06"},
     {"Jul", "07"},
     {"Aug", "08"},
     {"Sep", "09"},
     {"Oct", "10"},
     {"Nov", "11"},
     {"Dec", "12"}};
    
    const std::map<std::string, std::string> digitAlphabet = 
    {{"01", "Jan"},
     {"02", "Feb"},
     {"03", "Mar"},
     {"04", "Apr"},
     {"05", "May"},
     {"06", "Jun"},
     {"07", "Jul"},
     {"08", "Aug"},
     {"09", "Sep"},
     {"10", "Oct"},
     {"11", "Nov"},
     {"12", "Dec"}};
}}
