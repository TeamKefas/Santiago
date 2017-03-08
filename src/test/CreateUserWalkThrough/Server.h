#ifndef SLABS_APPSERVER_SERVER_H
#define SLABS_APPSERVER_SERVER_H

#include <boost/asio.hpp>

#include "../../Santiago/AppServer/ServerBase.h"
#include "../../Santiago/User/SingleNode/Controller.h"
#include "Santiago/ThreadSpecificVar/ThreadSpecificVar.h"
#include "Santiago/SantiagoDBTables/MariaDBConnection.h"
#include "RequestHandlerBase.h"
#include "CreateUserHandler.h"
#include "ErrorURIHandler.h"


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
                ret.reset(new CreateUserHandler(_userController));
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
        Santiago::User::SingleNode::Controller              _userController;

    };

}}

#endif
