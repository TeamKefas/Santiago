#ifndef SANTIAGO_AUTHENTICATION_SINGLENODE_AUTHENTICATORV1_H
#define SANTIAGO_AUTHENTICATION_SINGLENODE_AUTHENTICATORV1_H

#include "Santiago/Authentication/AuthenticatorBase.h"
#include "Santiago/Thread/ThreadSpecificVar.h"

namespace Santiago{ namespace Authentication{ namespace SingleNode
{
    //TODO: Update AuthenticatorBase before implementing this
    class Authenticator:public AuthenticatorBase
    {
    public:
        typedef Thread::ThreadSpecificVar<SantiagoDBTables::MariaDBConnection> ThreadSpecificDbConnection;
        
        Authenticator(boost::asio::io_service& ioService_,
                      const boost::property_tree::ptree& config_,
                      ThreadSpecificDbConnection& databaseConnection_)

        virtual std::pair<ControllerPtr,StrandPtr> getControllerAndStrandForString(
            const std::string& string_,
            bool isNotEmailAddress_);
    };

}}}

#endif
