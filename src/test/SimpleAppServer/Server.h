#ifndef SANTIAGO_TEST_SIMPLEAPPSERVER_SERVER_H
#define SANTIAGO_TEST_SIMPLEAPPSERVER_SERVER_H

#include <boost/asio.hpp>

#include "Santiago/AppServer/ServerBase.h"
#include "Santiago/Authentication/SingleNode/Authenticator.h"
#include "Santiago/Thread/ThreadSpecificVar.h"
#include "Santiago/SantiagoDBTables/MariaDBConnection.h"
#include "LoginPageHandler.h"
#include "SimplePageHandler.h"
#include "SignupHandler.h"
#include "PasswordRecoveryInitiateRecoveryHandler.h"

namespace SimpleAppServer
{
    class Server:public Santiago::AppServer::ServerBase<boost::asio::ip::tcp>
    {
    public:

        typedef Santiago::Thread::ThreadSpecificVar<Santiago::SantiagoDBTables::MariaDBConnection>
        SantiagoDBConnection;

        typedef Santiago::AppServer::ServerBase<boost::asio::ip::tcp> MyBase;

        Server(const boost::property_tree::ptree& config_):
            MyBase(getServerLocalEndpoint(config_)),
            _config(config_),
	    _databaseConnection(std::bind(Santiago::SantiagoDBTables::CreateMariaDBConnection,_config)),
            _userController(_databaseConnection,MyBase::_ioService,config_)
        {
        }

    protected:

        virtual RequestHandlerBasePtr route(const std::string& documentURI_)
        {
            RequestHandlerBasePtr ret;
            if(documentURI_ == "/login.fcgi")
            {
                ret.reset(new LoginPageHandler(_userController));
            }
            else if(documentURI_ == "/signup.fcgi")
            {
                ret.reset(new SignupHandler(_userController));
            }
            else if(documentURI_ == "/password-recovery-initiate.fcgi")
            {
                ret.reset(new PasswordRecoveryInitiateRecoveryHandler(_userController));
            }
            else
            {
                ret.reset(new SimplePageHandler(_userController));
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

}

#endif
