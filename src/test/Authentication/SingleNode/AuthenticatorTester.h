#ifndef SANTIAGOTEST_AUTHENTICATION_AUTHENTICATORTESTER_H
#define SANTIAGOTEST_AUTHENTICATION_AUTHENTICATORTESTER_H

#include <iostream>
#include <memory>

#include "Santiago/SantiagoDBTables/MariaDBConnection.h"
#include "Santiago/Thread/ThreadSpecificVar.h"
#include "Santiago/Authentication/ControllerDataBase.h"

#include "../AuthenticatorTesterBase.h"

namespace Test{ namespace Authentication{ namespace SingleNode
{
    class AuthenticatorTester:public AuthenticatorTesterBase
    {
    public:
        typedef std::shared_ptr<Santiago::Authentication::SingleNode::Authenticator> AuthenticatorBasePtr;
        typedef Santiago::Thread::ThreadSpecificVar<Santiago::SantiagoDBTables::MariaDBConnection> ThreadSpecificDbConnection;

        typedef typename Santiago::Authentication::ControllerData::ClientIdType ClientIdType;

        AuthenticatorTester(boost::asio::io_service& ioService_,
                            const boost::property_tree::ptree& config_)
            :AuthenticatorTesterBase(ioService_,config_)
            ,_databaseConnection(boost::bind(&Santiago::SantiagoDBTables::CreateMariaDBConnection,config_))
        {
            initAuthenticator(config_);
        }

    protected:
        
        void initAuthenticator(const boost::property_tree::ptree& config_);
        
        ClientIdType                    _clientId;
        ThreadSpecificDbConnection      _databaseConnection;

    };
    
}}}
#endif
