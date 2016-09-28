#include "MariaDBConnection.h"

namespace Santiago{ namespace SantiagoDBTables
{
    MariaDBConnection::MariaDBConnection()
    {
        MYSQL* con1 = mysql_init(NULL);
        
        if(con1 == NULL) 
        {
            exit(1);
        }
        
        if(mysql_real_connect(con1, "localhost", "root", "kefas123", 
                               "databaseName", 0, NULL, 0) == NULL) //replace kefas123 with MariaDB password
        {
            mysql_close(con1);
            exit(1);
        }
        
        if(mysql_query(con1, "SELECT COALESCE(MAX(ID), 0) FROM USER_PROFILE"))
        {
            mysql_close(con1);
            exit(1);
        }
        
        MYSQL_RES *result1 = mysql_store_result(con1);
        MYSQL_ROW row1;
        
        if(result1 == NULL) 
        {
            mysql_close(con1);
            exit(1);
        }
        
        row1 = mysql_fetch_row(result1);
        _userProfileId = atoi(row1[0]);
        
        MYSQL* con2 = mysql_init(NULL);
        
        if(con2 == NULL) 
        {
            exit(1);
        }
        
        if(mysql_real_connect(con2, "localhost", "root", "kefas123", 
                               "databaseName", 0, NULL, 0) == NULL) //replace kefas123 with MariaDB password
        {
            mysql_close(con2);
            exit(1);
        }
        
        if(mysql_query(con2, "SELECT COALESCE(MAX(ID), 0) FROM SESSION"))
        {
            mysql_close(con2);
            exit(1);
        }
        
        MYSQL_RES *result2 = mysql_store_result(con2);
        MYSQL_ROW row2;
        
        if(result2 == NULL) 
        {
            mysql_close(con2);
            exit(1);
        }
        
        row2 = mysql_fetch_row(result2);
        _sessionId = atoi(row2[0]);
        
        MYSQL* con3 = mysql_init(NULL);
        
        if(con3 == NULL) 
        {
            exit(1);
        }
        
        if(mysql_real_connect(con3, "localhost", "root", "kefas123", 
                               "databaseName", 0, NULL, 0) == NULL) //replace kefas123 with MariaDB password
        {
            mysql_close(con3);
            exit(1);
        }
        
        if(mysql_query(con3, "SELECT COALESCE(MAX(ID), 0) FROM PERMISSION"))
        {
            mysql_close(con3);
            exit(1);
        }
        
        MYSQL_RES *result3 = mysql_store_result(con3);
        MYSQL_ROW row3;
        
        if(result3 == NULL) 
        {
            mysql_close(con3);
            exit(1);
        }
        
        row3 = mysql_fetch_row(result3);
        _permissionId = atoi(row3[0]);
        
        _permission =
            {{UserPermission::OWNER, "OWNER"}, 
             {UserPermission::READ, "READ"},
             {UserPermission::WRITE, "WRITE"},
             {UserPermission::READ_WRITE, "READ_WRITE"}};
    }

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
        
        return 1;
    }

    bool MariaDBConnection::disconnect()
    {
        mysql_close(con);
        return 1;
    }
    
    bool MariaDBConnection::addUserProfileRecord(const std::string userName_, const std::string password_)
    {
        if(connect())
        {
            if(_userProfileId)
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
            }
            
            MYSQL* conAdd = mysql_init(NULL);
            
            if(conAdd == NULL) 
            {
                return 0;
            }
            
            if(mysql_real_connect(conAdd, "localhost", "root", "kefas123", 
                                   "databaseName", 0, NULL, 0) == NULL) //replace kefas123 with MariaDB password
            {
                mysql_close(conAdd);
                return 0;
            }
            
            ++_userProfileId;
            std::stringstream userProfileId;
            userProfileId << _userProfileId;
            
            std::string insertQuery = "INSERT INTO USER_PROFILE VALUES('" +
                userProfileId.str() + "', '" + userName_ + "', '" + password_ + "')";
            
            if(mysql_query(conAdd, insertQuery.c_str()))
            {
                mysql_close(conAdd);
                return 0;
            }
            
            mysql_close(conAdd);
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
                    MYSQL* conUpdate = mysql_init(NULL);
                    if(conUpdate == NULL) 
                    {
                        return 0;
                    }
                
                    if(mysql_real_connect(conUpdate, "localhost", "root", "kefas123", 
                                          "databaseName", 0, NULL, 0) == NULL) //replace kefas123 with MariaDB password
                    {
                        mysql_close(conUpdate);
                        disconnect();
                        return 0;
                    }
                    
                    std::string updateQuery = "UPDATE USER_PROFILE SET PASSWORD='" +
                        newPassword_ + "' WHERE USERNAME='" + userId_ +"'";
                    
                    if(mysql_query(conUpdate, updateQuery.c_str()))
                    {              
                        mysql_close(conUpdate);
                        disconnect();
                        return 0;
                    }
                    
                    mysql_close(conUpdate);
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
            ++_sessionId;
            std::stringstream sessionId;
            sessionId << _sessionId;
            
            std::map<std::string, std::string> months
            {{"Jan", "01"},
             {"Feb", "02"},
             {"Mar", "03"},
             {"Apr", "04"},
             {"May", "05"},
             {"Jun", "06"},
             {"Jul", "07"},
             {"Aug", "08"},
             {"Sep", "09"},
             {"Oct", "10"},
             {"Nov", "11"},
             {"Dec", "12"}
            };
            
            std::stringstream loginTime;
            loginTime << loginTime_;
            std::string login = loginTime.str().substr(0, 5);
            auto searchLogin = months.find(loginTime.str().substr(5, 3));
            login += searchLogin->second + loginTime.str().substr(8,12);
            
            std::string insertQuery = "INSERT INTO SESSION(ID,USERNAME,COOKIE_ID,LOGIN_TIME) VALUES('" +
                sessionId.str() + "', '" + userName_ + "', '" +
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
            std::map<std::string, std::string> months
            {{"Jan", "01"},
             {"Feb", "02"},
             {"Mar", "03"},
             {"Apr", "04"},
             {"May", "05"},
             {"Jun", "06"},
             {"Jul", "07"},
             {"Aug", "08"},
             {"Sep", "09"},
             {"Oct", "10"},
             {"Nov", "11"},
             {"Dec", "12"}
            };
        
            std::stringstream logoutTime;
            logoutTime << logoutTime_;
            std::string logout = logoutTime.str().substr(0, 5);
            auto searchLogout = months.find(logoutTime.str().substr(5, 3));
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
                                                std::string userName_,
                                                UserPermission permission_)
    {
        if(connect())
        {
            ++_permissionId;
            std::stringstream permissionId;
            permissionId << _permissionId;
            std::stringstream resId;
            resId << resId_;
            auto search = _permission.find(permission_);
            
            std::string insertQuery = "INSERT INTO PERMISSION VALUES(" +
                permissionId.str() + ", " + resId.str() + ", '" + userName_ + "', '" + search->second + "')";
            
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
    
    bool MariaDBConnection::getUserProfileRecord(std::string userName_,
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

    bool MariaDBConnection::getSessionRecord(std::string userName_,
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
            
            std::map<std::string, std::string> months
            {{"01", "Jan"},
             {"02", "Feb"},
             {"03", "Mar"},
             {"04", "Apr"},
             {"05", "May"},
             {"06", "Jun"},
             {"07", "Jul"},
             {"08", "Aug"},
             {"09", "Sep"},
             {"10", "Oct"},
             {"11", "Nov"},
             {"12", "Dec"}
            };
            
            while((row = mysql_fetch_row(result))) 
            { 
                sessionRecord_->_id = atoi(row[0]);
                sessionRecord_->_userName = row[1];
                sessionRecord_->_cookieId = row[2];
                
                std::stringstream loginTime;
                loginTime << row[3];
                std::string login = loginTime.str().substr(0, 5);
                auto searchLogin = months.find(loginTime.str().substr(5, 2));
                login += searchLogin->second + loginTime.str().substr(7,12);
                loginTime.str("");
                loginTime << login;
                loginTime >> sessionRecord_->_loginTime;
            
                std::stringstream logoutTime;
                logoutTime << row[4];
                std::string logout = logoutTime.str().substr(0, 5);
                auto searchLogout = months.find(logoutTime.str().substr(5, 2));
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

    bool MariaDBConnection::getPermissionRecord(std::string userName_,
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
            
            std::map<std::string, UserPermission> permission =
                {{"OWNER", UserPermission::OWNER}, 
                 {"READ", UserPermission::READ},
                 {"WRITE", UserPermission::WRITE},
                 {"READ_WRITE", UserPermission::READ_WRITE}};
            
            while((row = mysql_fetch_row(result))) 
            { 
                permissionRecord_->_id = atoi(row[0]);
                permissionRecord_->_resId = atoi(row[1]);
                permissionRecord_->_userName = row[2];
                std::string permissionString = row[3];
                auto search = permission.find(permissionString);
                if(search != permission.end())
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
