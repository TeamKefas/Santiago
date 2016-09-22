#ifndef SANTIAGO_AUTHENTICATION_USERCONTROLLER_H
#define SANTIAGO_AUTHENTICATION_USERCONTROLLER_H

#include <ctime>
#include <functional>
#include <mutex>
#include <vector>
#include <cstdlib>

#include "Database/MariaDBConnections.h"
#include "ServerMessage.h"
#include "ConnectionMessageSocket.h"


namespace Santiago{ namespace Authentication
{
    class UserController
    {
        struct CookieData
        {
            std::string            _userId;
            std::vector<unsigned>  _connectionIds;
        };

        struct UserIdData
        {
            std::vector<std::string>  _cookieList;
        };

    public:

        typedef std::function<void(ServerMessage)> SendMessageCallbackFn;
       
        UserController(SendMessageCallbackFn sendMessageCallbackFn_)
            :_sendMessageCallbackFn(sendMessageCallbackFn_)
        {}

        void handleClientMessage(const ServerMessage& serverMessage_);
           
    protected:

        void createUser(const std::string& userId_, const std::string& password_);
        void loginUser(unsigned connectionId_, const std::string& userId_, const std::string& password_);
        void verifyUserForCookie(const std::string& cookie_);
        void logoutUserForCookie(const std::string& cookie_);
        void logoutUserForAllCookies(const std::string& userId_);
        void changeUserPassword(const std::string& userId_,
                                const std::string& oldPassword_,
                                const std::string& newPassword_);

        SendMessageCallbackFn                   _sendMessageCallbackFn;
        std::map<std::string,CookieData>        _cookieCookieDataMap;
        std::map<std::string,UserIdData>        _userIdUserIdDataMap;
        
        Database::MariaDBConnections            _databaseConnector;
        

    };


}} //closing namespace Santiago::Authentication
#endif
