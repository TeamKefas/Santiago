#include "MariaDBConnection.h"

namespace Santiago{ namespace SantiagoDBTables
{
    std::error_code MariaDBConnection::connect()
    {       
        _mysql = mysql_init(NULL);

        if(_mysql == NULL) 
        {
            return std::error_code(ERR_DATABASE_EXCEPTION, ErrorCategory::GetInstance());
        }
        
        if(mysql_real_connect(_mysql,_host,_user,_passwd,
           _db,_port,_unixSocket,_flags) == NULL)
        {
            return std::error_code(ERR_DATABASE_EXCEPTION, ErrorCategory::GetInstance());
        }
       
        return std::error_code(ERR_SUCCESS, ErrorCategory::GetInstance());
    }

    bool MariaDBConnection::isConnected()
    {
        if(_mysql == NULL)
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }

    std::error_code MariaDBConnection::disconnect()
    {
        mysql_close(_mysql);
        return std::error_code(ERR_SUCCESS, ErrorCategory::GetInstance());
    }
    
    std::error_code MariaDBConnection::addUserProfilesRec(UserProfilesRec& userProfilesRec_)
    {
        if(isConnected())
        {
             boost::optional<UserProfilesRec> userProfilesRecord = UserProfilesRec();

             if(getUserProfilesRec(userProfilesRec_._userName, userProfilesRecord))
             {
                std::string addUserProfilesRecQuery = "INSERT INTO USER_PROFILE(USERNAME,PASSWORD) VALUES('" +
                    userProfilesRec_._userName + "', '" + userProfilesRec_._password + "')";
                
                if(mysql_query(_mysql, addUserProfilesRecQuery.c_str()))
                {
                    return std::error_code(ERR_DATABASE_EXCEPTION, ErrorCategory::GetInstance());
                }
             }
             else
             {
                return std::error_code(ERR_USERNAME_ALREADY_EXISTS, ErrorCategory::GetInstance());
             }
            
             return std::error_code(ERR_SUCCESS, ErrorCategory::GetInstance());
        }
        else
        {
            return std::error_code(ERR_DATABASE_EXCEPTION, ErrorCategory::GetInstance());
        }
    }

    std::error_code MariaDBConnection::getUserProfilesRec(const std::string& userName_,
                                                          boost::optional<UserProfilesRec>& userProfilesRec_)
    {
        if(isConnected())
        {
            std::string getUserProfilesRecQuery = "SELECT * FROM USER_PROFILE WHERE USERNAME = '" + userName_ + "'";
            
            if(mysql_query(_mysql, getUserProfilesRecQuery.c_str()))
            {
                return std::error_code(ERR_DATABASE_EXCEPTION, ErrorCategory::GetInstance());
            }
        
            MYSQL_RES *result = mysql_store_result(_mysql);
            
            if(result == NULL) 
            {
                return  std::error_code(ERR_DATABASE_EXCEPTION, ErrorCategory::GetInstance());
            }
        
           if(mysql_num_rows(result))
            {
                MYSQL_ROW row;
            
                while((row = mysql_fetch_row(result))) 
                { 
                    userProfilesRec_->_id = atoi(row[0]);
                    userProfilesRec_->_userName = row[1];
                    userProfilesRec_->_password = row[2];
                }
                
                mysql_free_result(result);
                                
                return std::error_code(ERR_SUCCESS, ErrorCategory::GetInstance());
            }
            else
            {
                return std::error_code(ERR_DATABASE_QUERY_FAILED, ErrorCategory::GetInstance());
            }
        }
        else
        {
            return std::error_code(ERR_DATABASE_EXCEPTION, ErrorCategory::GetInstance());
        }
    }
       
    std::error_code MariaDBConnection::updateUserProfilesRec(UserProfilesRec& userProfilesRec_,
                                                             const std::string& newPassword_)
    {
        if(isConnected())
        {
            std::string retrieveOldPassword = "SELECT PASSWORD FROM USER_PROFILE WHERE USERNAME = '"
                + userProfilesRec_._userName + "'";
        
            if(mysql_query(_mysql, retrieveOldPassword.c_str()))
            {
                return std::error_code(ERR_DATABASE_EXCEPTION, ErrorCategory::GetInstance());
            }
            
            MYSQL_RES *result = mysql_store_result(_mysql);
            
            if(result == NULL) 
            {
                return std::error_code(ERR_DATABASE_EXCEPTION, ErrorCategory::GetInstance());
            }
                    
            if(mysql_num_rows(result))
            {
                MYSQL_ROW row;
                row = mysql_fetch_row(result);
                        
                if(userProfilesRec_._password != row[0])
                {
                    return std::error_code(ERR_INVALID_USERNAME_PASSWORD, ErrorCategory::GetInstance());
                }
                
                else
                {                   
                    if(isConnected())
                    {
                        std::string updateUserProfilesRecQuery = "UPDATE USER_PROFILE SET PASSWORD='" +
                            newPassword_ + "' WHERE USERNAME = '" + userProfilesRec_._userName +"'";
                        
                        if(mysql_query(_mysql, updateUserProfilesRecQuery.c_str()))
                        {              
                            return  std::error_code(ERR_DATABASE_EXCEPTION, ErrorCategory::GetInstance());
                        }
                        
                        return  std::error_code(ERR_SUCCESS, ErrorCategory::GetInstance());
                    }
                    else
                    {
                        return  std::error_code(ERR_DATABASE_EXCEPTION, ErrorCategory::GetInstance());
                    }
                }
            }
            else
            {
                return std::error_code(ERR_DATABASE_QUERY_FAILED, ErrorCategory::GetInstance());
            }
        }
        else
        {
            return std::error_code(ERR_DATABASE_EXCEPTION, ErrorCategory::GetInstance());
        }  
    }

    std::error_code MariaDBConnection::deleteUserProfilesRec(const std::string& userName_)
    {
        if(isConnected())
        {
            boost::optional<UserProfilesRec> userProfilesRecord = UserProfilesRec();
            if(!getUserProfilesRec(userName_, userProfilesRecord))
            {
                std::string deleteUserProfilesRecQuery = "DELETE FROM USER_PROFILE WHERE USERNAME = '" +
                    userName_ + "'";
                    
                if(mysql_query(_mysql, deleteUserProfilesRecQuery.c_str()))
                {
                    return std::error_code(ERR_DATABASE_EXCEPTION, ErrorCategory::GetInstance());
                }
            
                return std::error_code(ERR_SUCCESS, ErrorCategory::GetInstance());
            }
            else
            {
                return std::error_code(ERR_INVALID_USERNAME_PASSWORD, ErrorCategory::GetInstance());
            }
        }
        else
        {
            return std::error_code(ERR_DATABASE_EXCEPTION, ErrorCategory::GetInstance());
        }
    }
        
    std::error_code MariaDBConnection::addSessionsRec(SessionsRec& sessionsRec_)
    {
        if(isConnected())
        {
            std::stringstream loginTime;
            loginTime << sessionsRec_._loginTime;
            std::string login = loginTime.str().substr(0, 5);
            auto searchLogin = alphabetDigit.find(loginTime.str().substr(5, 3));
            login += searchLogin->second + loginTime.str().substr(8,12);
            
            std::string addSessionsRecQuery = "INSERT INTO SESSION(USERNAME,COOKIE_ID,LOGIN_TIME) VALUES('" +
                sessionsRec_._userName + "', '" +
                sessionsRec_._cookieId + "', '" + login + "')";
            
            if(mysql_query(_mysql, addSessionsRecQuery.c_str()))
            {
                return std::error_code(ERR_DATABASE_EXCEPTION, ErrorCategory::GetInstance());
            }
            
            return std::error_code(ERR_SUCCESS, ErrorCategory::GetInstance());
        }
        else
        {
            return std::error_code(ERR_DATABASE_EXCEPTION, ErrorCategory::GetInstance());
        }
    }
      
    std::error_code MariaDBConnection::getSessionsRec(const std::string& userName_,
                                                      boost::optional<SessionsRec>& sessionsRec_)
    {
        if(isConnected())
        {
            std::string getSessionsRecQuery = "SELECT * FROM SESSION WHERE USERNAME = '" + userName_ + "'";
            
            if(mysql_query(_mysql, getSessionsRecQuery.c_str()))
            {
                return std::error_code(ERR_DATABASE_EXCEPTION, ErrorCategory::GetInstance());
            }
            
            MYSQL_RES *result = mysql_store_result(_mysql);
            
            if(result == NULL) 
            {
                return std::error_code(ERR_DATABASE_EXCEPTION, ErrorCategory::GetInstance());
            }

            if(mysql_num_rows(result))
            {            
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
                
                return std::error_code(ERR_SUCCESS, ErrorCategory::GetInstance());
            }
            else
            {
                return std::error_code(ERR_DATABASE_EXCEPTION, ErrorCategory::GetInstance());
            }
        }
        else
        {
            return std::error_code(ERR_DATABASE_EXCEPTION, ErrorCategory::GetInstance());
        }
    }

    std::error_code MariaDBConnection::updateSessionsRec(SessionsRec& sessionsRec_)
    {
        if(isConnected())
        {        
            std::stringstream logoutTime;
            logoutTime << sessionsRec_._logoutTime;
            std::string logout = logoutTime.str().substr(0, 5);
            auto searchLogout = alphabetDigit.find(logoutTime.str().substr(5, 3));
            logout += searchLogout->second + logoutTime.str().substr(8, 12);
            std::string updateSessionsRecQuery = "UPDATE SESSION SET LOGOUT_TIME='" +
                logout + "' WHERE USERNAME='" + sessionsRec_._userName + "'";
            
            if(mysql_query(_mysql, updateSessionsRecQuery.c_str()))
            {
                return std::error_code(ERR_DATABASE_EXCEPTION, ErrorCategory::GetInstance());
            }
            
            return std::error_code(ERR_SUCCESS, ErrorCategory::GetInstance());
        }
        else
        {
            return std::error_code(ERR_DATABASE_EXCEPTION, ErrorCategory::GetInstance());
        }
    }
    
    std::error_code MariaDBConnection::addPermissionsRec(PermissionsRec& permissionsRec_)
    {
        if(isConnected())
        {
            std::stringstream resId;
            resId << permissionsRec_._resId;
            auto search = userPermissionString.find(permissionsRec_._userPermission);
            
            std::string addPermissionsRecQuery = "INSERT INTO PERMISSION(RES_ID,USERNAME,PERMISSION) VALUES(" +
                resId.str() + ", '" + permissionsRec_._userName + "', '" + search->second + "')";
            
            if(mysql_query(_mysql, addPermissionsRecQuery.c_str()))
            {
                return std::error_code(ERR_DATABASE_EXCEPTION, ErrorCategory::GetInstance());
            }
            
            return std::error_code(ERR_SUCCESS, ErrorCategory::GetInstance());
        }
        else
        {
            return std::error_code(ERR_DATABASE_EXCEPTION, ErrorCategory::GetInstance());
        }
    }
  
    std::error_code MariaDBConnection::getPermissionsRec(const std::string& userName_,
                                                         boost::optional<PermissionsRec>& permissionsRec_)
    {
        if(isConnected())
        {
            std::string getPermissionsRecQuery = "SELECT * FROM PERMISSION WHERE USERNAME = '" + userName_ + "'";
            
            if(mysql_query(_mysql, getPermissionsRecQuery.c_str()))
            {
                return std::error_code(ERR_DATABASE_EXCEPTION, ErrorCategory::GetInstance());
            }
            
            MYSQL_RES *result = mysql_store_result(_mysql);
            
            if(result == NULL) 
            {
                return std::error_code(ERR_DATABASE_EXCEPTION, ErrorCategory::GetInstance());
            }
            
            if(mysql_num_rows(result))
            {
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
                        
                return std::error_code(ERR_SUCCESS, ErrorCategory::GetInstance());
            }
            else
            {
                return std::error_code(ERR_DATABASE_QUERY_FAILED, ErrorCategory::GetInstance());
            }
        }
        else
        {
            return std::error_code(ERR_DATABASE_EXCEPTION, ErrorCategory::GetInstance());
        }
    }
    
}}