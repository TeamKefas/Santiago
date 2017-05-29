#ifndef SANTIAGO_TEST_APPSERVER_SERVER_H
#define SANTIAGO_TEST_APPSERVER_SERVER_H

#include <boost/asio.hpp>

#include "../../Santiago/AppServer/ServerBase.h"
#include "../../Santiago/Authentication/SingleNode/Authenticator.h"
#include "Santiago/ThreadSpecificVar/ThreadSpecificVar.h"
#include "Santiago/SantiagoDBTables/MariaDBConnection.h"
#include "RequestHandlerBase.h"
#include "LogoutUserHandler.h"
#include "LoginUserHandler.h"
#include "CreateUserHandler.h"
#include "LogoutUserForAllCookieHandler.h"
#include "GetUserInfoHandler.h"
#include "DeleteUserHandler.h"
#include "ErrorURIHandler.h"
#include "PasswordRecoveryInitiateRecoveryHandler.h"
#include "PasswordRecoveryReceiveNewPasswordHandler.h"
#include "PasswordRecoverySetNewPasswordHandler.h"

#include "AsyncCreateUserHandler.h"
#include "AsyncLoginUserHandler.h"
#include "AsyncLogoutUserHandler.h"
#include "AsyncLogoutUserForAllCookieHandler.h"
#include "AsyncGetUserInfoHandler.h"
#include "AsyncDeleteUserHandler.h"

namespace Test{ namespace AppServer
{
    class Server:public Santiago::AppServer::ServerBase<boost::asio::ip::tcp>
    {
    public:

        typedef Santiago::ThreadSpecificVar::ThreadSpecificVar<Santiago::SantiagoDBTables::MariaDBConnection>
        SantiagoDBConnection;
        typedef Santiago::AppServer::ServerBase<boost::asio::ip::tcp> MyBase;

        Server(const boost::property_tree::ptree& config_):
            MyBase(getServerLocalEndpoint(config_)),
            _config(config_),
	    _databaseConnection(std::bind(Santiago::SantiagoDBTables::CreateMariaDBConnection,_config)),
            _userController(_databaseConnection,MyBase::_ioService,config_)
        {
        }


        ~Server()
        {}
      
    protected:

        virtual RequestHandlerBasePtr route(const std::string& documentURI_)
        {
            RequestHandlerBasePtr ret;
            
            if(documentURI_ == "/createuser.fcgi")
            {
                	ret.reset(new AsyncCreateUserHandler(_userController));
            }
            else if(documentURI_ == "/loginuser.fcgi")
            {
                ret.reset(new AsyncLoginUserHandler(_userController));
            }
            else if(documentURI_ == "/logoutuser.fcgi")
            {
                ret.reset(new AsyncLogoutUserHandler(_userController));
            }
            else if(documentURI_ == "/logoutuserforallcookie.fcgi")
            {
                ret.reset(new AsyncLogoutUserForAllCookieHandler(_userController));
            }
            else if(documentURI_ == "/getuserinfo.fcgi")
            {
                ret.reset(new AsyncGetUserInfoHandler(_userController));
            }
            else if(documentURI_ == "/deleteuser.fcgi")
            {
                ret.reset(new AsyncDeleteUserHandler(_userController));
            }
            else if(documentURI_ == "/password-recovery-initiate.fcgi")
            {
                ret.reset(new PasswordRecoveryInitiateRecoveryHandler(_userController));
            }
            else if(documentURI_ == "/password-recovery-receive-new-password.fcgi")
            {
                ret.reset(new PasswordRecoveryReceiveNewPasswordHandler(_userController));
            }
            else if(documentURI_ == "/password-recovery-set-new-password.fcgi")
            {
                ret.reset(new PasswordRecoverySetNewPasswordHandler(_userController));
            }
            else
            {
                ret.reset(new ErrorURIHandler(_userController));
            }
            return ret;
        }

       Santiago::LocalEndpoint<boost::asio::ip::tcp>
       getServerLocalEndpoint(const boost::property_tree::ptree& config_) const 
        {
            //unsigned portNo = config_.get<unsigned>("Santiago.AppServer.port");
            
            return Santiago::LocalEndpoint<typename boost::asio::ip::tcp>
                (config_.get<unsigned>("Santiago.AppServer.port"));
        }

        
        boost::property_tree::ptree                         _config;
        SantiagoDBConnection                                _databaseConnection;
        Santiago::Authentication::SingleNode::Authenticator              _userController;

    };

}}

#endif
