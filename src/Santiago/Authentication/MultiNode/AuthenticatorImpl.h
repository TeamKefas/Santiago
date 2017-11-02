#ifndef SANTIAGO_AUTHENTICATION_MULTINODE_AUTHENTICATORIMPL_H
#define SANTIAGO_AUTHENTICATION_MULTINODE_AUTHENTICATORIMPL_H

#include <functional>
#include <map>
#include <system_error>

#include "Santiago/ErrorCategory.h"
#include "Santiago/Utils/STLog.h"

#include "Santiago/Authentication/AuthenticatorImplBase.h"

#include "ConnectionRequestsController.h"
#include "ClientCache.h"

namespace Santiago{ namespace Authentication{ namespace MultiNode
{
    typedef std::function<void(const std::error_code&,const boost::optional<UserInfo>&)>
    ErrorCodeUserInfoCallbackFn;
	
    typedef std::function<void(const std::error_code&,const boost::optional<std::string>&)>
    ErrorCodeStringCallbackFn;

    typedef std::function<void(const std::error_code&,const boost::optional<std::pair<UserInfo,std::string> >&)>
    ErrorCodeUserInfoStringPairCallbackFn;

    typedef std::function<void(const std::error_code&)> ErrorCodeCallbackFn;

    typedef std::function<void(const std::error_code&,const boost::optional<ConnectionMessage>&)> ErrorCodeConnectionMessageOptCallBackFn;
    
    class AuthenticatorImpl:public AuthenticatorImplBase
    {
    public:
        typedef std::shared_ptr<boost::asio::strand> StrandPtr;
        
        AuthenticatorImpl(boost::asio::io_service& ioService_,
                          const StrandPtr& strandPtr_,
                          const boost::asio::ip::tcp::endpoint& serverEndPoint_);

    protected:
        
        virtual void createUserImpl(const std::string& userName_,
                                    const std::string& emailAddress_,
                                    const std::string& password_,
                                    const ErrorCodeCallbackFn& onCreateUserCallbackFn_);
        
        void handleCreateUserConnectionMessage(const std::error_code& error_,
                                               const boost::optional<ConnectionMessage>& connectionMessageOpt_,
                                               const ErrorCodeCallbackFn& onCreateUserCallbackFn_);
        
        virtual void loginUserImpl(const std::string& userNameOrEmailAddress_,
                                   bool isUserNameNotEmailAddress_,
                                   const std::string& password_,
                                   const ErrorCodeUserInfoStringPairCallbackFn& onLoginUserCallbackFn_);
        
        void handleLoginUserConnectionMessage(const std::error_code& error_,
                                              const boost::optional<ConnectionMessage>& connectionMessageOpt_,
                                              const ErrorCodeUserInfoStringPairCallbackFn& onLoginUserCallbackFn_);

        virtual void verifyCookieAndGetUserInfoImpl(const std::string& cookieString_,
                                                    const ErrorCodeUserInfoCallbackFn& onVerifyUserCallbackFn_);
        
        void handleVerifyCookieConnectionMessage(const std::error_code& error_,
                                                 const boost::optional<ConnectionMessage>& connectionMessageOpt_,
                                                 const ErrorCodeUserInfoCallbackFn& onVerifyUserCallbackFn_);

        virtual void logoutUserForCookieImpl(const std::string& cookieString_,
                                             const ErrorCodeCallbackFn& onLogoutCookieCallbackFn_);

        void handleLogoutUserForCookieConnectionMessage(const std::error_code& error_,
                                                        const boost::optional<ConnectionMessage>& connectionMessageOpt_,
                                                        const ErrorCodeCallbackFn& onLogoutCookieCallbackFn_);

        virtual void logoutUserForAllCookiesImpl(const std::string& userName_,
                                                 const ErrorCodeCallbackFn& onLogoutAllCookiesCallbackFn_);

        void handleLogoutUserForAllCookiesConnectionMessage(const std::error_code& error_,
                                                            const boost::optional<ConnectionMessage>& connectionMessageOpt_,
                                                            const ErrorCodeCallbackFn& onLogoutAllCookiesCallbackFn_);

        virtual void changeUserPasswordImpl(const std::string& cookieString_,
                                            const std::string& oldPassword_,
                                            const std::string& newPassword_,
                                            const ErrorCodeCallbackFn& onChangePasswordCallbackFn_);

        void handleChangeUserPasswordConnectionMessage(const std::error_code& error_,
                                                       const boost::optional<ConnectionMessage>& connectionMessageOpt_,
                                                       const ErrorCodeCallbackFn& onChangePasswordCallbackFn_);

        virtual void getUserForEmailAddressAndRecoveryStringImpl(const std::string& emailAddress_,
                                                                 const std::string& recoveryString_,
                                                                 const ErrorCodeStringCallbackFn& onGetUserForEmailAddressAndRecoveryStringCallbackFn_);

        void handleGetUserForEmailAddressAndRecoveryStringConnectionMessage(const std::error_code& error_,
                                                                            const boost::optional<ConnectionMessage>& connectionMessageOpt_,
                                                                            const ErrorCodeStringCallbackFn& onGetUserForEmailAddressAndRecoveryStringCallbackFn_);

        virtual void changeUserPasswordForEmailAddressAndRecoveryStringImpl(const std::string& emailAddress_,
                                                                            const std::string& recoverystring_,
                                                                            const std::string& newPassword_,
                                                                            const ErrorCodeCallbackFn& onChangePasswordForEmailAddressAndRecoveryStringCallbackFn_);

        void handleChangeUserPasswordForEmailAddressAndRecoveryStringConnectionMessage(const std::error_code& error_,
                                                                                       const boost::optional<ConnectionMessage>& connectionMessageOpt_,
                                                                                       const ErrorCodeCallbackFn& onChangePasswordForEmailAddressAndRecoveryStringCallbackFn_);

        virtual void changeUserEmailAddressImpl(const std::string& cookieString_,
                                                const std::string& newEmailAddress_,
                                                const std::string& password_,
                                                const ErrorCodeCallbackFn& onChangeEmailAddressCallbackFn_);

        void handleChangeUserEmailAddressConnectionMessage(const std::error_code& error_,
                                                           const boost::optional<ConnectionMessage>& connectionMessageOpt_,
                                                           const ErrorCodeCallbackFn& onChangeEmailAddressCallbackFn_);
        
        virtual void createAndReturnRecoveryStringImpl(const std::string& emailAddress_,
                                                       const ErrorCodeStringCallbackFn& onCreateAndReturnRecoveryStringCallbackFn_);

        void handleCreateAndReturnRecoveryStringConnectionMessage(const std::error_code& error_,
                                                                  const boost::optional<ConnectionMessage>& connectionMessageOpt_,
                                                                  const ErrorCodeStringCallbackFn& onCreateAndReturnRecoveryStringCallbackFn_);

        virtual void deleteUserImpl(const std::string& cookieString_,
                                    const ErrorCodeCallbackFn& onDeleteUserCallbackFn_);

        void handleDeleteUserConnectionMessage(const std::error_code& error_,
                                               const boost::optional<ConnectionMessage>& connectionMessageOpt_,
                                               const ErrorCodeCallbackFn& onDeleteUserCallbackFn_);
        
        std::string generateSHA256(const std::string str);

        void handleServerRequestMessage(const ConnectionMessage& connectonMessage_);
        void handleConnectionDisconnect() {_lastPingTimeOpt.reset();}
        
        void handleServerRequestPing(const ConnectionMessage& connectionMessage_);
        void handleServerRequestLogoutUserForCookie(const ConnectionMessage& connectionMessage_);
        void handleServerRequestLogoutUserForAllCookies(const ConnectionMessage& connectionMessage_);


        std::error_code getErrorCodeFromConnectionMessage(const ConnectionMessage& connectonMessage_) const; //TODO

        boost::asio::io_service           &_ioService;
        StrandPtr                          _strandPtr;

        ConnectionRequestsController       _connectionRequestsController;
        ClientCache                        _clientCache;
        boost::optional<clock_t>           _lastPingTimeOpt;
        unsigned                           _lastRequestId;
    };
}}}

#endif
