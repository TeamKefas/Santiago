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
        UserProfilesRec():_id(INVALID_DATABASE_ID)
        {}

        int         _id;
        std::string _userName;
        std::string _emailAddress;
        std::string _password;
    };
    
    struct SessionsRec
    {
        SessionsRec():_id(INVALID_DATABASE_ID)
        {}

        int                      _id;
        std::string              _userName;
        std::string              _cookieString;
        ptime                    _loginTime;
        boost::optional<ptime>   _logoutTime;
        ptime                    _lastActiveTime;
    };
        
    struct PermissionsRec
    {
        PermissionsRec():_id(INVALID_DATABASE_ID)
        {}

        int             _id;
        int             _resId;
        std::string     _userName;
        UserPermission  _userPermission;
    };
}}

#endif
