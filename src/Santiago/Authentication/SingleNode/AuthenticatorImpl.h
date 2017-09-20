#ifndef SANTIAGO_AUTHENTICATION_SINGLENODE_AUTHENTICATORIMPL_H
#define SANTIAGO_AUTHENTICATION_SINGLENODE_AUTHENTICATORIMPL_H

#include "Santiago/Authentication/AuthenticatorImplBase.h"
#include "Santiago/Thread/ThreadSpecificVar.h"

#include "Controller.h"

namespace Santiago{ namespace Authentication{ namespace SingleNode
{
    class AuthenticatorImpl:public AuthenticatorImplBase
    {
    public:
        
        typedef Thread::ThreadSpecificVar<SantiagoDBTables::MariaDBConnection> ThreadSpecificDbConnection;
        
        AuthenticatorImpl(ThreadSpecificDbConnection& databaseConnection_)
            :_controller(databaseConnection_)
        {}

        virtual std::error_code createUser(const std::string& userName_,
                                           const std::string& emailAddress_,
                                           const std::string& password_,
                                           boost::asio::yield_context yield_);

        virtual std::pair<std::error_code,boost::optional<std::pair<UserInfo,std::string> > >
        loginUser(const std::string& userNameOrEmailAddress_,
                  bool isUserNameNotEmailAddress_,
                  const std::string& password_,
                  boost::asio::yield_context yield_);
        
        virtual std::pair<std::error_code,boost::optional<UserInfo> >
        verifyCookieAndGetUserInfo(const std::string& cookieString_,
                                   boost::asio::yield_context yield_);

        virtual std::error_code logoutUserForCookie(const std::string& cookieString_,
                                                    boost::asio::yield_context yield_);

        virtual std::error_code logoutUserForAllCookies(const std::string& userName_,
                                                        boost::asio::yield_context yield_);

        virtual std::error_code changeUserPassword(const std::string& cookieString_,
                                                   const std::string& oldPassword_,
                                                   const std::string& newPassword_,
                                                   boost::asio::yield_context yield_);

        virtual std::pair<std::error_code,boost::optional<std::string> >
        getUserForEmailAddressAndRecoveryString(const std::string& emailAddress_,
                                                const std::string& recoveryString_,
                                                boost::asio::yield_context yield_);

        virtual std::error_code changeUserPasswordForEmailAddressAndRecoveryString(
            const std::string& emailAddress_,
            const std::string& recoverystring_,
            const std::string& newPassword_,
            boost::asio::yield_context yield_);

        virtual std::error_code changeUserEmailAddress(const std::string& cookieString_,
                                                       const std::string& newEmailAddress_,
                                                       const std::string& password_,
                                                       boost::asio::yield_context yield_);

        virtual std::pair<std::error_code,boost::optional<std::string> >
        createAndReturnRecoveryString(const std::string& emailAddress_,
                                      boost::asio::yield_context yield_);

        virtual std::error_code deleteUser(const std::string& cookieString_,boost::asio::yield_context yield_);

        Controller               _controller;

    };
    
}}}

#endif
