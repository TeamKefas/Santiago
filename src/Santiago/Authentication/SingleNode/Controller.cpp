#include "Controller.h"

#include "../ControllerBase.ipp"

namespace Santiago{ namespace Authentication
{
    template ControllerBase<SingleNode::ControllerTypes>::
    ControllerBase(ThreadSpecificDbConnection& databaseConnection_);
        
    template std::error_code ControllerBase<SingleNode::ControllerTypes>::
    createUser(const ClientRequestData& requestData_,
               const std::string& userName_,
               const std::string& emailAddress_,
               const std::string& password_,
               boost::asio::yield_context yield_);

    template std::pair<std::error_code,boost::optional<std::pair<UserInfo,std::string> > >
    ControllerBase<SingleNode::ControllerTypes>::
    loginUser(const ClientRequestData& requestData_,
              const std::string& userNameOrEmailAddress_,
              bool isUserNameNotEmailAddress_,
              const std::string& password_,
              boost::asio::yield_context yield_);

    template std::pair<std::error_code,boost::optional<UserInfo> >
    ControllerBase<SingleNode::ControllerTypes>::
    verifyCookieAndGetUserInfo(const ClientRequestData& requestData_,
                               const std::string& cookieString_,
                               boost::asio::yield_context yield_);

    template std::error_code ControllerBase<SingleNode::ControllerTypes>::
    logoutUserForCookie(const ClientRequestData& requestData_,
                        const std::string& cookieString_,
                        boost::asio::yield_context yield_);

    template std::error_code ControllerBase<SingleNode::ControllerTypes>::
    logoutUserForAllCookies(const ClientRequestData& requestData_,
                            const std::string& userName_,
                            boost::asio::yield_context yield_);

    template std::error_code ControllerBase<SingleNode::ControllerTypes>::
    changeUserPassword(const ClientRequestData& requestData_,
                       const std::string& cookieString_,
                       const std::string& oldPassword_,
                       const std::string& newPassword_,
                       boost::asio::yield_context yield_);

    template std::pair<std::error_code,boost::optional<std::string> > ControllerBase<SingleNode::ControllerTypes>::
    createAndReturnRecoveryString(const ClientRequestData& requestData_,
                                  const std::string& emailAddress_,
                                  boost::asio::yield_context yield_);

    template std::pair<std::error_code,boost::optional<std::string> > ControllerBase<SingleNode::ControllerTypes>::
    getUserForEmailAddressAndRecoveryString(const ClientRequestData& requestData_,
                                            const std::string& emailAddress_,
                                            const std::string& recoveryString_,
                                            boost::asio::yield_context yield_);

    template std::error_code ControllerBase<SingleNode::ControllerTypes>::
    changeUserPasswordForEmailAddressAndRecoveryString(const ClientRequestData& requestData_,
                                                       const std::string& emailAddress_,
                                                       const std::string& recoveryString_,
                                                       const std::string& newPassword_,
                                                       boost::asio::yield_context yield_);

    template std::error_code ControllerBase<SingleNode::ControllerTypes>::
    changeUserEmailAddress(const ClientRequestData& requestData_,
                           const std::string& cookieString_,
                           const std::string& newEmailAddress_,
                           const std::string& password_,
                           boost::asio::yield_context yield_);

    template std::error_code ControllerBase<SingleNode::ControllerTypes>::
    deleteUser(const ClientRequestData& requestData_,
               const std::string& cookieString_,
               boost::asio::yield_context yield_);

    template std::pair<std::error_code,boost::optional<SantiagoDBTables::UsersRec> >
    ControllerBase<SingleNode::ControllerTypes>::
    verifyUserNamePasswordAndGetUsersRec(const std::string& userName_, const std::string& password_);

    template std::pair<std::error_code,boost::optional<SantiagoDBTables::UsersRec> >
    ControllerBase<SingleNode::ControllerTypes>::
    verifyEmailAddressPasswordAndGetUsersRec(const std::string& emailAddress_,
                                             const std::string& password_);
    
    template std::pair<std::error_code,boost::optional<SantiagoDBTables::UsersRec> > 
    ControllerBase<SingleNode::ControllerTypes>::
    verifyEmailAddressRecoveryStringAndGetUsersRec(const std::string& emailAddress_,
                                                   const std::string& recoveryString_);

    template std::pair<std::error_code,boost::optional<SantiagoDBTables::SessionsRec> >
    ControllerBase<SingleNode::ControllerTypes>::
    partiallyVerifyCookieAndGetSessionsRec(const std::string& cookieString_);

    template std::error_code ControllerBase<SingleNode::ControllerTypes>::
    cleanupLocalCookieDataAndUpdateSessionsRecordImpl(SantiagoDBTables::SessionsRec& sessionsRec_);

    template std::error_code ControllerBase<SingleNode::ControllerTypes>::
    cleanupCookieDataAndUpdateSessionRecord(const ClientRequestData& requestData_,
                                            const std::string& cookieString_,
                                            boost::asio::yield_context yield_);

    template std::error_code ControllerBase<SingleNode::ControllerTypes>::
    cleanupCookieDataAndUpdateSessionRecordsForAllCookies(const ClientRequestData& requestData_,
                                                          const std::string& userName_,
                                                          boost::asio::yield_context yield_);

    template std::string ControllerBase<SingleNode::ControllerTypes>::
    generateSHA256(const std::string str);

    template std::string ControllerBase<SingleNode::ControllerTypes>::
    generateUniqueCookie(std::string userName_);

    template std::string ControllerBase<SingleNode::ControllerTypes>::
    generateRecoveryString();

    template boost::posix_time::time_duration ControllerBase<SingleNode::ControllerTypes>::
    getMaxSessionInactiveDuration() const;
            
}}
