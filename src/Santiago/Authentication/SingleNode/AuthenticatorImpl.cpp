#include <cjose/error.h>
#include <cjose/jws.h>

#include "AuthenticatorImpl.h"

namespace Santiago{ namespace Authentication{ namespace SingleNode
{
    AuthenticatorImpl::AuthenticatorImpl(ThreadSpecificDbConnection& databaseConnection_,
                                         const std::map<std::string,std::string>& oicProviderNameCertURLMap_)
        :AuthenticatorImplBase(oicProviderNameCertURLMap_)
        ,_controller(databaseConnection_)
    {}
    
    std::error_code AuthenticatorImpl::createUser(const std::string& userName_,
                                                  const std::string& emailAddress_,
                                                  const std::string& password_,
                                                  boost::asio::yield_context yield_)
    {
        return _controller.createUser(ControllerTypes::ClientRequestData(),
                                      userName_,
                                      emailAddress_,
                                      password_,
                                      yield_);
    }

    std::pair<std::error_code,boost::optional<std::pair<UserInfo,std::string> > >
    AuthenticatorImpl::loginUser(const std::string& userNameOrEmailAddress_,
                                 bool isUserNameNotEmailAddress_,
                                 const std::string& password_,
                                 boost::asio::yield_context yield_)
    {
        return _controller.loginUser(ControllerTypes::ClientRequestData(),
                                     userNameOrEmailAddress_,
                                     isUserNameNotEmailAddress_,
                                     password_,
                                     yield_);  
    }

    std::pair<std::error_code,boost::optional<std::pair<UserInfo,std::string> > >
    AuthenticatorImpl::loginUserWithOICTokenId(const std::string& oicProviderName_,
                                               const std::string& emailAddress_,
                                               const JWSPtr& tokenId_,
                                               boost::asio::yield_context yield_)
    {
        //TODO: See if we can move this fn to the base class
        auto iter = _oicProviderNameDataMap.find(oicProviderName_);
        if(iter == _oicProviderNameDataMap.end())
        {
            ST_LOG_INFO("Unsupported oic provider name: "<< oicProviderName_ <<std::endl);
            return std::make_pair(std::error_code(ErrorCode::ERR_UNSUPPORTED_OIC_PROVIDER),
                                  boost::none);
        }

        cjose_err error;
        error.code = CJOSE_ERR_NONE;
        cjose_header_t *headerRawPtr = cjose_jws_get_protected(tokenId_.get());
        const char *kid = cjose_header_get(headerRawPtr,"kid",&error);
        if(kid == NULL)
        {
            ST_ASSERT(error.code != CJOSE_ERR_NONE);
            ST_LOG_INFO("jws kid retrieval failed, at file "<< error.file <<":" << error.line
                        << ", message: " << error.message <<std::endl);
            return std::make_pair(std::error_code(ErrorCode::ERR_TOKENID_VERIFICATION_FAILED),
                                  boost::none);            
        }
        ST_ASSERT(error.code == CJOSE_ERR_NONE);
        
        JWKPtr key = iter->second.getJWKForKeyId(kid);
        if(!key)
        {
            return std::make_pair(std::error_code(ErrorCode::ERR_TOKENID_VERIFICATION_FAILED),
                                  boost::none);
        }

        bool isVerified = cjose_jws_verify(tokenId_.get(), key.get(), &error);

        if(!isVerified)
        {
            ST_ASSERT(error.code != CJOSE_ERR_NONE);
            ST_LOG_INFO("jws verification failed, at file "<< error.file <<":" << error.line
                        << ", message: " << error.message <<std::endl);
            return std::make_pair(std::error_code(ErrorCode::ERR_TOKENID_VERIFICATION_FAILED),
                                  boost::none);
        }

        return _controller.loginUserForVerifiedOICToken(ControllerTypes::ClientRequestData(),
                                                        emailAddress_,
                                                        yield_);
    }
        
    std::pair<std::error_code,boost::optional<UserInfo> >
    AuthenticatorImpl::verifyCookieAndGetUserInfo(const std::string& cookieString_,
                                                  boost::asio::yield_context yield_)
    {
        return _controller.verifyCookieAndGetUserInfo(ControllerTypes::ClientRequestData(),
                                                      cookieString_,
                                                      yield_);
    }

    std::error_code AuthenticatorImpl::logoutUserForCookie(const std::string& cookieString_,
                                                           boost::asio::yield_context yield_)
    {
        return _controller.logoutUserForCookie(ControllerTypes::ClientRequestData(),
                                               cookieString_,
                                               yield_);
    }

    std::error_code AuthenticatorImpl::logoutUserForAllCookies(const std::string& userName_,
                                                               boost::asio::yield_context yield_)
    {
        return _controller.logoutUserForAllCookies(ControllerTypes::ClientRequestData(),
                                                   userName_,
                                                   yield_);
    }

    std::error_code AuthenticatorImpl::changeUserPassword(const std::string& cookieString_,
                                                          const std::string& oldPassword_,
                                                          const std::string& newPassword_,
                                                          boost::asio::yield_context yield_)
    {
        return _controller.changeUserPassword(ControllerTypes::ClientRequestData(),
                                              cookieString_,
                                              oldPassword_,
                                              newPassword_,
                                              yield_);
    }

    std::pair<std::error_code,boost::optional<std::string> >
    AuthenticatorImpl::getUserForEmailAddressAndRecoveryString(const std::string& emailAddress_,
                                                               const std::string& recoveryString_,
                                                               boost::asio::yield_context yield_)
    {
        return _controller.getUserForEmailAddressAndRecoveryString(ControllerTypes::ClientRequestData(),
                                                                   emailAddress_,
                                                                   recoveryString_,
                                                                   yield_);
    }

    std::error_code AuthenticatorImpl::changeUserPasswordForEmailAddressAndRecoveryString(
        const std::string& emailAddress_,
        const std::string& recoveryString_,
        const std::string& newPassword_,
        boost::asio::yield_context yield_)
    {
        return _controller.changeUserPasswordForEmailAddressAndRecoveryString(ControllerTypes::ClientRequestData(),
                                                                              emailAddress_,
                                                                              recoveryString_,
                                                                              newPassword_,
                                                                              yield_);
    }

    std::error_code AuthenticatorImpl::changeUserEmailAddress(const std::string& cookieString_,
                                                              const std::string& newEmailAddress_,
                                                              const std::string& password_,
                                                              boost::asio::yield_context yield_)
    {
        return _controller.changeUserEmailAddress(ControllerTypes::ClientRequestData(),
                                                  cookieString_,
                                                  newEmailAddress_,
                                                  password_,
                                                  yield_);
    }

    std::pair<std::error_code,boost::optional<std::string> >
    AuthenticatorImpl::createAndReturnRecoveryString(const std::string& emailAddress_,
                                                     boost::asio::yield_context yield_)
    {
        return _controller.createAndReturnRecoveryString(ControllerTypes::ClientRequestData(),
                                                         emailAddress_,
                                                         yield_);
    }

    std::error_code AuthenticatorImpl::deleteUser(const std::string& cookieString_,
                                                  boost::asio::yield_context yield_)
    {
        return _controller.deleteUser(ControllerTypes::ClientRequestData(),
                                      cookieString_,
                                      yield_);
    }
    
}}}
