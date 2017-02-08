#ifndef SANTIAGO_USER_SERVER_DATABASEINTERFACE_H
#define SANTIAGO_USER_SERVER_DATABASEINTERFACE_H

/**
 * @file DatabaseInterface.h
 *
 * @section DESCRIPTION
 *
 * Contains the DatabaseInterface class  
 */

#include <vector>
#include <cstdlib>
#include <string>
#include <sstream>

#include "SantiagoDBTables/MariaDBConnection.h"
#include "ServerData.h"

namespace Santiago{ namespace User { namespace Server
{
    using namespace boost::posix_time;
    class DatabaseInterface
    {
    public:
        /**
         * The constructor
         * @param config_- 
         */
        DatabaseInterface(const boost::property_tree::ptree& config_);
       /**
        * ///Message\\
        * @param userId_ - 
        * @param password_ -
        */
        bool createUser(const std::string& userId_, const std::string& password_);
       /**
        * ///Message\\
        * @param userId_ - 
        * @param password_ -
        */
        bool loginUser(const std::string& userId_, const std::string& password_);
       /**
        * ///Message\\
        * @param cookie_ - 
        */
        bool verifyUserForCookie(const std::string& cookie_);
       /**
        * ///Message\\
        * @param cookie_ - 
        */
        bool logoutUserForCookie(const std::string& cookie_);
       /**
        * ///Message\\
        * @param userId_ - 
        */
        bool logoutUserForAllCookies(const std::string& userId_);
       /**
        * ///Message\\
        * @param userId_ - 
        * @param oldPassword_ -
        * @param newPassword_ -
        */
        bool changeUserPassword(const std::string& userId_, const std::string& oldPassword_,
                                const std::string& newPassword_);
       /**
        * ///Message\\
        * @param resId_ - 
        * @param userName_ -
        * @param permission_ -
        */
        bool addResource(const std::string resId_, const std::string userName_,
                         SantiagoDBTables::UserPermission permission_);

    protected:

        boost::property_tree::ptree&                    _config;
        SantiagoDBTables::MariaDBConnection             _databaseConnector;
        ServerData                              	_serverData;

    };


}}} //closing namespace Santiago::User::Server
#endif
