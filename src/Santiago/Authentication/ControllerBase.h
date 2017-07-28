#ifndef SANTIAGO_AUTHENTICATION_CONTROLLERBASE_H
#define SANTIAGO_AUTHENTICATION_CONTROLLERBASE_H

/**
 * @file ControllerBase.h
 *
 * @section DESCRIPTION
 *
 * Contains the ControllerBase class 
 */

#include <functional>
#include <map>
#include <system_error>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <sstream>

#include <openssl/sha.h>

#include <boost/asio/spawn.hpp>

#include "Santiago/SantiagoDBTables/MariaDBConnection.h"
#include "Santiago/Authentication/AuthenticatorBase.h"
#include "Santiago/Thread/ThreadSpecificVar.h"
#include "Santiago/Utils/STLog.h"

#include "ControllerData.h"

#define MAX_SESSION_DURATION 1

namespace Santiago{ namespace Authentication
{
    template<typename ClientIdType>
    class ControllerBase
    {
    public:
        
        typedef Thread::ThreadSpecificVar<SantiagoDBTables::MariaDBConnection> ThreadSpecificDbConnection;
        typedef ControllerData<ClientIdType> MyControllerData;
        
        /**
         * The constructor
         * @param databaseConnection_- Used for multithreaded operations.  
         * @param ioService_- the ioservice to use.
         * @param config_- Used to hold the json data.
         */
        ControllerBase(ThreadSpecificDbConnection& databaseConnection_,
                       boost::asio::io_service& ioService_,
                       const boost::property_tree::ptree& config_);
        /**
         * The destructor
         */
	virtual ~ControllerBase()
	{}

        std::error_code createUser(const std::string& userName_,
                                   const std::string& emailAddress_,
                                   const std::string& password_,
                                   boost::asio::yield_context yield_);

        std::pair<std::error_code,boost::optional<std::pair<UserInfo,std::string> > >
        loginUser(const ClientIdType& clientId_,
                  const std::string& userNameOrEmailAddress_,
                  bool isUserNameNotEmailAddress_,
                  const std::string& password_,
                  boost::asio::yield_context yield_);
        
        std::pair<std::error_code,boost::optional<UserInfo> >
        verifyCookieAndGetUserInfo(const ClientIdType& clientId_,
                                   const std::string& cookieString_,
                                   boost::asio::yield_context yield_);

        std::error_code logoutUserForCookie(const std::string& cookieString_,boost::asio::yield_context yield_);

        std::error_code logoutUserForAllCookies(const std::string& userName_,boost::asio::yield_context yield_);

        std::error_code changeUserPassword(const std::string& cookieString_,
                                           const std::string& oldPassword_,
                                           const std::string& newPassword_,
                                           boost::asio::yield_context yield_);

        std::pair<std::error_code,boost::optional<std::string> >
        getUserForEmailAddressAndRecoveryString(const std::string& emailAddress_,
                                                const std::string& recoveryString_,
                                                boost::asio::yield_context yield_);

        std::error_code changeUserPasswordForEmailAddressAndRecoveryString(const std::string& emailAddress_,
                                                                           const std::string& recoverystring_,
                                                                           const std::string& newPassword_,
                                                                           boost::asio::yield_context yield_);

        std::error_code changeUserEmailAddress(const std::string& cookieString_,
                                               const std::string& newEmailAddress_,
                                               const std::string& password_,
                                               boost::asio::yield_context yield_);

        std::pair<std::error_code,boost::optional<std::string> >
        createAndReturnRecoveryString(const std::string& emailAddress_,boost::asio::yield_context yield_);

        std::error_code deleteUser(const std::string& cookieString_,boost::asio::yield_context yield_);

    protected:
        
        std::pair<std::error_code,boost::optional<SantiagoDBTables::UsersRec> > 
        verifyUserNamePasswordAndGetUsersRec(const std::string& userName_, const std::string& password_);

        std::pair<std::error_code,boost::optional<SantiagoDBTables::UsersRec> > 
        verifyEmailAddressPasswordAndGetUsersRec(const std::string& emailAddress_, const std::string& password_);

        std::pair<std::error_code,boost::optional<SantiagoDBTables::UsersRec> > 
        verifyEmailAddressRecoveryStringAndGetUsersRec(const std::string& emailAddress_,
                                                       const std::string& recoveryString_);
        std::error_code logoutUserForAllCookiesImpl(const std::string& userName_,
                                                    boost::asio::yield_context yield_);
        std::pair<std::error_code,boost::optional<std::string> >
        createAndReturnRecoveryStringImpl(const std::string& emailAddress_,boost::asio::yield_context yield_);
         std::error_code
         changeUserPasswordForEmailAddressAndRecoveryStringImpl(const std::string& emailAddress_,
                                                           const std::string& recoveryString_,
                                                           const std::string& newPassword_,
                                                           boost::asio::yield_context yield_);
        std::error_code deleteUserImpl(const std::string& cookieString_,
                                                                     boost::asio::yield_context yield_);

        std::error_code cleanupLocalCookieDataAndUpdateSessionsRecord(
            const SantiagoDBTables::SessionsRec& sessionsRec_);

        std::error_code cleanupCookieDataAndUpdateSessionRecord(const std::string& cookieString_,
                                                                boost::asio::yield_context yield_);

        std::error_code cleanupLocalCookieDataAndUpdateSessionsRecordImpl(const SantiagoDBTables::SessionsRec& sessionsRec_);
        std::error_code cleanupCookieDataAndUpdateSessionRecordsForAllCookies(const std::string& userName_,
                                                                              boost::asio::yield_context yield_);
        boost::posix_time::time_duration getMaxSessionInactiveDuration() const;

        virtual std::error_code logoutCookieFromAllClients(const std::string& cookieString_,
                                                           boost::asio::yield_context yield_) = 0;
        
        virtual std::error_code logoutUserFromAllClients(const std::string& userName_,
                                                         boost::asio::yield_context yield_) = 0;

        std::string generateSHA256(const std::string str);
        std::string generateUniqueCookie();
        std::string generateRecoveryString();
        virtual boost::posix_time::time_duration getMaxSessionInactiveTime() const;

        ThreadSpecificDbConnection          &_databaseConnection;
        MyControllerData                     _localData;
        
    };
}}

#endif
