#ifndef SANTIAGO_SANTIAGODBTABLES_DATABASERECORDS_H
#define SANTIAGO_SANTIAGODBTABLES_DATABASERECORDS_H

#include <string>

#include "Definitions.h"

#include <boost/date_time/posix_time/posix_time.hpp>

namespace Santiago{ namespace SantiagoDBTables
{
    using namespace boost::posix_time;
            
    struct UserProfilesRec
    {
        int _id;
        std::string _userName;
        std::string _password;
    };
    
    struct SessionsRec
    {
        int _id;
        std::string _userName;
        std::string _cookieId;
        ptime _loginTime;
        ptime _logoutTime;  
    };
        
    struct PermissionsRec
    {
        int _id;
        int _resId;
        std::string _userName;
        UserPermission  _userPermission;
    };
}}

#endif
