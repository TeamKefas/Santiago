#include "AsyncGetUserInfoHandler.h"

namespace Test{ namespace AppServer
{
    void AsyncGetUserInfoHandler::handleVerifiedRequest(const RequestPtr& request_,
                                                        boost::asio::yield_context yield_,
                                                        const std::string& userName_,
                                                        const std::string& emailAddress_,
                                                        const std::string& cookieString_)
    {
        std::error_code error1;
        request_->setContentMIMEType(Santiago::MIMEType::TEXT);
        const boost::optional<Santiago::Authentication::UserInfo> &userInfo_ = _userController.verifyCookieAndGetUserInfo(
            cookieString_,
            yield_,
            error1);

        if(error1)
        {
            request_->out()<<"Failed to fetch user info. \n";
            request_->out()<<error1.message()<<std::endl;
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

    void AsyncGetUserInfoHandler::handleNonVerifiedRequest(const RequestPtr& request_,
                                                           boost::asio::yield_context yield_)
    {
        request_->setContentMIMEType(Santiago::MIMEType::TEXT);
        request_->out()<<"User not logged in. Please login(login.fcgi) before trying again."<<std::endl;
        request_->setAppStatus(0);
        std::error_code error;
        request_->commit(error);        
    }

    }}
