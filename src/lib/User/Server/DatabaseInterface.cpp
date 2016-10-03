#include "DatabaseInterface.h"

namespace Santiago{ namespace User { namespace Server
{   
    bool DatabaseInterface::createUser(const std::string& userId_, const std::string& password_)
    {
        bool available = _databaseConnector.addUserProfilesRec(userId_, password_);
        
        if(available)
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
        boost::optional<UserProfile> userProfileRecord = UserProfile();
        
        bool match = _databaseConnector.getUserProfilesRec(userId_, userProfileRecord);
        
        if(match)
        {
            std::string cookie;
            long cookieNum = rand() % 1000000000000000;
            std::stringstream temp;
            temp << cookieNum;
            cookie = temp.str();          // Random generated cookie. May not be unique.
            ptime loginTime = second_clock::local_time();
            _databaseConnector.addSessionsRec(userId_, cookie, loginTime);
            return 1;
        }

        else
        {
            return 0;
        } 
    }

    bool DatabaseInterface::verifyUserForCookie(const std::string& cookie_)
    {    
        //TODO: if the verify call from another connection add that connection to the _userIdUserIdDataMap   
        ptime loginTime = second_clock::local_time();
        
        CookieData cookieData = _serverData._cookieCookieDataMap.find(cookie_)->second;     
        _databaseConnector.addSessionsRec(cookieData._userId, cookie_, loginTime);
        return 1;
    }
    // TODO: 
    bool DatabaseInterface::logoutUserForCookie(const std::string& userId_)
    {
        //TODO: has some work..skip this for now  
        ptime logoutTime = second_clock::local_time();
        _databaseConnector.updateSessionsRec(userId_, logoutTime);
        return 1;
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
        bool update = _databaseConnector.updateUserPassword(userId_, oldPassword_, newPassword_);
        if(update)
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
        std::stringstream resId;
        resId << resId_;
        int newResId;
        resId >> newResId;
        if(_databaseConnector.addPermissionsRec(newResId,userName_,permission_))
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }


}}}//closing Santiago::User::Server
