#include "EchoHandler.h"

namespace Test{ namespace AppServer
{


    void EchoHandler::handleRequest(const RequestPtr& request_)
    {
        request_->setContentMIMEType(Santiago::MIMEType::TEXT);

        request_->out()<<"Hello World "<<std::endl;
        
        request_->setAppStatus(0);
        std::error_code error;
        request_->commit(error);
    }

 
}}
