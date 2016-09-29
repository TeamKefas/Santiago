#include "MariaDBConnection.h"

namespace Santiago{ namespace SantiagoDBTables
{
    bool MariaDBConnection::connect()
    {       
        con = mysql_init(NULL);

        if(con == NULL) 
        {
            return 0;
        }
        
        if(mysql_real_connect(con, "localhost", "root", "kefas123", 
                               "databaseName", 0, NULL, 0) == NULL) //replace kefas123 with MariaDB password
        {
            mysql_close(con);
            return 0;
        }

        conInside = mysql_init(NULL);
               
        if(conInside == NULL) 
        {
            return 0;
        }
        
        if(mysql_real_connect(conInside, "localhost", "root", "kefas123", 
                               "databaseName", 0, NULL, 0) == NULL) //replace kefas123 with MariaDB password
        {
            mysql_close(conInside);
            return 0;
        }
        
        return 1;        
    }

    bool MariaDBConnection::disconnect()
    {
        mysql_close(conInside);
        mysql_close(con);
        return 1;
    }
    
    bool MariaDBConnection::addUserProfileRecord(const std::string userName_, const std::string password_)
    {
        if(connect())
        {
            std::string checkUserId =
                "SELECT COALESCE(MAX(ID), 0) FROM USER_PROFILE WHERE USERNAME = '" + userName_ + "'";
            
            if(mysql_query(con, checkUserId.c_str()))
            {
                disconnect();
                return 0;
            }
            
            MYSQL_RES *result = mysql_store_result(con);
            
            if(result == NULL) 
            {
                disconnect();
                return 0;
            }
            
            MYSQL_ROW row;
            row = mysql_fetch_row(result);
            
            if(atoi(row[0]))
            {
                disconnect();
                return 0;
            }
                                   
            std::string insertQuery = "INSERT INTO USER_PROFILE(USERNAME,PASSWORD) VALUES('" +
                 userName_ + "', '" + password_ + "')";
            
            if(mysql_query(conInside, insertQuery.c_str()))
            {
                disconnect();
                return 0;
            }
            
            disconnect();
            return 1;           
        }
        else
        {
            return 0;
        }
    }
       
    bool MariaDBConnection::updateUserPassword(const std::string userId_,
                                                const std::string oldPassword_, const std::string newPassword_)
    {
        if(connect())
        {
            std::string checkOldPassword = "SELECT PASSWORD FROM USER_PROFILE WHERE USERNAME='" + userId_ + "'";
        
            if(mysql_query(con, checkOldPassword.c_str()))
            {
                disconnect();
                return 0;
            }
            
            MYSQL_RES *result = mysql_store_result(con);
            
            if(result == NULL) 
            {
                disconnect();
                return 0;
            }
            
            MYSQL_ROW row;
            row = mysql_fetch_row(result);
            
            if(row)
            {
                if(oldPassword_ != row[0])
                {
                    disconnect();
                    return 0;
                }
                
                else
                {                   
                    std::string updateQuery = "UPDATE USER_PROFILE SET PASSWORD='" +
                        newPassword_ + "' WHERE USERNAME='" + userId_ +"'";
                    
                    if(mysql_query(conInside, updateQuery.c_str()))
                    {              
                        disconnect();
                        return 0;
                    }
                    
                    disconnect();
                    return 1;   
                }
            }
            else
            {
                disconnect();
                return 0;
            }
        }
        else
        {
            return 0;
        }
    }
        
    bool MariaDBConnection::addSessionRecord(const std::string userName_,
                                             const std::string cookieId_, ptime loginTime_)
    {
        if(connect())
        {
            std::stringstream loginTime;
            loginTime << loginTime_;
            std::string login = loginTime.str().substr(0, 5);
            auto searchLogin = _map.alphabetDigit.find(loginTime.str().substr(5, 3));
            login += searchLogin->second + loginTime.str().substr(8,12);
            
            std::string insertQuery = "INSERT INTO SESSION(USERNAME,COOKIE_ID,LOGIN_TIME) VALUES('" +
                userName_ + "', '" +
                cookieId_ + "', '" + login + "')";
            
            if(mysql_query(con, insertQuery.c_str()))
            {
                disconnect();
                return 0;
            }
            
            disconnect();
            return 1;
        }
        else
        {
            return 0;
        }
    }

    bool MariaDBConnection::updateSessionRecord(const std::string userId_, ptime logoutTime_)
    {
        if(connect())
        {        
            std::stringstream logoutTime;
            logoutTime << logoutTime_;
            std::string logout = logoutTime.str().substr(0, 5);
            auto searchLogout = _map.alphabetDigit.find(logoutTime.str().substr(5, 3));
            logout += searchLogout->second + logoutTime.str().substr(8, 12);
            std::string addLogoutTime = "UPDATE SESSION SET LOGOUT_TIME='" +
                logout + "' WHERE USERNAME='" + userId_ + "'";
            
            if(mysql_query(con, addLogoutTime.c_str()))
            {
                disconnect();
                return 0;
            }
            
            disconnect();
            return 1;
        }
        else
        {
            return 0;
        }
    }
    
    bool MariaDBConnection::addPermissionRecord(int resId_,
                                                const std::string userName_,
                                                UserPermission permission_)
    {
        if(connect())
        {
            std::stringstream resId;
            resId << resId_;
            auto search = _map.userPermissionString.find(permission_);
            
            std::string insertQuery = "INSERT INTO PERMISSION(RES_ID,USERNAME,PERMISSION) VALUES(" +
                resId.str() + ", '" + userName_ + "', '" + search->second + "')";
            
            if(mysql_query(con, insertQuery.c_str()))
            {
                disconnect();
                return 0;
            }
            
            disconnect();
            return 1;
        }
        else
        {
            return 0;
        }
    }
    
    bool MariaDBConnection::getUserProfileRecord(const std::string userName_,
                                                 boost::optional<UserProfile>& userProfileRecord_)
    {
        if(connect())
        {
            std::string getUserProfileRecordQuery = "SELECT * FROM USER_PROFILE WHERE USERNAME = '" + userName_ + "'";
            
            if(mysql_query(con, getUserProfileRecordQuery.c_str()))
            {
                disconnect();
                return 0;
            }
        
            MYSQL_RES *result = mysql_store_result(con);
            
            if(result == NULL) 
            {
                disconnect();
                return 0;
            }
        
            MYSQL_ROW row;
            
            while((row = mysql_fetch_row(result))) 
            { 
                userProfileRecord_->_id = atoi(row[0]);
                userProfileRecord_->_userName = row[1];
                userProfileRecord_->_password = row[2];
            }
        
            mysql_free_result(result);
            disconnect();
            
            if(mysql_num_rows(result))
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

    bool MariaDBConnection::getSessionRecord(const std::string userName_,
                                             boost::optional<Session>& sessionRecord_)
    {
        if(connect())
        {
            std::string getSessionRecordQuery = "SELECT * FROM SESSION WHERE USERNAME = '" + userName_ + "'";
            
            if(mysql_query(con, getSessionRecordQuery.c_str()))
            {
                disconnect();
                return 0;
            }
            
            MYSQL_RES *result = mysql_store_result(con);
            
            if(result == NULL) 
            {
                disconnect();
                return 0;
            }
            
            MYSQL_ROW row;
                       
            while((row = mysql_fetch_row(result))) 
            { 
                sessionRecord_->_id = atoi(row[0]);
                sessionRecord_->_userName = row[1];
                sessionRecord_->_cookieId = row[2];
                
                std::stringstream loginTime;
                loginTime << row[3];
                std::string login = loginTime.str().substr(0, 5);
                auto searchLogin = _map.digitAlphabet.find(loginTime.str().substr(5, 2));
                login += searchLogin->second + loginTime.str().substr(7,12);
                loginTime.str("");
                loginTime << login;
                loginTime >> sessionRecord_->_loginTime;
            
                std::stringstream logoutTime;
                logoutTime << row[4];
                std::string logout = logoutTime.str().substr(0, 5);
                auto searchLogout = _map.digitAlphabet.find(logoutTime.str().substr(5, 2));
                logout += searchLogout->second + logoutTime.str().substr(7,12);
                logoutTime.str("");
                logoutTime << logout;
                logoutTime >> sessionRecord_->_logoutTime;  
            }
            
            mysql_free_result(result);
            disconnect();
            
            if(mysql_num_rows(result))
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

    bool MariaDBConnection::getPermissionRecord(const std::string userName_,
                                                 boost::optional<Permission>& permissionRecord_)
    {
        if(connect())
        {
            std::string getPermissionRecordQuery = "SELECT * FROM PERMISSION WHERE USERNAME = '" + userName_ + "'";
            
            if(mysql_query(con, getPermissionRecordQuery.c_str()))
            {
                disconnect();
                return 0;
            }
            
            MYSQL_RES *result = mysql_store_result(con);
            
            if(result == NULL) 
            {
                disconnect();
                return 0;
            }
            
            MYSQL_ROW row;
            
            while((row = mysql_fetch_row(result))) 
            { 
                permissionRecord_->_id = atoi(row[0]);
                permissionRecord_->_resId = atoi(row[1]);
                permissionRecord_->_userName = row[2];
                std::string permissionString = row[3];
                auto search = _map.stringUserPermission.find(permissionString);
                if(search != _map.stringUserPermission.end())
                {
                    permissionRecord_->_userPermission = search->second;
                }
            }
            
            mysql_free_result(result);
            disconnect();

            if(mysql_num_rows(result))
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
    
}}
