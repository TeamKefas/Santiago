#ifndef SANTIAGO_SANTIAGODBTABLES_MARIADBCONNECTION_H
#define SANTIAGO_SANTIAGODBTABLES_MARIADBCONNECTION_H

/**
 * @file MariaDBConnection.h
 *
 * @section DESCRIPTION
 *
 * Contains the MariaDBConnection class.
 */

#include <functional>
#include <iostream>
#include <vector>
#include <sstream>

#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>

#include "../ErrorCategory.h"

#include "DatabaseRecords.h"
#include "Definitions.h"

struct st_mysql;
struct st_mysql_res;

namespace Santiago{ namespace SantiagoDBTables
{
    class MariaDBConnection
    {
        typedef st_mysql MYSQL;
        typedef st_mysql_res MYSQL_RES;

    public:
/**
 * The constructor
 * @param config_- the ioservice to use
 */

        MariaDBConnection(const boost::property_tree::ptree& config_);

        virtual ~MariaDBConnection();
        

        // possible error_code returns for the following fns
        // SUCCESS, DATABASE_EXCEPTION, DATABASE_QUERY_FAILED

/**
 * Adds the record to the users table in database.
 * @param usersRec_ : Record to be added to the users table in the database
 * @param error 
 */
        void addUsersRec(UsersRec& usersRec_, std::error_code& error_);
/**
 * Gets user record from the database for the given username.
 * @param userName_
 * @param error
 */   
     	boost::optional<UsersRec> getUsersRecForUserName(const std::string& userName_, std::error_code& error_);
/**
 * Gets user record from the databasae for the given email address.
 * @param emailAddress_
 * @param error
 */        
	boost::optional<UsersRec> getUsersRecForEmailAddress(const std::string& emailAddress_, std::error_code& error_);
/**
 * Updates the given record in users table in the database.
 * @param newUsersRec_ : Updated user record object.
 * @param error
 */ 
        void updateUsersRec(UsersRec& newUsersRec_, std::error_code& error_);
/**
 * Deletes the record from users table in the database for the given username.
 * @param userName_
 * @param error
 */ 
        void deleteUsersRec(const std::string& userName_, std::error_code& error_);
/**
 * Adds the record to the sessions table in database.
 * @param sessionsRec_ : Record to be added to the sessions table in the database
 * @param error 
 */        
        void addSessionsRec(SessionsRec& sessionsRec_, std::error_code& error_);
/**
 * Gets session record from the databasae for the given cookie string.
 * @param cookieString_
 * @param error 
 */
        boost::optional<SessionsRec> getSessionsRec(const std::string& cookieString_, std::error_code& error_);
/**
 * Updates the given record in sessions table in the database.
 * @param sessionsRec_ : Updated session record object.
 * @param error
 */ 
        void updateSessionsRec(SessionsRec& sessionsRec_, std::error_code& error_);
/**
 * Gets active session records from the database.
 * @param error
 */
        std::vector<SessionsRec> getActiveSessions(std::error_code& error_);
        
    protected:
/**
 * Connects to the database.
 * @param error
 */     
        void connect(std::error_code& error_);
/**
 * Disconnects the database.
 * @param error
 */   
	void disconnect(std::error_code& error_);
/**
 * Gets user record from the database.
 * @param queryString_ : Query to be executed. 
 * @param error
 */
        boost::optional<UsersRec> getUsersRecImpl(
            const std::string& queryString_, std::error_code& error_);
/**
 * Returns the string after truncating escape characters.
 * @param nonEscapedString_ : Input string from the user. 
 */
        std::string getEscapedString(const std::string& nonEscapedString_);
/**
 * Returns true when user input string and it's escaped string are same.
 * @param userInput_ : Input string from the user. 
 */
        bool isUserInputClean(const std::string& userInput_);
/**
 * Runs the query in the database.
 * @param queryString_ : Query to be executed. 
 * @param error
 */
        void runQueryImpl(const std::string& queryString_, std::error_code& error_);
/**
 * Runs the insert query and returns the id.
 * @param queryString_ : Query to be executed. 
 * @param error
 */
        int runInsertQuery(const std::string& queryString_, std::error_code& error_);
/**
 * Runs the select query and stores the result using postQueryFn_ function.
 * @param queryString_ : Query to be executed. 
 * @param postQueryFn_ : Stores the results from the database.
 * @param error
 */
        void runSelectQuery(const std::string& queryString_,
                            const std::function<void(MYSQL_RES *, std::error_code&)>& postQueryFn_,
                            std::error_code& error_);
/**
 * Runs the update query.
 * @param queryString_ : Query to be executed. 
 * @param error
 */
        void runUpdateQuery(const std::string& queryString_, std::error_code& error_);
/**
 * Runs the delete query.
 * @param queryString_ : Query to be executed. 
 * @param error
 */
        void runDeleteQuery(const std::string& queryString_, std::error_code& error_);        

        boost::property_tree::ptree   _config;
        MYSQL                        *_mysql;

    };

    //Factory function for use in ThreadSpecificVar
    MariaDBConnection* CreateMariaDBConnection(const boost::property_tree::ptree& config_);

}}

#endif
