#include "MariaDBConnection.h"

namespace Santiago{ namespace SantiagoDBTables
{
    std::error_code MariaDBConnection::connect()
    {       
        _mysql = mysql_init(NULL);

        if(con == NULL) 
        {
            return std::error_code(Error::DATABASE_EXCEPTION, Error::ErrorCode::GetInstance());
        }
        
        if(mysql_real_connect(_mysql,_host,_user,_passwd,
                              _db,_port,_unixSocket,_flags) == NULL) //replace kefas123 with MariaDB password
        {
            return std::error_code(Error::DATABASE_EXCEPTION, Error::ErrorCode::GetInstance());
        }
        
        return std::error_code(Error::SUCCESS, Error::ErrorCode::GetInstance());
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
        return std::error_code(Error::SUCCESS, Error::ErrorCode::GetInstance());
    }
    
    std::error_code MariaDBConnection::addUserProfilesRec(UserProfilesRec& userProfilesRec_)
    {
        if(isConnected())
        {
            if(getUserProfilesRec(userProfilesRec_.userName_,
                                  boost::optional<UserProfile>& userProfilesRecord_) != Error::SUCCESS)
            {
                std::string addUserProfilesRecQuery = "INSERT INTO USER_PROFILE(USERNAME,PASSWORD) VALUES('" +
                    userProfilesRec_.userName_ + "', '" + userProfilesRec_.password_ + "')";
                
                if(mysql_query(_mysql, addUserProfilesRecQuery.c_str()))
                {
                    return std::error_code(Error::DATABASE_EXCEPTION, Error::ErrorCode::GetInstance());
                }
            }
            else
            {
                // disconnect();
                return std::error_code(Error::USERNAME_ALREADY_EXISTS, Error::ErrorCode::GetInstance());
            }
            
            disconnect();
            return std::error_code(Error::SUCCESS, Error::ErrorCode::GetInstance());
        }
        else
        {
            return std::error_code(Error::DATABASE_EXCEPTION, Error::ErrorCategory::GetInstance());
        }
    }

    std::error_code MariaDBConnection::getUserProfilesRec(const std::string userName_,
                                                          boost::optional<UserProfile>& userProfilesRec_)
    {
        if(isConnected())
        {
            std::string getUserProfilesRecQuery = "SELECT * FROM USER_PROFILE WHERE USERNAME = '" + userName_ + "'";
            
            if(mysql_query(_mysql, getUserProfilesRecQuery.c_str()))
            {
                // disconnect();
                retur std::error_code(Error::DATABASE_EXCEPTION, Error::ErrorCode::GetInstance());
            }
        
            MYSQL_RES *result = mysql_store_result(_mysql);
            
            if(result == NULL) 
            {
                //disconnect();
                return  std::error_code(Error::DATABASE_EXCEPTION, Error::ErrorCode::GetInstance());
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
                //disconnect();
                
                return std::error_code(Error::SUCCESS, Error::ErrorCode::GetInstance());
            }
            else
            {
                return std::error_code(Error::DATABASE_QUERY_FAILED, Error::ErrorCode::GetInstance());
            }
        }
        else
        {
            return std::error_code(Error::DATABASE_EXCEPTION, Error::ErrorCode::GetInstance());
        }
    }
       
    std::error_code MariaDBConnection::updateUserProfilesRec(UserProfilesRec& userProfilesRec_,
                                                             const std::string newPassword_)
    {
        if(isConnected())
        {
            std::string retrieveOldPassword = "SELECT PASSWORD FROM USER_PROFILE WHERE USERNAME = '"
                + userProfilesRec_.userId_ + "'";
        
            if(mysql_query(_mysql, retrieveOldPassword.c_str()))
            {
                //disconnect();
                return std::error_code(Error::DATABASE_EXCEPTION, Error::ErrorCode::GetInstance());
            }
            
            MYSQL_RES *result = mysql_store_result(_mysql);
            
            if(result == NULL) 
            {
                //disconnect();
                return std::error_code(Error::DATABASE_EXCEPTION, Error::ErrorCode::GetInstance());
            }
                    
            if(mysql_num_rows(result))
            {
                MYSQL_ROW row;
                row = mysql_fetch_row(result);
                        
                if(oldPassword_ != row[0])
                {
                    //  disconnect();
                    return std::error_code(Error::INVALID_USERNAME_PASSWORD, Error::ErrorCode::GetInstance());
                }
                
                else
                {                   
                    //disconnect();
                    
                    if(isConnected())
                    {
                        std::string updateUserProfilesRecQuery = "UPDATE USER_PROFILE SET PASSWORD='" +
                            newPassword_ + "' WHERE USERNAME = '" + userProfilesRec_.userId_ +"'";
                        
                        if(mysql_query(_mysql, updateUserProfilesRecQuery.c_str()))
                        {              
                            //      disconnect();
                            return  std::error_code(Error::DATABASE_EXCEPTION, Error::ErrorCategory::GetInstance());
                        }
                        
                        //disconnect();
                        return  std::error_code(Error::SUCCESS, Error::ErrorCategory::GetInstance());
                    }
                    else
                    {
                        return  std::error_code(Error::DATABASE_EXCEPTION, Error::ErrorCode::GetInstance());
                    }
                }
            }
            else
            {
                // disconnect();
                return std::error_code(Error::DATABASE_QUERY_FAILED, Error::ErrorCode::GetInstance());
            }
        }
        else
        {
            return std::error_code(Error::DATABASE_EXCEPTION, Error::ErrorCode::GetInstance());
        }  
    }

    std::error_code MariaDBConnection::deleteUserProfilesRec(const std::string& userName_)
    {
        if(isConnected())
        {
            std::string deleteUserProfilesRecQuery = "DELETE FROM USER_PROFILE WHERE USERNAME = '" +
                userName_ + ")";
                    
            if(mysql_query(_mysql, deleteUserProfilesRecQuery.c_str()))
            {
                //disconnect();
                return std::error_code(Error::DATABASE_EXCEPTION, Error::ErrorCode::GetInstance());
            }
            
            //disconnect();
            return std::error_code(Error::SUCCESS, Error::ErrorCode::GetInstance());
        }
        else
        {
            return std::error_code(Error::DATABASE_EXCEPTION, Error::ErrorCode::GetInstance());
        }
    }
        
    std::error_code MariaDBConnection::addSessionsRec(SessionsRec& sessionsRec_)
    {
        if(isConnected())
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
                //  disconnect();
                return std::error_code(Error::DATABASE_EXCEPTION, Error::ErrorCode::GetInstance());
            }
            
            //disconnect();
            return std::error_code(Error::SUCCESS, Error::ErrorCode::GetInstance());
        }
        else
        {
            return std::error_code(Error::DATABASE_EXCEPTION, Error::ErrorCode::GetInstance());
        }
    }
      
    std::error_code MariaDBConnection::getSessionsRec(const std::string userName_,
                                                      boost::optional<Session>& sessionsRec_)
    {
        if(isConnected())
        {
            std::string getSessionsRecQuery = "SELECT * FROM SESSION WHERE USERNAME = '" + userName_ + "'";
            
            if(mysql_query(_mysql, getSessionsRecQuery.c_str()))
            {
                //disconnect();
                return std::error_code(Error::DATABASE_EXCEPTION, Error::ErrorCode::GetInstance());
            }
            
            MYSQL_RES *result = mysql_store_result(_mysql);
            
            if(result == NULL) 
            {
                //disconnect();
                return std::error_code(Error::DATABASE_EXCEPTION, Error::ErrorCode::GetInstance());
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
                //disconnect();
                
                return std::error_code(Error::SUCCESS, Error::ErrorCode::GetInstance());
            }
            else
            {
                return std::error_code(Error::DATABASE_EXCEPTION, Error::ErrorCode::GetInstance());
            }
        }
        else
        {
            return std::error_code(Error::DATABASE_EXCEPTION, Error::ErrorCode::GetInstance());
        }
    }

    std::error_code MariaDBConnection::updateSessionsRec(SessionsRec& sessionsRec_)
    {
        if(isConnected())
        {        
            std::stringstream logoutTime;
            logoutTime << sessionsRec_.logoutTime_;
            std::string logout = logoutTime.str().substr(0, 5);
            auto searchLogout = alphabetDigit.find(logoutTime.str().substr(5, 3));
            logout += searchLogout->second + logoutTime.str().substr(8, 12);
            std::string updateSessionsRecQuery = "UPDATE SESSION SET LOGOUT_TIME='" +
                logout + "' WHERE USERNAME='" + sessionsRec_.userId_ + "'";
            
            if(mysql_query(_mysql, updateSessionsRecQuery.c_str()))
            {
                //disconnect();
                return std::error_code(Error::DATABASE_EXCEPTION, Error::ErrorCode::GetInstance());
            }
            
            //disconnect();
            return std::error_code(Error::SUCCESS, Error::ErrorCode::GetInstance());
        }
        else
        {
            return std::error_code(Error::DATABASE_EXCEPTION, Error::ErrorCode::GetInstance());
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
                //  disconnect();
                return std::error_code(Error::DATABASE_EXCEPTION, Error::ErrorCode::GetInstance());
            }
            
            // disconnect();
            return std::error_code(Error::SUCCESS, Error::ErrorCode::GetInstance());
        }
        else
        {
            return std::error_code(Error::DATABASE_EXCEPTION, Error::ErrorCode::GetInstance());
        }
    }
  
    std::error_code MariaDBConnection::getPermissionsRec(const std::string userName_,
                                                         boost::optional<Permission>& permissionsRec_)
    {
        if(isConnected())
        {
            std::string getPermissionsRecQuery = "SELECT * FROM PERMISSION WHERE USERNAME = '" + userName_ + "'";
            
            if(mysql_query(_mysql, getPermissionsRecQuery.c_str()))
            {
                //  disconnect();
                return std::error_code(Error::DATABASE_EXCEPTION, Error::ErrorCode::GetInstance());
            }
            
            MYSQL_RES *result = mysql_store_result(_mysql);
            
            if(result == NULL) 
            {
                //disconnect();
                return std::error_code(Error::DATABASE_EXCEPTION, Error::ErrorCode::GetInstance());
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
                //disconnect();
                           
                return std::error_code(Error::SUCCESS, Error::ErrorCode::GetInstance());
            }
            else
            {
                return std::error_code(Error::DATABASE_QUERY_FAILED, Error::ErrorCode::GetInstance());
            }
        }
        else
        {
            return std::error_code(Error::DATABASE_EXCEPTION, Error::ErrorCode::GetInstance());
        }
    }
    
}}
