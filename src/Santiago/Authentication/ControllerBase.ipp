#ifndef SANTIAGO_AUTHENTICATION_CONTROLLERBASE_IPP
#define SANTIAGO_AUTHENTICATION_CONTROLLERBASE_IPP

#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <sstream>

#include <openssl/sha.h>
#include <boost/asio/spawn.hpp>

#include "../Utils/STLog.h"

#include "ControllerBase.h"

namespace Santiago{ namespace Authentication
{
    template<typename ControllerTypes>
    ControllerBase<ControllerTypes>::ControllerBase(ThreadSpecificDbConnection& databaseConnection_):
        _databaseConnection(databaseConnection_),
        _localData()
    {
        srand ( time(NULL) );
        std::error_code error;
        std::vector<SantiagoDBTables::SessionsRec> activeSessionsRec =
            _databaseConnection.get().getActiveSessions(error);
        if(error && (ErrorCode::ERR_DATABASE_GET_RETURNED_ZERO_RESULTS != error.value()))
        {
            ST_LOG_ERROR("Authenticator construction failed. Error message:"<<
                         error.message());
            throw std::runtime_error("Authenticator construction failed. Error message:" +
                                     error.message());
        }
        
        for(std::vector<SantiagoDBTables::SessionsRec>::iterator it = activeSessionsRec.begin();
            it != activeSessionsRec.end();++it)
        {
            boost::optional<SantiagoDBTables::UsersRec> usersRecOpt =
                _databaseConnection.get().getUsersRecForUserName(it->_userName,error);
            if(error)
            {
                ST_LOG_ERROR("Authenticator construction failed. Error message:"<<
                             error.message());
                throw std::runtime_error("Authenticator construction failed. Error message:" +
                                         error.message());
            }
            
            ST_ASSERT(usersRecOpt);
            _localData.addCookie(usersRecOpt->_userName,usersRecOpt->_emailAddress,*it,boost::none);
            
            if(boost::posix_time::second_clock::universal_time() - it->_lastActiveTime >=
               boost::posix_time::time_duration(MAX_SESSION_DURATION,0,0,0))
            {
                _localData.setCookieBeingLoggedOutFlag(it->_cookieString);
                std::error_code error = cleanupLocalCookieDataAndUpdateSessionsRecordImpl(*it);                    
                ST_ASSERT(!error);
            }
        }
    }

    template<typename ControllerTypes>
    std::error_code ControllerBase<ControllerTypes>::createUser(const ClientRequestData& requestData_,
                                                                const std::string& userName_,
                                                                const std::string& emailAddress_,
                                                                const std::string& password_,
                                                                boost::asio::yield_context yield_)
    {

        boost::optional<SantiagoDBTables::UsersRec> usersRecOpt;
        std::error_code error;

        //check for existing accounts with same userName
        std::tie(error,usersRecOpt) = verifyUserNamePasswordAndGetUsersRec(userName_,"");
        if((ErrorCode::ERR_DATABASE_EXCEPTION == error.value()) ||
           (ErrorCode::ERR_DATABASE_INVALID_USER_INPUT == error.value()))
        {
            return error;
        }
        else if(usersRecOpt)
        {
            return std::error_code(ErrorCode::ERR_USERNAME_ALREADY_EXISTS);
        }

        //check for existing accounts with same emailAddress
        std::tie(error,usersRecOpt) = verifyEmailAddressPasswordAndGetUsersRec(emailAddress_,"");
        if((ErrorCode::ERR_DATABASE_EXCEPTION == error.value()) ||
           (ErrorCode::ERR_DATABASE_INVALID_USER_INPUT == error.value()))
        {
            return error;
        }
        else if(usersRecOpt)
        {
            return std::error_code(ErrorCode::ERR_EMAIL_ADDRESS_ALREADY_EXISTS);
        }

        //create new usersRec and add to db
        SantiagoDBTables::UsersRec usersRec;
        usersRec._userName = userName_;
        usersRec._emailAddress = emailAddress_;
        usersRec._password = generateSHA256(password_);
        _databaseConnection.get().addUsersRec(usersRec,error);
        if(error)
        {
            return error;
        }

        ST_LOG_INFO("Create user successfull for userName:"<<userName_<<std::endl);
        return std::error_code(ErrorCode::ERR_SUCCESS);
    }

    template<typename ControllerTypes>
    std::pair<std::error_code,boost::optional<std::pair<UserInfo,std::string> > > ControllerBase<ControllerTypes>::
    loginUser(const ClientRequestData& requestData_,
              const std::string& userNameOrEmailAddress_,
              bool isUserNameNotEmailAddress_,
              const std::string& password_,
              boost::asio::yield_context yield_)
    {
        //verify username-password
        boost::optional<SantiagoDBTables::UsersRec> usersRecOpt;
        std::error_code error;

        if(isUserNameNotEmailAddress_)
        {
            std::tie(error,usersRecOpt) =
                verifyUserNamePasswordAndGetUsersRec(userNameOrEmailAddress_,
                                                     password_);
        }
        else
        {
            std::tie(error,usersRecOpt) =
                verifyEmailAddressPasswordAndGetUsersRec(userNameOrEmailAddress_,
                                                         password_);
        }

        if(error)
        {
            return std::pair<std::error_code,boost::optional<std::pair<UserInfo,std::string> > >(error,boost::none);
        }
        ST_ASSERT(usersRecOpt);

        return createSessionsDataForUserImpl(requestData_,*usersRecOpt);
    }    

    template<typename ControllerTypes>
    std::pair<std::error_code,boost::optional<std::pair<UserInfo,std::string> > > ControllerBase<ControllerTypes>::
    loginUserForVerifiedOICToken(const ClientRequestData& requestData_,
                                 const std::string& emailAddress_,
                                 boost::asio::yield_context yield_)
    {
        //verify username-password
        std::error_code error;
        boost::optional<SantiagoDBTables::UsersRec> usersRecOpt = 
            _databaseConnection.get().getUsersRecForEmailAddress(emailAddress_,error);
        if(error && (ErrorCode::ERR_DATABASE_GET_RETURNED_ZERO_RESULTS != error.value()))
        {
            return std::make_pair(error,boost::none);
        }
        else if(!usersRecOpt)
        {
            ST_LOG_INFO("Unregisted user logging in using OIC:"<<emailAddress_<<std::endl);
            return std::make_pair(std::error_code(ErrorCode::ERR_EMAIL_ADDRESS_NOT_REGISTERED),
                                  boost::none);
        }
        
        ST_ASSERT(!error && usersRecOpt);
        return createSessionsDataForUserImpl(requestData_,*usersRecOpt);
    }

    template<typename ControllerTypes>
    std::pair<std::error_code,boost::optional<std::pair<UserInfo,std::string> > > ControllerBase<ControllerTypes>::
    createSessionsDataForUserImpl(const ClientRequestData& requestData_,
                                  const SantiagoDBTables::UsersRec& usersRec_)
    {
        if(_localData.isUserBeingLoggedOut(usersRec_._userName))
        {
            ST_LOG_INFO("User in being logged out state. userName:"<<usersRec_._userName<<std::endl);
            return std::pair<std::error_code,boost::optional<std::pair<UserInfo,std::string> > >(
                std::error_code(ErrorCode::ERR_USER_BEING_LOGGED_OUT),
                boost::none);
        }

        //create new session record and add to db
        SantiagoDBTables::SessionsRec sessionsRec;
        sessionsRec._userName = usersRec_._userName;
        sessionsRec._loginTime = boost::posix_time::second_clock::universal_time();
        sessionsRec._lastActiveTime = sessionsRec._loginTime;
        
        std::error_code error(ErrorCode::ERR_SUCCESS);
        for(unsigned i=0;i<5;i++)
        { //5 attempts with different cookie strings
            sessionsRec._cookieString = generateUniqueCookie(sessionsRec._userName);
            _databaseConnection.get().addSessionsRec(sessionsRec,error);
            if(!error)
            {
                break;
            }
        }

        if(error)
        {
            return std::pair<std::error_code,boost::optional<std::pair<UserInfo,std::string> > >(error,boost::none);
        }
     
        _localData.addCookie(usersRec_._userName,usersRec_._emailAddress,sessionsRec,requestData_.getClientId());
        
        return std::pair<std::error_code,boost::optional<std::pair<UserInfo,std::string> > >(
            std::error_code(ErrorCode::ERR_SUCCESS),
            std::make_pair(UserInfo(usersRec_._userName,
                                    usersRec_._emailAddress),
                           sessionsRec._cookieString));
    }

    template<typename ControllerTypes>
    std::pair<std::error_code,boost::optional<UserInfo> > ControllerBase<ControllerTypes>::
    verifyCookieAndGetUserInfo(const ClientRequestData& requestData_,
                               const std::string& cookieString_,
                               boost::asio::yield_context yield_)
    {
        std::error_code error;
        boost::optional<SantiagoDBTables::SessionsRec> sessionsRecOpt;
        std::tie(error,sessionsRecOpt) = partiallyVerifyCookieAndGetSessionsRec(cookieString_);        
        if(error)
        {
            ST_LOG_INFO(error.message() <<" cookieString:" <<cookieString_<<std::endl);
            return std::pair<std::error_code,boost::optional<UserInfo> >(error,boost::none);
        }
        ST_ASSERT(sessionsRecOpt);
        
        //check if the lastActiveTime older than MAX_SESSION_DURATION. If yes then logout
        if((boost::posix_time::second_clock::universal_time() - sessionsRecOpt->_lastActiveTime) >
           getMaxSessionInactiveDuration())
        {

            ST_LOG_INFO("Session lastActiveTime older than MAX_SESSION_DURATION. Going to log out. cookieString:"
                     <<cookieString_<<std::endl);            
            cleanupCookieDataAndUpdateSessionRecord(requestData_,cookieString_,yield_);

            return std::pair<std::error_code,boost::optional<UserInfo> >(
                std::error_code(ErrorCode::ERR_INVALID_SESSION_COOKIE),
                boost::none);
        }
        else //set the lastActiveTime to now
        {
            sessionsRecOpt->_lastActiveTime = boost::posix_time::second_clock::universal_time();
//            std::error_code error; //calling db for every verify is very inefficient. So commenting out
//            _databaseConnection.get().updateSessionsRec(*cookieStringSessionsRecMapIter->second,error_);
        }
        ST_LOG_INFO("Verify cookie successfull. cookieString:"<<cookieString_<<std::endl);

        boost::optional<std::string> emailAddressOpt = _localData.getUserEmailAddress(sessionsRecOpt->_userName);
        ST_ASSERT(emailAddressOpt);

        _localData.updateCookieAndAddClient(*sessionsRecOpt,requestData_.getClientId());

        return std::pair<std::error_code,boost::optional<UserInfo> >(
            std::error_code(ErrorCode::ERR_SUCCESS),
            UserInfo(sessionsRecOpt->_userName,
                     *emailAddressOpt));
    }

    template<typename ControllerTypes>
    std::error_code ControllerBase<ControllerTypes>::logoutUserForCookie(const ClientRequestData& requestData_,
                                                                        const std::string& cookieString_,
                                                                        boost::asio::yield_context yield_)
    {
        std::error_code error;
        boost::optional<SantiagoDBTables::SessionsRec> sessionsRecOpt;
        std::tie(error,sessionsRecOpt) = partiallyVerifyCookieAndGetSessionsRec(cookieString_);        
        if(error)
        {
            ST_LOG_INFO(error.message() <<" cookieString:" <<cookieString_<<std::endl);
            return error;
        }
        
        return cleanupCookieDataAndUpdateSessionRecord(requestData_,cookieString_,yield_);
    }

    template<typename ControllerTypes>
    std::error_code ControllerBase<ControllerTypes>::logoutUserForAllCookies(const ClientRequestData& requestData_,
                                                                            const std::string& userName_,
                                                                            boost::asio::yield_context yield_)
    {
        //verify if such a user is already logged in
        if(!_localData.getUserEmailAddress(userName_))
        {
            ST_LOG_INFO("No active session for username. username:"<<userName_);
            return std::error_code(ErrorCode::ERR_NO_ACTIVE_SESSION_FOR_USERNAME);
        }
        
        if(_localData.isUserBeingLoggedOut(userName_))
        {
            ST_LOG_INFO("User in being logged out state. userName:"<<userName_<<std::endl);
            return std::error_code(ErrorCode::ERR_USER_BEING_LOGGED_OUT);
        }
        
        return cleanupCookieDataAndUpdateSessionRecordsForAllCookies(requestData_,userName_,yield_);
    }

    template<typename ControllerTypes>
    std::error_code ControllerBase<ControllerTypes>::changeUserPassword(
        const ClientRequestData& requestData_,
        const std::string& cookieString_,
        const std::string& oldPassword_,
        const std::string& newPassword_,
        boost::asio::yield_context yield_)
    {
        std::error_code error;
        boost::optional<SantiagoDBTables::SessionsRec> sessionsRecOpt;
        std::tie(error,sessionsRecOpt) = partiallyVerifyCookieAndGetSessionsRec(cookieString_);        
        if(error)
        {
            ST_LOG_INFO(error.message() <<" cookieString:" <<cookieString_<<std::endl);
            return error;
        }
        ST_ASSERT(sessionsRecOpt);
        
        //verify username-password
        boost::optional<SantiagoDBTables::UsersRec> usersRecOpt;
        std::tie(error,usersRecOpt) =
            verifyUserNamePasswordAndGetUsersRec(sessionsRecOpt->_userName, oldPassword_);
        if(error)
        {
            return error;
        }
        ST_ASSERT(usersRecOpt);

        //change and update the password
        usersRecOpt->_password = generateSHA256(newPassword_);
        _databaseConnection.get().updateUsersRec(*usersRecOpt,error);
        //whether succeed or db error...it will be passed to the onChangePasswordCallbackFn
        return error;
    }

    template<typename ControllerTypes>
    std::pair<std::error_code,boost::optional<std::string> > ControllerBase<ControllerTypes>::
    createAndReturnRecoveryString(const ClientRequestData& requestData_,
                                  const std::string& emailAddress_,
                                  boost::asio::yield_context yield_)
    {
        boost::optional<SantiagoDBTables::UsersRec> usersRecOpt;
        std::error_code error;
        usersRecOpt = _databaseConnection.get().getUsersRecForEmailAddress(emailAddress_,error);
        if(error && (ErrorCode::ERR_DATABASE_GET_RETURNED_ZERO_RESULTS != error.value()))
        {
            return std::pair<std::error_code,boost::optional<std::string> >(error,boost::none);
        }
        if(ErrorCode::ERR_DATABASE_GET_RETURNED_ZERO_RESULTS == error.value())
        {
            return std::pair<std::error_code,boost::optional<std::string> >(
                std::error_code(ErrorCode::ERR_EMAIL_ADDRESS_NOT_REGISTERED),
                boost::none);
        }
        ST_ASSERT(usersRecOpt);
        
        SantiagoDBTables::UsersRec newUsersRec = *usersRecOpt;
        newUsersRec._recoveryString = generateRecoveryString();
        newUsersRec._recoveryStringCreateTime = boost::posix_time::second_clock::universal_time();
        _databaseConnection.get().updateUsersRec(newUsersRec,error);
        if(error)
        {
            return std::pair<std::error_code,boost::optional<std::string> >(error,boost::none);
        }
        
        ST_LOG_INFO("Created recovery string successfully for emailAddress:"<<emailAddress_<<std::endl);
        return std::pair<std::error_code,boost::optional<std::string> >(
            std::error_code(ErrorCode::ERR_SUCCESS),
            newUsersRec._recoveryString);
    }

    template<typename ControllerTypes>
    std::pair<std::error_code,boost::optional<std::string> > ControllerBase<ControllerTypes>::
    getUserForEmailAddressAndRecoveryString(const ClientRequestData& requestData_,
                                            const std::string& emailAddress_,
                                            const std::string& recoveryString_,
                                            boost::asio::yield_context yield_)
    {
        boost::optional<SantiagoDBTables::UsersRec> usersRecOpt;
        std::error_code error;
        usersRecOpt = _databaseConnection.get().getUsersRecForEmailAddress(emailAddress_,error);
        if(error && (ErrorCode::ERR_DATABASE_GET_RETURNED_ZERO_RESULTS != error.value()))
        {
            return std::pair<std::error_code,boost::optional<std::string> >(error,boost::none);
        }
        if(ErrorCode::ERR_DATABASE_GET_RETURNED_ZERO_RESULTS == error.value())
        {
            return std::pair<std::error_code,boost::optional<std::string> >(
                std::error_code(ErrorCode::ERR_EMAIL_ADDRESS_NOT_REGISTERED),
                boost::none);
        }
        ST_ASSERT(usersRecOpt);

        if(usersRecOpt->_recoveryString != recoveryString_)
        {
            return std::pair<std::error_code,boost::optional<std::string> >(
                std::error_code(ErrorCode::ERR_INVALID_EMAIL_ADDRESS_RECOVERY_STRING),
                boost::none);
        }

        return std::pair<std::error_code,boost::optional<std::string> >(
            std::error_code(ErrorCode::ERR_SUCCESS),
            usersRecOpt->_userName);
    }

    template<typename ControllerTypes>
    std::error_code ControllerBase<ControllerTypes>::
    changeUserPasswordForEmailAddressAndRecoveryString(const ClientRequestData& requestData_,
                                                       const std::string& emailAddress_,
                                                       const std::string& recoveryString_,
                                                       const std::string& newPassword_,
                                                       boost::asio::yield_context yield_)
    {
        boost::optional<SantiagoDBTables::UsersRec> usersRecOpt;
        std::error_code error;
        std::tie(error,usersRecOpt) =
            verifyEmailAddressRecoveryStringAndGetUsersRec(emailAddress_,recoveryString_);
        if(error)
        {
            return error;
        }
        ST_ASSERT(usersRecOpt);
            
        //change and update the password
        usersRecOpt->_password = generateSHA256(newPassword_);
        usersRecOpt->_recoveryString = boost::none;
        usersRecOpt->_recoveryStringCreateTime = boost::none;
        _databaseConnection.get().updateUsersRec(*usersRecOpt,error);
        //whether succeed or db error...it will be passed to the onChangePasswordCallbackFn
        return error;
    }

    template<typename ControllerTypes>
    std::error_code ControllerBase<ControllerTypes>::changeUserEmailAddress(
        const ClientRequestData& requestData_,
        const std::string& cookieString_,
        const std::string& newEmailAddress_,
        const std::string& password_,
        boost::asio::yield_context yield_)
    {
        std::error_code error;
        boost::optional<SantiagoDBTables::SessionsRec> sessionsRecOpt;
        std::tie(error,sessionsRecOpt) = partiallyVerifyCookieAndGetSessionsRec(cookieString_);        
        if(error)
        {
            ST_LOG_INFO(error.message() <<" cookieString:" <<cookieString_<<std::endl);
            return error;
        }
        ST_ASSERT(sessionsRecOpt);
        
        //verify password
        boost::optional<SantiagoDBTables::UsersRec> usersRecOpt;
        std::tie(error,usersRecOpt) =
            verifyUserNamePasswordAndGetUsersRec(sessionsRecOpt->_userName, password_);
        
        if(error)
        {
            //onChangeEmailAddressCallbackFn_(error);
            return error;
        }
        ST_ASSERT(usersRecOpt);

        SantiagoDBTables::UsersRec newUsersRec = *usersRecOpt;

        //verify no other existing user with same email address
        usersRecOpt = _databaseConnection.get().getUsersRecForEmailAddress(newEmailAddress_,error);
        if(error && (ErrorCode::ERR_DATABASE_GET_RETURNED_ZERO_RESULTS != error.value()))
        {
            return error;
        }        
        if(usersRecOpt)
        {
            return std::error_code(ErrorCode::ERR_EMAIL_ADDRESS_ALREADY_EXISTS);
        }
        ST_ASSERT(ErrorCode::ERR_DATABASE_GET_RETURNED_ZERO_RESULTS == error.value());

        //change and update the password
        newUsersRec._emailAddress = newEmailAddress_;
        _databaseConnection.get().updateUsersRec(newUsersRec,error);
        //whether succeed or db error...it will be passed to the onChangePasswordCallbackFn
        return error;

    }

    template<typename ControllerTypes>
    std::error_code ControllerBase<ControllerTypes>::deleteUser(const ClientRequestData& requestData_,
                                                               const std::string& cookieString_,
                                                               boost::asio::yield_context yield_)
    {
        std::error_code error;
        boost::optional<SantiagoDBTables::SessionsRec> sessionsRecOpt;
        std::tie(error,sessionsRecOpt) = partiallyVerifyCookieAndGetSessionsRec(cookieString_);        
        if(error)
        {
            ST_LOG_INFO(error.message() <<" cookieString:" <<cookieString_<<std::endl);
            return error;
        }
        ST_ASSERT(sessionsRecOpt);

        //delete from db
        boost::optional<SantiagoDBTables::UsersRec> usersRecOpt;
        std::tie(error,usersRecOpt) =
            verifyUserNamePasswordAndGetUsersRec(sessionsRecOpt->_userName,"");
        if((ErrorCode::ERR_DATABASE_EXCEPTION == error.value()) ||
           (ErrorCode::ERR_DATABASE_INVALID_USER_INPUT == error.value()))
        {
            return error;
        }

        ST_ASSERT(usersRecOpt);
        _databaseConnection.get().deleteUsersRec(usersRecOpt->_userName,error);
        if(error)
        {
            return error;
        }

        //remove from memory
        return cleanupCookieDataAndUpdateSessionRecordsForAllCookies(requestData_,
                                                                     sessionsRecOpt->_userName,
                                                                     yield_);
    }

    template<typename ControllerTypes>
    std::pair<std::error_code,boost::optional<SantiagoDBTables::UsersRec> > ControllerBase<ControllerTypes>::
    verifyUserNamePasswordAndGetUsersRec(const std::string& userName_, const std::string& password_)
    {
        //get the UsersRec from db
        std::error_code error;
        boost::optional<SantiagoDBTables::UsersRec> usersRecOpt = 
            _databaseConnection.get().getUsersRecForUserName(userName_,error);
        if(error && (ErrorCode::ERR_DATABASE_GET_RETURNED_ZERO_RESULTS != error.value()))
        {
            return std::make_pair(error,usersRecOpt);
        }

        //check if the username/password matches
        if(!usersRecOpt || (usersRecOpt->_password != generateSHA256(password_)))
        {
            ST_LOG_INFO("Wrong username_password. userName:"<<userName_<<std::endl);
            return std::make_pair(std::error_code(ErrorCode::ERR_INVALID_USERNAME_PASSWORD),
                                  usersRecOpt);
        }
        ST_ASSERT(!error);

        ST_LOG_INFO("Username password verified for userName:"<<userName_<<std::endl);
        return std::make_pair(std::error_code(ErrorCode::ERR_SUCCESS),usersRecOpt);
    }

    template<typename ControllerTypes>
    std::pair<std::error_code,boost::optional<SantiagoDBTables::UsersRec> > ControllerBase<ControllerTypes>::
    verifyEmailAddressPasswordAndGetUsersRec(const std::string& emailAddress_,
                                             const std::string& password_)
    {
        //get the UsersRec from db
        std::error_code error;
        boost::optional<SantiagoDBTables::UsersRec> usersRecOpt = 
            _databaseConnection.get().getUsersRecForEmailAddress(emailAddress_,error);
        if(error && (ErrorCode::ERR_DATABASE_GET_RETURNED_ZERO_RESULTS != error.value()))
        {
            return std::make_pair(error,usersRecOpt);
        }
        
        //check if the username/password matches
        if(!usersRecOpt || (usersRecOpt->_password != generateSHA256(password_)))
        {
            ST_LOG_INFO("Wrong emailaddress_password. emailAddress:"<<emailAddress_<<std::endl);
            return std::make_pair(std::error_code(ErrorCode::ERR_INVALID_EMAIL_ADDRESS_PASSWORD),
                                  usersRecOpt);
        }
        ST_ASSERT(!error);
        
        ST_LOG_INFO("EmailAddress password verified for emailAddress:"<<emailAddress_<<std::endl);
        return std::make_pair(std::error_code(ErrorCode::ERR_SUCCESS),usersRecOpt);
    }

    template<typename ControllerTypes>
    std::pair<std::error_code,boost::optional<SantiagoDBTables::UsersRec> > 
    ControllerBase<ControllerTypes>::verifyEmailAddressRecoveryStringAndGetUsersRec(const std::string& emailAddress_,
                                                                                 const std::string& recoveryString_)
    {
        //get the UsersRec from db
        std::error_code error;
        boost::optional<SantiagoDBTables::UsersRec> usersRecOpt = 
            _databaseConnection.get().getUsersRecForEmailAddress(emailAddress_,error);
        if(error && (ErrorCode::ERR_DATABASE_GET_RETURNED_ZERO_RESULTS != error.value()))
        {
            return std::make_pair(error,usersRecOpt);
        }

        //check if the username/password matches
        if(!usersRecOpt || (usersRecOpt->_recoveryString != recoveryString_))
        {
            ST_LOG_INFO("Wrong emailaddress_recoverystring. emailAddress:"<<emailAddress_<<std::endl);
            return std::make_pair(std::error_code(ErrorCode::ERR_INVALID_EMAIL_ADDRESS_RECOVERY_STRING),
                                  usersRecOpt);
        }
        ST_ASSERT(!error);

        ST_LOG_INFO("EmailAddress recovery string verified for emailAddress:"<<emailAddress_<<std::endl);
        return std::make_pair(std::error_code(ErrorCode::ERR_SUCCESS),usersRecOpt);
    }

    template<typename ControllerTypes>
    std::pair<std::error_code,boost::optional<SantiagoDBTables::SessionsRec> >
    ControllerBase<ControllerTypes>::partiallyVerifyCookieAndGetSessionsRec(const std::string& cookieString_)
    {
        boost::optional<SantiagoDBTables::SessionsRec> sessionsRecOpt = _localData.getCookieSessionsRec(cookieString_);
        if(!sessionsRecOpt)
        {
            return std::pair<std::error_code,boost::optional<SantiagoDBTables::SessionsRec> >(
                std::error_code(ErrorCode::ERR_INVALID_SESSION_COOKIE),
                boost::none);
        }
        else if(_localData.isCookieBeingLoggedOut(cookieString_))
        {
            return std::pair<std::error_code,boost::optional<SantiagoDBTables::SessionsRec> >(
                std::error_code(ErrorCode::ERR_COOKIE_BEING_LOGGED_OUT),
                sessionsRecOpt);
        }
        else if(_localData.isUserBeingLoggedOut(sessionsRecOpt->_userName))
        {
            return std::pair<std::error_code,boost::optional<SantiagoDBTables::SessionsRec> >(
                std::error_code(ErrorCode::ERR_USER_BEING_LOGGED_OUT),
                sessionsRecOpt);
        }
        else
        {
            return std::pair<std::error_code,boost::optional<SantiagoDBTables::SessionsRec> >(
                std::error_code(ErrorCode::ERR_SUCCESS),
                sessionsRecOpt);
        }
    }

    template<typename ControllerTypes>
    std::error_code ControllerBase<ControllerTypes>::
    cleanupLocalCookieDataAndUpdateSessionsRecordImpl(SantiagoDBTables::SessionsRec& sessionsRec_)
    {
        ST_ASSERT(_localData.isCookieBeingLoggedOut(sessionsRec_._cookieString));
        
        //update the db 
        sessionsRec_._logoutTime = boost::posix_time::second_clock::universal_time();
        std::error_code error;
        SantiagoDBTables::SessionsRec sessionsRec = sessionsRec_;
        _databaseConnection.get().updateSessionsRec(sessionsRec,error);
        if(error)
        {
            ST_LOG_INFO("updateSessionsRec failed. Logging out without writing to db. CookieString:"
                        <<sessionsRec._cookieString);
            _localData.unsetCookieBeingLoggedOutFlag(sessionsRec._cookieString);
        }

        //remove from _userNameUserDataMap
        _localData.removeCookie(sessionsRec._cookieString);
        return std::error_code(ErrorCode::ERR_SUCCESS);
    }

    template<typename ControllerTypes>
    std::error_code ControllerBase<ControllerTypes>::
    cleanupCookieDataAndUpdateSessionRecord(const ClientRequestData& requestData_,
                                            const std::string& cookieString_,
                                            boost::asio::yield_context yield_)
    {
        //internal fn. so when this fn is called the cookie should already be verified.
        boost::optional<SantiagoDBTables::SessionsRec> sessionsRecOpt = _localData.getCookieSessionsRec(cookieString_);
        ST_ASSERT(sessionsRecOpt);

        //logout from all clients
        std::error_code error;
        _localData.setCookieBeingLoggedOutFlag(cookieString_);
        error = logoutCookieFromAllClients(requestData_,cookieString_,yield_);
        if(error)
        {
            _localData.unsetCookieBeingLoggedOutFlag(cookieString_);
            ST_LOG_INFO("logoutCookieFromAllClients failed. CookieString:" << cookieString_ <<std::endl);
            return error;
        }

        return cleanupLocalCookieDataAndUpdateSessionsRecordImpl(*sessionsRecOpt);
    }

    template<typename ControllerTypes>
    std::error_code ControllerBase<ControllerTypes>::
    cleanupCookieDataAndUpdateSessionRecordsForAllCookies(const ClientRequestData& requestData_,
                                                          const std::string& userName_,
                                                          boost::asio::yield_context yield_)
    {
        //internal fn. the username should exist.
        ST_ASSERT(_localData.getUserEmailAddress(userName_));
        
        //logout from all clients
        _localData.setUserBeingLoggedOutFlag(userName_);
        std::error_code error;
        error = logoutUserFromAllClients(requestData_,userName_,yield_);
        if(error)
        {
            ST_LOG_INFO("logoutUserFromAllClients failed. userName:" << userName_ <<std::endl);
            _localData.unsetUserBeingLoggedOutFlag(userName_);
            return error;
        }

        std::vector<std::string> userCookieList = _localData.getAllCookieStringsForUser(userName_);
        _databaseConnection.get().updateSessionLogoutTimes(userCookieList,
                                                           boost::posix_time::second_clock::universal_time(),
                                                           error);
        
        if(error)
        {
            ST_LOG_INFO("updateSessionsRecsWithCurrentLogoutTime failed. userName:" << userName_);
            _localData.unsetUserBeingLoggedOutFlag(userName_);
            return error;
        }
        _localData.removeUser(userName_);

        //the user should also be removed by now
        ST_ASSERT(!_localData.getUserEmailAddress(userName_));
        return std::error_code(ErrorCode::ERR_SUCCESS);
    }

    template<typename ControllerTypes>
    std::string ControllerBase<ControllerTypes>::generateSHA256(const std::string str)
    {
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, str.c_str(), str.size());
        SHA256_Final(hash, &sha256);
        std::stringstream ss;
        for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        {
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
        }
        return ss.str();
    }

    template<typename ControllerTypes>
    std::string ControllerBase<ControllerTypes>::generateUniqueCookie(std::string userName_)
    {
        std::string str;
        str += userName_[0];
        static const char alphanum[] =
            "0123456789"
            "@#$%^*"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
        
        int stringLength = sizeof(alphanum) - 1;
	
        //return alphanum[rand() % stringLength];
	for(unsigned int i = 0; i < 45; ++i)
	{
            str += alphanum[rand() % stringLength];
	}
        return str;
    }

    template<typename ControllerTypes>
    std::string ControllerBase<ControllerTypes>::generateRecoveryString()
    {
        std::string str;
        static const char alphanum[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
                    
        int stringLength = sizeof(alphanum) - 1;
	
        //return alphanum[rand() % stringLength];
	for(unsigned int i = 0; i < 8; ++i)
	{
            str += alphanum[rand() % stringLength];
	}
        return str;
    }

    template<typename ControllerTypes>
    boost::posix_time::time_duration ControllerBase<ControllerTypes>::getMaxSessionInactiveDuration() const
    {
        return boost::posix_time::time_duration(MAX_SESSION_DURATION,0,0,0);   
    }
    
}}  

#endif
