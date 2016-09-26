#ifndef SANTIAGO_AUTHENTICATION_DATABASEINTERFACE_H
#define SANTIAGO_AUTHENTICATION_DATABASEINTERFACE_H

#include <vector>
#include <cstdlib>
#include <string>
#include <sstream>

#include "Database/MariaDBConnections.h"
#include "ServerData.h"

namespace Santiago{ namespace Authentication
{
    using namespace boost::posix_time;
    class DatabaseInterface
    {
    public:
        
        bool createUser(const std::string& userId_, const std::string& password_);
        bool loginUser(const std::string& userId_, const std::string& password_);
        bool verifyUserForCookie(const std::string& cookie_);
        bool logoutUserForCookie(const std::string& cookie_);
        bool logoutUserForAllCookies(const std::string& userId_);
        bool changeUserPassword(const std::string& userId_,const std::string& oldPassword_,
                                const std::string& newPassword_);
        bool addResource(std::string resId_,std::string userName_,
                         Database::UserPermission permission_);

    protected:
        
        Database::MariaDBConnections            _databaseConnector;
        ServerData                              _serverData;
        

    };


}} //closing namespace Santiago::Authentication
#endif
