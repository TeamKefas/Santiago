#include <iostream>
#include "RequestHandlerBase.h"


namespace Test{ namespace AppServer
{
   
    void RequestHandlerBase::handleRequest(const RequestPtr& request_)
    {
        printEcho(request_);
    }

   
   

    //debug fn
    void RequestHandlerBase::printEcho(const RequestPtr& request_) const
    {

        request_->setContentMIMEType(Santiago::MIMEType::TEXT);
        std::cout<<"_____________Echo_______________\n"<<"FCGI Params:\n";       
    }

   }
}
