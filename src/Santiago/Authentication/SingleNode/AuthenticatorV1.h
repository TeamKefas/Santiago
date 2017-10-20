#ifndef SANTIAGO_AUTHENTICATION_SINGLENODE_AUTHENTICATORV1_H
#define SANTIAGO_AUTHENTICATION_SINGLENODE_AUTHENTICATORV1_H

#include "AuthenticatorImpl.h"
#include "Santiago/Authentication/AuthenticatorBaseV1.h"
#include "Santiago/Thread/ThreadSpecificVar.h"
#include "Santiago/SantiagoDBTables/MariaDBConnection.h"

namespace Santiago{ namespace Authentication{ namespace SingleNode
{
    class Authenticator:public AuthenticatorBase
    {
    public:
        typedef Thread::ThreadSpecificVar<SantiagoDBTables::MariaDBConnection> ThreadSpecificDbConnection;
        typedef unsigned ClientIdType;
        
        Authenticator(boost::asio::io_service& ioService_,
                      const boost::property_tree::ptree& config_,
                      const ClientIdType& clientId_,
                      ThreadSpecificDbConnection& databaseConnection_):AuthenticatorBase(ioService_,config_),_databaseConnection(databaseConnection_)
        {
            for(int i = 0; i < 26; ++i)
            {
                auto strandPtr = std::make_shared<boost::asio::strand>(ioService_);
                auto authenticatorImplBasePtr = std::make_shared<AuthenticatorImpl>(_databaseConnection);
                _authenticatorStrandPair[i] = std::make_pair(authenticatorImplBasePtr,strandPtr);
            }
        }
                
        virtual std::pair<AuthenticatorImplBasePtr,StrandPtr> getAuthenticatorAndStrandForString(
            const std::string& string_,
            bool isNotEmailAddress_);

    private: 
        ThreadSpecificDbConnection& _databaseConnection;       
        std::pair<AuthenticatorImplBasePtr,StrandPtr> _authenticatorStrandPair[26];
    };

}}}

#endif
