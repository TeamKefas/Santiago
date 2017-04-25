#ifndef SANTIAGO_AUTHENTICATION_SERVER_USERCONTROLLER_H
#define SANTIAGO_AUTHENTICATION_SERVER_USERCONTROLLER_H

/**
 * @file UserController.h
 *
 * @section DESCRIPTION
 *
 * Contains the UserController class  
 */

#include <ctime>
#include <functional>
#include <mutex>
#include <vector>
#include <cstdlib>

#include "SantiagoDBTables/MariaDBConnections.h"
#include "ServerMessage.h"
#include "ConnectionMessageSocket.h"


namespace Santiago{ namespace Authentication { namespace Server
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

        /**
         * The constructor
         * @param sendMessageCallbackFn_- 
         */
        UserController(SendMessageCallbackFn sendMessageCallbackFn_)
            :_sendMessageCallbackFn(sendMessageCallbackFn_)
        {}

       /**
        * ///Message\\
        * @param serverMessage_ - 
        */
        void handleClientMessage(const ServerMessage& serverMessage_);
           
    protected:
       /**
        * ///Message\\
        * @param userId_ - 
        * @param password_ -
        */
        void createUser(const std::string& userId_, const std::string& password_);
       /**
        * ///Message\\
        * @param connectionId_ - 
        * @param userId_ -
        * @param password_ -
        */
        void loginUser(unsigned connectionId_, const std::string& userId_, const std::string& password_);
       /**
        * ///Message\\
        * @param cookie_ - 
        */
        void verifyUserForCookie(const std::string& cookie_);
       /**
        * ///Message\\
        * @param cookie_ - 
        */
        void logoutUserForCookie(const std::string& cookie_);
       /**
        * ///Message\\
        * @param userId_ - 
        */
        void logoutUserForAllCookies(const std::string& userId_);
       /**
        * ///Message\\
        * @param userId_ - 
        * @param oldPassword_ -
        * @param newPassword_ -
        */
        void changeUserPassword(const std::string& userId_,
                                const std::string& oldPassword_,
                                const std::string& newPassword_);

        SendMessageCallbackFn                   _sendMessageCallbackFn;
        std::map<std::string,CookieData>        _cookieCookieDataMap;
        std::map<std::string,UserIdData>        _userIdUserIdDataMap;
        
        Database::MariaDBConnections            _databaseConnector;
        

    };


}}} //closing namespace Santiago::Authentication::Server
#endif
