#ifndef SANTIAGO_AUTHENTICATION_SINGLENODE_AUTHENTICATORV1_H
#define SANTIAGO_AUTHENTICATION_SINGLENODE_AUTHENTICATORV1_H

#include "AuthenticatorImpl.h"
#include "../AuthenticatorBaseV1.h"
#include "../../Thread/ThreadSpecificVar.h"
#include "../..//SantiagoDBTables/MariaDBConnection.h"

namespace Santiago{ namespace Authentication{ namespace SingleNode
{
    class Authenticator:public AuthenticatorBase
    {
    public:
        typedef Thread::ThreadSpecificVar<SantiagoDBTables::MariaDBConnection> ThreadSpecificDbConnection;
        
        Authenticator(boost::asio::io_service& ioService_,
                      const boost::property_tree::ptree& config_,
                      ThreadSpecificDbConnection& databaseConnection_):AuthenticatorBase(ioService_,config_),_databaseConnection(databaseConnection_)
        {
            std::map<std::string,std::string> oicProviderNameCertURLMap;
            try
            {
                for(auto iter = _config.get_child("Santiago.Authentication..OICProviders").begin();
                    iter != _config.get_child("Santiago.Authentication..OICProviders").end();
                    iter++)
                {
                    //iter->first gives key id
                    //iter->second gives cert url
                    bool flag = oicProviderNameCertURLMap.insert(
                        std::make_pair(iter->first,
                                       iter->second.get<std::string>("cert_url"))).second;
                    if(!flag)
                    {
                        ST_LOG_INFO("Multiple entries for same provider in Santiago config in Authentication.OICProviders\n");
                    }
                }
            }
            catch(std::exception& e)
            {
                ST_LOG_INFO("OICProvider parsing failed. message: "<< e.what() << std::endl);
            }
            catch(...)
            {
                ST_LOG_INFO("OICProvider parsing failed." << std::endl);
            }
            
            for(int i = 0; i < 26; ++i)
            {
                auto strandPtr = std::make_shared<boost::asio::strand>(ioService_);
                auto authenticatorImplBasePtr = std::make_shared<AuthenticatorImpl>(_databaseConnection,oicProviderNameCertURLMap);
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
