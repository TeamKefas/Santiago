#ifndef SANTIAGO_AUTHENTICATION_AUTHENTICATORIMPLBASE_H
#define SANTIAGO_AUTHENTICATION_AUTHENTICATORIMPLBASE_H

#include <string>
#include <system_error>

#include <boost/asio/spawn.hpp>
#include <boost/optional.hpp>

#include "Common.h"
#include "OICProviderData.h"

namespace Santiago{ namespace Authentication
{
    class AuthenticatorImplBase
    {
    public:

        AuthenticatorImplBase(const std::map<std::string,std::string>& oicProviderNameCertURLMap_)
        {
            for(auto& providerNameCertURLPair: oicProviderNameCertURLMap_)
            {
                bool flag = _oicProviderNameDataMap.insert(
                    std::make_pair(providerNameCertURLPair.first,
                                   OICProviderData(providerNameCertURLPair.second))).second;
                ST_ASSERT(flag);
            }
        }

        virtual std::error_code createUser(const std::string& userName_,
                                           const std::string& emailAddress_,
                                           const std::string& password_,
                                           boost::asio::yield_context yield_) = 0;

        virtual std::pair<std::error_code,boost::optional<std::pair<UserInfo,std::string> > >
        loginUser(const std::string& userNameOrEmailAddress_,
                  bool isUserNameNotEmailAddress_,
                  const std::string& password_,
                  boost::asio::yield_context yield_) = 0;

        virtual std::pair<std::error_code,boost::optional<std::pair<UserInfo,std::string> > >
        loginUserWithOICTokenId(const std::string& oicProviderName_,
                                const std::string& emailAddress_,
                                const JWSPtr& tokenId_,
                                boost::asio::yield_context yield_) = 0;
               
        virtual std::pair<std::error_code,boost::optional<UserInfo> >
        verifyCookieAndGetUserInfo(const std::string& cookieString_,
                                   boost::asio::yield_context yield_) = 0;

        virtual std::error_code logoutUserForCookie(const std::string& cookieString_,
                                                    boost::asio::yield_context yield_) = 0;

        virtual std::error_code logoutUserForAllCookies(const std::string& userName_,
                                                        boost::asio::yield_context yield_) = 0;

        virtual std::error_code changeUserPassword(const std::string& cookieString_,
                                                   const std::string& oldPassword_,
                                                   const std::string& newPassword_,
                                                   boost::asio::yield_context yield_) = 0;

        virtual std::pair<std::error_code,boost::optional<std::string> >
        getUserForEmailAddressAndRecoveryString(const std::string& emailAddress_,
                                                const std::string& recoveryString_,
                                                boost::asio::yield_context yield_) = 0;

        virtual std::error_code changeUserPasswordForEmailAddressAndRecoveryString(
            const std::string& emailAddress_,
            const std::string& recoveryString_,
            const std::string& newPassword_,
            boost::asio::yield_context yield_) = 0;

        virtual std::error_code changeUserEmailAddress(const std::string& cookieString_,
                                                       const std::string& newEmailAddress_,
                                                       const std::string& password_,
                                                       boost::asio::yield_context yield_) = 0;

        virtual std::pair<std::error_code,boost::optional<std::string> >
        createAndReturnRecoveryString(const std::string& emailAddress_,
                                      boost::asio::yield_context yield_) = 0;

        virtual std::error_code deleteUser(const std::string& cookieString_,boost::asio::yield_context yield_) = 0;
        
    protected:
        
        std::map<std::string,OICProviderData> _oicProviderNameDataMap;

    };
}}

#endif
