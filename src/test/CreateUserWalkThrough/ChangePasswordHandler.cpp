#include "ChangePasswordHandler.h"

namespace Test{ namespace AppServer
{
    void ChangePasswordHandler::handleVerifiedRequest(const RequestPtr& request_,
                                                      boost::asio::yield_context yield_,
                                                      const std::string& userName_,
                                                      const std::string& emailAddress_,
                                                      const std::string& cookieString_)
    {
        request_->setContentMIMEType(Santiago::MIMEType::TEXT);

        std::map<std::string, std::string>::const_iterator oldPasswordIter = request_->getPostData().find("old_password");
        std::map<std::string,std::string>::const_iterator newPasswordIter =  request_->getPostData().find("new_password");
        if((request_->getPostData().end() == oldPasswordIter && request_->getPostData().end() == newPasswordIter))
        {
            request_->out()<<"old_password/new_password not send in the post data."<<std::endl;
            request_->setAppStatus(0);
            std::error_code error;
            request_->commit(error);        
        }
        else
        {
            _userController.changeUserPassword(
                userName_,
                oldPasswordIter->second,
                newPasswordIter->second,
                std::bind(&ChangePasswordHandler::handleChangePassword,
                          this->sharedFromThis<ChangePasswordHandler>(),
                          request_,
                          std::placeholders::_1));
        }
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
            request_->setAppStatus(0);
            std::error_code error;
            request_->commit(error);
        }
    }
    
    }}
