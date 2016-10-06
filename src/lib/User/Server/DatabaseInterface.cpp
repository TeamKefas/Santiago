#include "DatabaseInterface.h"

namespace Santiago{ namespace User { namespace Server
{
    DatabaseInterface::DatabaseInterface(const boost::property_tree::ptree& config_)
        :_config(config_),
         _databaseConnector(_config)
    {}
    
    
    bool DatabaseInterface::createUser(const std::string& userId_, const std::string& password_)
    {
        SantiagoDBTables::UserProfilesRec userProfilesRec;
        userProfilesRec._userName = userId_;
        userProfilesRec._password = password_;
        
        //int available = _databaseConnector.addUserProfilesRec(userProfilesRec);
        
        if(!(_databaseConnector.addUserProfilesRec(userProfilesRec)))
        {
            return 1;
        }

        else
        {
            return 0;
        }
    }
    
    bool DatabaseInterface::loginUser(const std::string& userId_,
                                      const std::string& password_)
    {
        boost::optional<SantiagoDBTables::UserProfilesRec> userProfilesRec =SantiagoDBTables::UserProfilesRec();
        
        //  int match = _databaseConnector.getUserProfilesRec(userId_, userProfilesRec);
        
        if(!( _databaseConnector.getUserProfilesRec(userId_, userProfilesRec)))
        {
            SantiagoDBTables::SessionsRec sessionsRec;
            sessionsRec._userName = userId_;

            long cookieNum = rand() % 1000000000000000;
            std::stringstream cookie;
            cookie << cookieNum;
            sessionsRec._cookieId = cookie.str();          // Random generated cookie. May not be unique.

            sessionsRec._loginTime = second_clock::local_time();
           
            if(!_databaseConnector.addSessionsRec(sessionsRec))
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }

        else
        {
            return 0;
        } 
    }

    bool DatabaseInterface::verifyUserForCookie(const std::string& cookie_)
    {    
        //TODO: if the verify call from another connection add that connection to the _userIdUserIdDataMap   
        SantiagoDBTables::SessionsRec sessionsRec;
        sessionsRec._cookieId = cookie_;
        sessionsRec._loginTime = second_clock::local_time();
        
        CookieData cookieData = _serverData._cookieCookieDataMap.find(cookie_)->second;
        //TODO: sessionsRec._userId = ;

        if(!_databaseConnector.addSessionsRec(sessionsRec))
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    // TODO: 
    bool DatabaseInterface::logoutUserForCookie(const std::string& userId_)
    {
        //TODO: has some work..skip this for now
        SantiagoDBTables::SessionsRec sessionsRec;
        sessionsRec._userName = userId_;
        sessionsRec._logoutTime = second_clock::local_time();

        if(!_databaseConnector.updateSessionsRec(sessionsRec))
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }

    bool DatabaseInterface::logoutUserForAllCookies(const std::string& userId_)
    {
        //TODO: has some work..skip for now
        return 1;
    }

    bool DatabaseInterface::changeUserPassword(const std::string& userId_,
                                               const std::string& oldPassword_,
                                               const std::string& newPassword_)
    {
        SantiagoDBTables::UserProfilesRec userProfilesRec;
        userProfilesRec._userName = userId_;
        userProfilesRec._password = oldPassword_;
        
        //  int update = _databaseConnector.updateUserProfilesRec(userProfilesRec, newPassword_);

        if(!(_databaseConnector.updateUserProfilesRec(userProfilesRec, newPassword_)))
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }

    bool DatabaseInterface::addResource(const std::string resId_, const std::string userName_,
                                        SantiagoDBTables::UserPermission permission_)
    {
        SantiagoDBTables::PermissionsRec permissionsRec;

        std::stringstream resId;
        resId << resId_;
        resId >> permissionsRec._resId;

        permissionsRec._userName = userName_;
        permissionsRec._userPermission = permission_;
        
        if(!_databaseConnector.addPermissionsRec(permissionsRec))
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }


}}}//closing Santiago::User::Server
