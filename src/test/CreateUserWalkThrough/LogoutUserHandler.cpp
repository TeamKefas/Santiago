#include "LogoutUserHandler.h"

namespace Test{ namespace AppServer
{
    void LogoutUserHandler::handleVerifiedRequest(const RequestPtr& request_,
                                                  boost::asio::yield_context yield_,
                                                  const std::string& userName_,
                                                  const std::string& emailAddress_,
                                                  const std::string& cookieString_)
    {
        request_->setContentMIMEType(Santiago::MIMEType::TEXT);
        request_->out()<<"Logging out "<<userName_<<"..."<<std::endl;

        _userController.logoutUserForCookie(
            cookieString_,
            std::bind(&LogoutUserHandler::handleLogoutUser,
                      this->sharedFromThis<LogoutUserHandler>(),
                      request_,
                      std::placeholders::_1));
    }

    void LogoutUserHandler::handleNonVerifiedRequest(const RequestPtr& request_,
                                                     boost::asio::yield_context yield_)
    {
        request_->setContentMIMEType(Santiago::MIMEType::TEXT);
        request_->out()<<"User not logged in. Please login(login.fcgi) before trying again."<<std::endl;
        request_->setAppStatus(0);
        std::error_code error;
        request_->commit(error);        
    }

    void LogoutUserHandler::handleLogoutUser(const RequestPtr& request_,
                                             std::error_code error_)
    {
        if(error_)
        {
            request_->out()<<"Logging out failed. \n";
            request_->out()<<error_.message()<<std::endl;
            request_->setAppStatus(0);
            std::error_code error;
            request_->commit(error);
        }
        else
        {
            Santiago::HTTPCookieData cookieData;
            cookieData._name = "SID";
            cookieData._value = "LOGGED OUT";
            cookieData._expiryTime = boost::posix_time::second_clock::universal_time() - boost::posix_time::seconds(1);
            bool flag = request_->responseHTTPCookies().insert(cookieData).second;
            BOOST_ASSERT(flag);

            request_->out()<<"User logout successfull. \n";
            request_->setAppStatus(0);
            std::error_code error;
            request_->commit(error);
        }
    }
    }}
