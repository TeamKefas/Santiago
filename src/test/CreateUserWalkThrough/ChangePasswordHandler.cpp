#include "ChangePasswordHandler.h"

namespace Test{ namespace AppServer
{
    void ChangePasswordHandler::handleVerifiedRequest(const RequestPtr& request_,
                                                 boost::asio::yield_context yield_,
                                                 const std::string& userName_,
                                                 const std::string& oldPassword_,
                                                 const std::string& newPassword_)
    {
        request_->setContentMIMEType(Santiago::MIMEType::TEXT);

        _userController.changeUserPassword(
            userName_,
            oldPassword_,
            newPassword_,
            std::bind(&ChangePasswordHandler::handleChangePassword,
                      this->sharedFromThis<ChangePasswordHandler>(),
                      request_,
                      std::placeholders::_1));
    }

    void ChangePasswordHandler::handleNonVerifiedRequest(const RequestPtr& request_,
                                                    boost::asio::yield_context yield_)
    {
         request_->setContentMIMEType(Santiago::MIMEType::TEXT);
        request_->out()<<"User not logged in. Please login(login.fcgi) before trying again."<<std::endl;
        request_->setAppStatus(0);
        std::error_code error;
        request_->commit(error);        
    }

     void ChangePasswordHandler::handleChangePassword(const RequestPtr& request_,
                                                      std::error_code error_)
    {
        if(error_)
        {
            request_->out()<<"Change password failed. \n";
            request_->out()<<error_.message()<<std::endl;
            request_->setAppStatus(0);
            std::error_code error;
            request_->commit(error);
        }
        else
        {
            request_->out()<<"Change password successful. \n";
            request_->out()<<error_.message()<<std::endl;
            request_->setAppStatus(0);
            std::error_code error;
            request_->commit(error);
        }
    }
    
    }}
