#ifndef SANTIAGO_TEST_SIMPLEAPPSERVER_SERVER_H
#define SANTIAGO_TEST_SIMPLEAPPSERVER_SERVER_H

#include <boost/asio.hpp>

#include "Santiago/AppServer/ServerBase.h"
#include "Santiago/User/SingleNode/Controller.h"

namespace SimpleAppServer
{
    class Server:public AppServer::ServerBase<boost::asio::tcp::ip>
    {

    public:

        typedef AppServer::ServerBase<boost::asio::tcp::ip> MyBase;

        Server(const boost::property_tree::ptree& config_):
            MyBase(getServerLocalEndpoint(config_)),
            _config(config_),
            _databaseConnection(config_),
            _userController(_databaseConnection,MyBase::_ioService,config_)
        {
        }

    protected:

        virtual RequestHandlerPtr route(const std::string& documentURI_)
        {
            RequestHandlerPtr ret;
            if(documentURI_ == "/login.fcgi")
            {
                ret.reset(new LoginPageHandler(_userController));
            }
            else
            {
                ret.reset(new SimplePageHandler(_userController));
            }
            return ret;
        }

        Santiago::LocalEndpoint<boost::asio::ip::tcp> 
        getServerLocalEndpoint(const boost::property_tree::ptree& config_) const;

        boost::property_tree::ptree                         _config;
        SantiagoDBTables::MariaDBConnection                 _databaseConnection;
        User::SingleNode::Controller                        _userController;
    };

}

#endif
