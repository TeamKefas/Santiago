#include "UserController.h"

namespace Santiago{ namespace Authentication
{
    void UserController::handleClientMessage(const ServerMessage& serverMessage_)
    {
        switch(serverMessage_._connectionMessage._type)
        {
            
        case MessageType::CREATE_USER:
            createUser(serverMessage_._connectionMessage._parameters[0],
                       serverMessage_._connectionMessage._parameters[1]);
            break;

        case MessageType::LOGIN_USER:
            loginUser(serverMessage_._connectionId,
                      serverMessage_._connectionMessage._parameters[0],
                      serverMessage_._connectionMessage._parameters[1]);
            break;
            
        case MessageType::VERIFY_USER_FOR_COOKIE:
            verifyUserForCookie(serverMessage_._connectionMessage._parameters[0]);
            break;
            
        case MessageType::LOGOUT_USER_FOR_COOKIE:
            logoutUserForCookie(serverMessage_._connectionMessage._parameters[0]);
            break;
            
        case MessageType::LOGOUT_USER_FOR_ALL_COOKIES:
            logoutUserForAllCookies(serverMessage_._connectionMessage._parameters[0]);
                break;
                
        case MessageType::CHANGE_USER_PASSWORD:
            changeUserPassword(serverMessage_._connectionMessage._parameters[0],
                               serverMessage_._connectionMessage._parameters[1],
                               serverMessage_._connectionMessage._parameters[2]);
            break;
            
        default:
            BOOST_ASSERT(false);
            break;
        }            
    }
    
    void UserController:: createUser(const std::string& userId_, const std::string& password_)
    {
        bool available = _databaseConnector.addUserProfileRecord(userId_, password_);
        
        if(available)
        {
            _sendMessageCallbackFn(usedId_); 
        }

        else
        {
            _sendMessageCallbackFn(); 
        }
    }
    
    void UserController::loginUser(unsigned connectionId_,
                                   const std::string& userId_,
                                   const std::string& password_)
    {
        //TODO: find a way to generate unique cookie. Use connectionId_ to differentiate simultaneous
        //calls if we are using time based generation
        
        bool match = _databaseConnector.checkUserProfileRecord(userId_, password_);
        
        if(match)
        {
            std::string cookie;
            long cookieNum = rand() % 1000000000000000;
            std::stringstream temp;
            temp << cookieNum;
            cookie = temp.str();          // Random generated cookie. May not be unique.
            time_t rawTime;
            struct tm * timeInfo;
            time (&rawTime);
            timeInfo = localtime (&rawTime);
            time_t newTime = timegm(timeInfo);
            ptime loginTime = from_time_t(newTime);
            _databaseConnector.addSessionRecord(userId_, cookie, loginTime);
            std::lock_guard<std::mutex>  lock(mtx);
            _cookieCookieDataMap[cookie] = CookieData{userId_,connectionId_};
            _userIdUserDataMap[userId_]._cookieList.push_back(cookie);
            _sendMessageCallbackFn(cookie);
        }

        else
        {
            _sendMessageCallbackFn(); 
        } 
    }

    void UserController::verifyUserForCookie(const std::string& cookie_)
    {    
        //TODO: if the verify call from another connection add that connection to the _userIdUserIdDataMap

        bool match = (_cookieCookieDataMap.find(cookie_) != _cookieCookieDataMap.end());
        
        if(match)
        {
            time_t rawTime;
            struct tm * timeInfo;
            time (&rawTime);
            timeInfo = localtime (&rawTime);
            time_t newTime = timegm(timeInfo);
            ptime loginTime = from_time_t(newTime);
            _databaseConnector.addSessionRecord(_cookieCookieDataMap[cookie_]._userId, cookie_, loginTime);
            _sendMessageCallbackFn(_cookieCookieDataMap[cookie_]._userId);
        }
        
        else
        {
            _sendMessageCallbackFn();  
        }
    }

    void UserController::logoutUserForCookie(const std::string& userId_)
    {
        //TODO: has some work..skip this for now
        time_t rawTime;
        struct tm * timeInfo;
        time (&rawTime);
        timeInfo = localtime (&rawTime);
        time_t newTime = timegm(timeInfo);
        ptime logoutTime = from_time_t(newTime);
        _databaseConnector.updateSessionRecord(userId_, logoutTime);
    }

    void logoutUserForAllCookies(const std::string& userId_)
    {
        //TODO: has some work..skip for now
    }

    void UserController::changeUserPassword(const std::string& userId_,
                                            const std::string& oldPassword_,
                                            const std::string& newPassword_)
    {
        bool update = _databaseConnector.updateUserPassword(userId_, oldPassword_, newPassword_);
        if(update)
        {
            _sendMessageCallbackFn(usedId_);
        }

        else
        {
            _sendMessageCallbackFn();
        }
    }


}}//closing Santiago::Authentication
