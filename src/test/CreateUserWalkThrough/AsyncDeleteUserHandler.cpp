#include "AsyncDeleteUserHandler.h"

namespace Test{ namespace AppServer
{
    void AsyncDeleteUserHandler::handleVerifiedRequest(const RequestPtr& request_,
                                                  boost::asio::yield_context yield_,
                                                  const std::string& userName_,
                                                  const std::string& emailAddress_,
                                                  const std::string& cookieString_)
    {
        std::error_code error1;
        request_->setContentMIMEType(Santiago::MIMEType::TEXT);
        request_->out()<<"Deleting "<<userName_<<"..."<<std::endl;

        _userController.deleteUser(
            cookieString_,
            yield_,
            error1);

        if(error1)
        {
            request_->out()<<"Delete Operation failed. \n";
            request_->out()<<error1.message()<<std::endl;
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

    void AsyncDeleteUserHandler::handleNonVerifiedRequest(const RequestPtr& request_,
                                                     boost::asio::yield_context yield_)
    {
        request_->setContentMIMEType(Santiago::MIMEType::TEXT);
        request_->out()<<"User not logged in. Please login(login.fcgi) before trying again."<<std::endl;
        request_->setAppStatus(0);
        std::error_code error;
        request_->commit(error);        
    }
    
    }}
