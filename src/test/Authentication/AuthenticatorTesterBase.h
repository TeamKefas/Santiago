#ifndef SANTIAGOTEST_AUTHENTICATION_AUTHENTICATORTESTERBASE_H
#define SANTIAGOTEST_AUTHENTICATION_AUTHENTICATORTESTERBASE_H

#include <memory>

#include <boost/asio.hpp>
#include <boost/property_tree/ptree.hpp>

#include "Santiago/Authentication/SingleNode/Authenticator.h"

namespace Test{ namespace Authentication
{
    class AuthenticatorTesterBase
    {
    public:
        typedef std::shared_ptr<Santiago::Authentication::AuthenticatorBase> AuthenticatorBasePtr;
        AuthenticatorTesterBase(boost::asio::io_service& ioService_,
                                const boost::property_tree::ptree& config_)
            :_ioService(ioService_)
            ,_config(config_)
            ,_authenticatorBasePtr()
        {}

        void runTests(boost::asio::yield_context yieldContext_);
        
        struct SantiagoTestUser
        {
            std::string userName;
            std::string password;   
        };

    protected:

        void testAssert(const std::string& description, bool value_);

        void run1UserTests(boost::asio::yield_context yieldContext_,
                           const SantiagoTestUser& santiagoTestUser1);

                           /*const std::string& userName_,
                             const std::string& password_);*/
        
        void run2UserTests(boost::asio::yield_context yieldContext_,
                           const SantiagoTestUser& santiagoTestUser1,
                           const SantiagoTestUser& santiagoTestUser2);
                           
                           /*const std::string& user1UserName_,
                           const std::string& user1Password_,
                           const std::string& user2UserName_,
                           const std::string& user2Password_);*/

        boost::asio::io_service     &_ioService;
        boost::property_tree::ptree  _config;
        AuthenticatorBasePtr         _authenticatorBasePtr;
    };
    
}}
#endif
