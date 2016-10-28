#ifndef SANTIAGO_EXAMPLES_SESSIOMANAGER_SERVER_H
#define SANTIAGO_EXAMPLES_SESSIOMANAGER_SERVER_H

#include <boost/asio.hpp>

#include "Santiago/AppServer/ServerBase.h"
#include "Santiago/User/SingleNode/Controller.h"
#include "Santiago/ThreadSpecificVar/ThreadSpecificVar.h"
#include "Santiago/SantiagoDBTables/MariaDBConnection.h"
#include "LoginPageHandler.h"
#include "SimplePageHandler.h"
#include "ErrorPageHandler.h"
#include "SignupPageHandler.h"

namespace SimpleAppServer
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
            _databaseConnection(std::bind(Santiago::SantiagoDBTables::CreateMariaDBConnection,config_)),
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
                ret.reset(new SignupPageHandler(_userController));
            }
            else if(documentURI_ == "/")
            {
                ret.reset(new SimplePageHandler(_userController, "/"));
            }
            else if(documentURI_ == "/signup.html")
            {
                ret.reset(new SimplePageHandler(_userController, "signup.html"));
            }
            else //random url
            {
                ret.reset(new ErrorPageHandler(_userController));
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
        Santiago::User::SingleNode::Controller              _userController;
    };

}

#endif
