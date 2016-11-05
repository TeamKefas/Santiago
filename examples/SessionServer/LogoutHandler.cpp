#include "LogoutHandler.h"

namespace SessionServer
{
    void LogoutHandler::handleVerifiedRequest(const RequestPtr& request_,
                                             const std::string& userName_,
                                             const std::string& emailAddress_,
                                             const std::string& cookieString_)
    {
        request_->setContentMIMEType(Santiago::MIMEType::TEXT);
        request_->out()<<"Logging out "<<userName_<<"..."<<std::endl;

        _userController.logoutUserForCookie(
            cookieString_,
            std::bind(&LogoutHandler::handleLogoutUser,
                      this->sharedFromThis<LogoutHandler>(),
                      request_,
                      std::placeholders::_1));
    }

    void LogoutHandler::handleNonVerifiedRequest(const RequestPtr& request_)
    {
        request_->setContentMIMEType(Santiago::MIMEType::TEXT);
        request_->out()<<"User not logged in. Please login(login.fcgi) before trying again."<<std::endl;
        
        printEcho(request_);
        request_->setAppStatus(0);
        std::error_code error;
        request_->commit(error);        
    }

    void LogoutHandler::handleLogoutUser(const RequestPtr& request_,
                                         std::error_code error_)
    {
        if(error_)
        {
            request_->out()<<"Logging out failed. \n";
            request_->out()<<error_.message()<<std::endl;
            printEcho(request_);
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
            printEcho(request_);
            request_->setAppStatus(0);
            std::error_code error;
            request_->commit(error);
        }
    }
}
