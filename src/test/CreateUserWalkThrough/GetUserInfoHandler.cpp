#include "GetUserInfoHandler.h"

namespace Test{ namespace AppServer
{
    void GetUserInfoHandler::handleVerifiedRequest(const RequestPtr& request_,
                                             const std::string& userName_,
                                             const std::string& emailAddress_,
                                             const std::string& cookieString_)
    {
        request_->setContentMIMEType(Santiago::MIMEType::TEXT);
        _userController.verifyCookieAndGetUserInfo(
            cookieString_,
            std::bind(&GetUserInfoHandler::handleGetUserInfo,
                      this->sharedFromThis<GetUserInfoHandler>(),
                      request_,
                      std::placeholders::_1,
                      std::placeholders::_2));
    }

    void GetUserInfoHandler::handleNonVerifiedRequest(const RequestPtr& request_)
    {
        request_->setContentMIMEType(Santiago::MIMEType::TEXT);
        request_->out()<<"User not logged in. Please login(login.fcgi) before trying again."<<std::endl;
        request_->setAppStatus(0);
        std::error_code error;
        request_->commit(error);        
    }

    void GetUserInfoHandler::handleGetUserInfo(const RequestPtr& request_,
                                               std::error_code error_,
                                               const boost::optional<Santiago::Authentication::UserInfo> &userInfo_)
    {
        if(error_)
        {
            request_->out()<<"Failed to fetch user info. \n";
            request_->out()<<error_.message()<<std::endl;
            request_->setAppStatus(0);
            std::error_code error;
            request_->commit(error);
        }
        else
        {
            request_->out() << "Username : "  << userInfo_->_userName << std::endl;
            request_->out() << "Email addresss : "  << userInfo_->_emailAddress << std::endl;
            request_->out()<<"Info fetched successfully. \n";
            request_->setAppStatus(0);
            std::error_code error;
            request_->commit(error);
        }
    }
    }}
