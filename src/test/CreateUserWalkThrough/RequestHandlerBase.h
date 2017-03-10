#ifndef SANTIAGO_TEST_SESSIONSERVER_REQUESTHANDLERBASE_H
#define SANTIAGO_TEST_SESSIONSERVER_REQUESTHANDLERBASE_H

#include <memory>

#include <boost/asio.hpp>

#include "../../Santiago/AppServer/RequestHandlerBase.h"
#include "../../Santiago/User/SingleNode/Controller.h"
namespace Test { namespace AppServer
    {
        class RequestHandlerBase:public Santiago::AppServer::RequestHandlerBase<boost::asio::ip::tcp>
        {
        public:
            
            typedef Santiago::AppServer::RequestHandlerBase<boost::asio::ip::tcp> MyBase;
            typedef std::shared_ptr<RequestHandlerBase> Ptr;
            
            RequestHandlerBase(Santiago::User::ControllerBase& userController_):
                _userController(userController_)
        {}
            
            virtual void handleRequest(const RequestPtr& request_);
            
        protected:
            
            virtual void handleVerifyCookieAndGetUserInfo(const RequestPtr& request_,
                                                          const std::string& cookieString_,
                                                          std::error_code error_,
                                                      const boost::optional<Santiago::User::UserInfo>& userInfo_);
            
            virtual void handleVerifiedRequest(const RequestPtr& request_,
                                               const std::string& userName_,
                                               const std::string& emailAddress_,
                                               const std::string& cookieString_) = 0;
            virtual void handleNonVerifiedRequest(const RequestPtr& request_) = 0;
            
        virtual void printEcho(const RequestPtr& request_);
            
            Santiago::User::ControllerBase                  &_userController;
        };
        
    }}        
#endif
