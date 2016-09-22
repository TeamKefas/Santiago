#ifndef RECORDS_H
#define RECORDS_H

#include <string>

#include <boost/date_time/posix_time/posix_time.hpp>

namespace Santiago{ namespace Database
{
    using namespace boost::posix_time;
        
    enum UserPermission 
    {
        OWNER,
        READ,
        WRITE,
        READ_WRITE
    };
    
    struct UserProfile
    {
        int _id;
        std::string _userName;
        std::string _password;
    };
    
    struct Session
    {
        int _id;
        std::string _userName;
        std::string _cookieId;
        ptime _loginTime;
        ptime _logoutTime;  
    };
        
    struct Permission
    {
        int _id;
        int _resId;
        std::string _userName;
        UserPermission  _userPermission;
    };
}}

#endif
