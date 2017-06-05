#ifndef SANTIAGOTEST_AUTHENTICATION_AUTHENTICATORTESTER_H
#define SANTIAGOTEST_AUTHENTICATION_AUTHENTICATORTESTER_H

#include <iostream>
#include <memory>

#include "Santiago/SantiagoDBTables/MariaDBConnection.h"
#include "Santiago/ThreadSpecificVar/ThreadSpecificVar.h"

#include "../AuthenticatorTesterBase.h"

namespace Test{ namespace Authentication{ namespace SingleNode
{
    class AuthenticatorTester:public AuthenticatorTesterBase
    {
    public:
        typedef std::shared_ptr<Santiago::Authentication::SingleNode::Authenticator> AuthenticatorBasePtr;
        typedef ThreadSpecificVar::ThreadSpecificVar<SantiagoDBTables::MariaDBConnection> ThreadSpecificDbConnection;

        AuthenticatorTester(boost::asio::io_service& ioService_,
                            const boost::property_tree::ptree& config_)
            :AuthenticatorTesterBase(ioService_,config_)
            ,_databaseConnection(boost::bind(&Santiago::SantiagoDBTables::CreateMariaDBConnection,config))
        {
            initAuthenticator();
        }

    protected:
        
        void initAuthenticator();
        

        ThreadSpecificDbConnection      _databaseConnection;

    };
    
}}}
#endif
