#include "MariaDBConnection.h"

namespace Santiago{ namespace SantiagoDBTables
{
    std::error_code MariaDBConnection::connect()
    {       
        con = mysql_init(NULL);

        if(con == NULL) 
        {
            return 0;
        }
        
        if(mysql_real_connect(con, "localhost", "root", "kefas123", 
                               "databaseName", 0, NULL, 0) == NULL) //replace kefas123 with MariaDB password
        {
            disconnect();
            return 0;
        }
        
        return 1;
    }

    bool MariaDBConnection::isConnected()
    {
        if(connect)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }

    std::error_code MariaDBConnection::disconnect()
    {
        mysql_close(con);
    }
    
    std::error_code MariaDBConnection::addUserProfilesRec(UserProfilesRec& userProfilesRec_)
    {
        if(connect())
        {
            std::string addUserProfilesRecQuery = "INSERT INTO USER_PROFILE(USERNAME,PASSWORD) VALUES('" +
                userProfilesRec_.userName_ + "', '" + userProfilesRec_.password_ + "')";
                    
            if(mysql_query(con, addUserProfilesRecQuery.c_str()))
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

    std::error_code MariaDBConnection::getUserProfilesRec(const std::string userName_,
                                                          boost::optional<UserProfile>& userProfilesRec_)
    {
        if(connect())
        {
            std::string getUserProfilesRecQuery = "SELECT * FROM USER_PROFILE WHERE USERNAME = '" + userName_ + "'";
            
            if(mysql_query(con, getUserProfilesRecQuery.c_str()))
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
                userProfilesRec_->_id = atoi(row[0]);
                userProfilesRec_->_userName = row[1];
                userProfilesRec_->_password = row[2];
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
       
    std::error_code MariaDBConnection::updateUserProfilesRec(UserProfilesRec& userProfilesRec_,
                                                             const std::string newPassword_)
    {
        if(connect())
        {
            std::string retrieveOldPassword = "SELECT PASSWORD FROM USER_PROFILE WHERE USERNAME = '"
                + userProfilesRec_.userId_ + "'";
        
            if(mysql_query(con, retrieveOldPassword.c_str()))
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
            
            if(mysql_num_rows(result))
            {
                if(oldPassword_ != row[0])
                {
                    disconnect();
                    return 0;
                }
                
                else
                {                   
                    disconnect();
                    
                    if(connect())
                    {
                        std::string updateUserProfilesRecQuery = "UPDATE USER_PROFILE SET PASSWORD='" +
                            newPassword_ + "' WHERE USERNAME = '" + userProfilesRec_.userId_ +"'";
                        
                        if(mysql_query(con, updateUserProfilesRecQuery.c_str()))
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

    std::error_code MariaDBConnection::deleteUserProfilesRec(const std::string& userName_)
    {
        if(connect())
        {
            std::string deleteUserProfilesRecQuery = "DELETE FROM USER_PROFILE WHERE USERNAME = '" +
                userName_ + ")";
                    
            if(mysql_query(con, deleteUserProfilesRecQuery.c_str()))
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
        
    std::error_code MariaDBConnection::addSessionsRec(SessionsRec& sessionsRec_)
    {
        if(connect())
        {
            std::stringstream loginTime;
            loginTime << sessionsRec_.loginTime_;
            std::string login = loginTime.str().substr(0, 5);
            auto searchLogin = alphabetDigit.find(loginTime.str().substr(5, 3));
            login += searchLogin->second + loginTime.str().substr(8,12);
            
            std::string addSessionsRecQuery = "INSERT INTO SESSION(USERNAME,COOKIE_ID,LOGIN_TIME) VALUES('" +
                sessionsRec_.userName_ + "', '" +
                sessionsRec_.cookieId_ + "', '" + login + "')";
            
            if(mysql_query(con, addSessionsRecQuery.c_str()))
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
      
    std::error_code MariaDBConnection::getSessionsRec(const std::string userName_,
                                                      boost::optional<Session>& sessionsRec_)
    {
        if(connect())
        {
            std::string getSessionsRecQuery = "SELECT * FROM SESSION WHERE USERNAME = '" + userName_ + "'";
            
            if(mysql_query(con, getSessionsRecQuery.c_str()))
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
                sessionsRec_->_id = atoi(row[0]);
                sessionsRec_->_userName = row[1];
                sessionsRec_->_cookieId = row[2];
                
                std::stringstream loginTime;
                loginTime << row[3];
                std::string login = loginTime.str().substr(0, 5);
                auto searchLogin = digitAlphabet.find(loginTime.str().substr(5, 2));
                login += searchLogin->second + loginTime.str().substr(7,12);
                loginTime.str("");
                loginTime << login;
                loginTime >> sessionsRec_->_loginTime;
            
                std::stringstream logoutTime;
                logoutTime << row[4];
                std::string logout = logoutTime.str().substr(0, 5);
                auto searchLogout = digitAlphabet.find(logoutTime.str().substr(5, 2));
                logout += searchLogout->second + logoutTime.str().substr(7,12);
                logoutTime.str("");
                logoutTime << logout;
                logoutTime >> sessionsRec_->_logoutTime;  
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

    std::error_code MariaDBConnection::updateSessionsRec(SessionsRec& sessionsRec_)
    {
        if(connect())
        {        
            std::stringstream logoutTime;
            logoutTime << sessionsRec_.logoutTime_;
            std::string logout = logoutTime.str().substr(0, 5);
            auto searchLogout = alphabetDigit.find(logoutTime.str().substr(5, 3));
            logout += searchLogout->second + logoutTime.str().substr(8, 12);
            std::string updateSessionsRecQuery = "UPDATE SESSION SET LOGOUT_TIME='" +
                logout + "' WHERE USERNAME='" + sessionsRec_.userId_ + "'";
            
            if(mysql_query(con, updateSessionsRecQuery.c_str()))
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
    
    std::error_code MariaDBConnection::addPermissionsRec(PermissionsRec& permissionsRec_)
    {
        if(connect())
        {
            std::stringstream resId;
            resId << permissionsRec_.resId_;
            auto search = userPermissionString.find(permission_);
            
            std::string addPermissionsRecQuery = "INSERT INTO PERMISSION(RES_ID,USERNAME,PERMISSION) VALUES(" +
                resId.str() + ", '" + permissionsRec_.userName_ + "', '" + search->second + "')";
            
            if(mysql_query(con, addPermissionsRecQuery.c_str()))
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
  
    std::error_code MariaDBConnection::getPermissionsRec(const std::string userName_,
                                                         boost::optional<Permission>& permissionsRec_)
    {
        if(connect())
        {
            std::string getPermissionsRecQuery = "SELECT * FROM PERMISSION WHERE USERNAME = '" + userName_ + "'";
            
            if(mysql_query(con, getPermissionsRecQuery.c_str()))
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
                permissionsRec_->_id = atoi(row[0]);
                permissionsRec_->_resId = atoi(row[1]);
                permissionsRec_->_userName = row[2];
                std::string permissionString = row[3];
                auto search = stringUserPermission.find(permissionString);
                if(search != stringUserPermission.end())
                {
                    permissionsRec_->_userPermission = search->second;
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
