#include "DatabaseInterface.h"

namespace Santiago{ namespace User { namespace Server
{   
    bool DatabaseInterface::createUser(const std::string& userId_, const std::string& password_)
    {
        UserProfilesRec userProfilesRec;
        userProfilesRec._userName = userId_;
        userProfilesRec._password = password_;

        int available = _databaseConnector.addUserProfilesRec(userProfilesRec);
        
        if(!available)
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
        boost::optional<UserProfilesRec> userProfilesRec = UserProfilesRec();
        
        int match = _databaseConnector.getUserProfilesRec(userId_, userProfilesRec);
        
        if(!match)
        {
            SessionsRec sessionsRec;
            sessionsRec._userName = userId_;

            long cookieNum = rand() % 1000000000000000;
            std::stringstream cookie;
            cookie << cookieNum;
            sessionsRec._cookieId = temp.str();          // Random generated cookie. May not be unique.

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
        SessionsRec sessionsRec;
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
        SessionsRec sessionsRec;
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
        UserProfilesRec userProfilesRec;
        userProfilesRec._userName = userId_;
        userProfilesRec._password = oldPassword_;
        
        int update = _databaseConnector.updateUserProfilesRec(userProfilesRec, newPassword_);

        if(!update)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }

    bool DatabaseInterface::addResource(const std::string resId_, const std::string userName_,
                                        SantiagoDBTables::permission_)
    {
        PermissionsRec permissionsRec;

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
