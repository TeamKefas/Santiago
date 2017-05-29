#include "DeleteUserHandler.h"

namespace Test{ namespace AppServer
{
    void DeleteUserHandler::handleVerifiedRequest(const RequestPtr& request_,
                                                  boost::asio::yield_context yield_,
                                                  const std::string& userName_,
                                                  const std::string& emailAddress_,
                                                  const std::string& cookieString_)
    {
        request_->setContentMIMEType(Santiago::MIMEType::TEXT);
        request_->out()<<"Deleting "<<userName_<<"..."<<std::endl;

        _userController.deleteUser(
            cookieString_,
            std::bind(&DeleteUserHandler::handleDeleteUser,
                      this->sharedFromThis<DeleteUserHandler>(),
                      request_,
                      std::placeholders::_1));
    }

    void DeleteUserHandler::handleNonVerifiedRequest(const RequestPtr& request_,
                                                     boost::asio::yield_context yield_)
    {
        request_->setContentMIMEType(Santiago::MIMEType::TEXT);
        request_->out()<<"User not logged in. Please login(login.fcgi) before trying again."<<std::endl;
        request_->setAppStatus(0);
        std::error_code error;
        request_->commit(error);        
    }

    void DeleteUserHandler::handleDeleteUser(const RequestPtr& request_,
                                             std::error_code error_)
    {
        if(error_)
        {
            request_->out()<<"Delete Operation failed. \n";
            request_->out()<<error_.message()<<std::endl;
            request_->setAppStatus(0);
            std::error_code error;
            request_->commit(error);
        }
        else
        {
            request_->out()<<"Delete operation successfull. \n";
            request_->setAppStatus(0);
            std::error_code error;
            request_->commit(error);
        }
    }
    }}
