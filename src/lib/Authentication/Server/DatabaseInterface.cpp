#include "DatabaseInterface.h"

namespace Santiago{ namespace Authentication
{   
    bool DatabaseInterface::createUser(const std::string& userId_, const std::string& password_)
    {
        bool available = _databaseConnector.addUserProfileRecord(userId_, password_);
        
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
               
        bool match = _databaseConnector.checkUserProfileRecord(userId_, password_);
        
        if(match)
        {
            std::string cookie;
            long cookieNum = rand() % 1000000000000000;
            std::stringstream temp;
            temp << cookieNum;
            cookie = temp.str();          // Random generated cookie. May not be unique.
            ptime loginTime = second_clock::local_time();
            _databaseConnector.addSessionRecord(userId_, cookie, loginTime);
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
        _databaseConnector.addSessionRecord(cookieData._userId, cookie_, loginTime);
        return 1;
    }
    // TODO: 
    bool DatabaseInterface::logoutUserForCookie(const std::string& userId_)
    {
        //TODO: has some work..skip this for now  
        ptime logoutTime = second_clock::local_time();
        _databaseConnector.updateSessionRecord(userId_, logoutTime);
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

    bool DatabaseInterface::addResource(std::string resId_,std::string userName_,
                                        Database::UserPermission permission_)
    {
        std::stringstream resId;
        resId<<resId_;
        int newResId;
        resId>>newResId;
        if(_databaseConnector.addPermissionRecord(newResId,userName_,permission_))
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }


}}//closing Santiago::Authentication
