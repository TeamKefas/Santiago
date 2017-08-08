#ifndef SANTIAGO_AUTHENTICATION_SINGLENODE_CONTROLLER_H
#define SANTIAGO_AUTHENTICATION_SINGLENODE_CONTROLLER_H

#include "Santiago/Authentication/ControllerBase.h"

namespace Santiago{ namespace Authentication{ namespace SingleNode
{
    template<typename ClientIdType>
    class Controller:public Authentication::ControllerBase<ClientIdType>
    {
    public:
        typedef Thread::ThreadSpecificVar<SantiagoDBTables::MariaDBConnection> ThreadSpecificDbConnection;
        typedef ControllerData<ClientIdType> MyControllerData;
        
        Controller(ThreadSpecificDbConnection& databaseConnection_,
                   boost::asio::io_service& ioService_,
                   const boost::property_tree::ptree& config_);

        virtual ~Controller()
	{}
        
        std::error_code logoutCookieFromAllClients(const std::string& cookieString_,
                                                   boost::asio::yield_context yield_);
        
        std::error_code logoutUserFromAllClients(const std::string& userName_,
                                                 boost::asio::yield_context yield_);

    private:
        ThreadSpecificDbConnection& _dbConnection;
        MyControllerData            _data;   
    };
        
}}}

#endif
