#ifndef SANTIAGO_SANTIAGODBTABLES_DATABASERECORDS_H
#define SANTIAGO_SANTIAGODBTABLES_DATABASERECORDS_H

/**
 * @file DatabaseRecord.h
 *
 * @section DESCRIPTION
 *
 * Contains the UsersRec SessionsRec PermissionsRec  class.
 */

#include <string>

#include "Definitions.h"

#include <boost/date_time/posix_time/posix_time.hpp>

namespace Santiago{ namespace SantiagoDBTables
{
    using namespace boost::posix_time;

/**
 * This structure stores the login credentials. 
 */
            
    struct UsersRec
 /**
  *The constructor
  */
    {
        UsersRec():_id(INVALID_DATABASE_ID)
        {}

        int         _id;
        std::string _userName;
        std::string _emailAddress;
        std::string _password;
        std::string _recoveryString;
    };

/**
 * This structure stores the session details.
 */
    
    struct SessionsRec
    {
/**
  *The constructor
  */
        SessionsRec():_id(INVALID_DATABASE_ID)
        {}

        int                      _id;
        std::string              _userName;
        std::string              _cookieString;
        ptime                    _loginTime;
        boost::optional<ptime>   _logoutTime;
        ptime                    _lastActiveTime;
    };

/**
 * This structure stores the access permissions for users and resources.
 */
        
    struct PermissionsRec
    {
/**
  *The constructor
  */
        PermissionsRec():_id(INVALID_DATABASE_ID)
        {}

        int             _id;
        int             _resId;
        std::string     _userName;
        UserPermission  _userPermission;
    };
}}

#endif
