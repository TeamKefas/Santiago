#include <iostream>

#include "AuthenticatorTesterBase.h"

namespace Test{ namespace Authentication
{
    void AuthenticatorTesterBase::testAssert(const std::string& description, bool value_)
    {
        std::cout<<"description: \t "<<(value_?"[SUCCEEDED]":"[FAILED]")<<std::endl;
    }

    void  AuthenticatorTesterBase::runTests(boost::asio::yield_context yieldContext_)
    {
        //first create 3 users santiagoTestUser1, santiagoTestUser2, santiagoTestUser3
        //call spawn on run1UserTests, run2UserTests
        
        SantiagoTestUser user1,user2,user3;

        user1.userName = "user1";
        user1.password = "pass1";

        user2.userName = "user2";
        user2.password = "pass2";
        
        user3.userName = "user3";
        user3.password = "pass3";


        boost::asio::spawn(_ioService, std::bind(&AuthenticatorTesterBase::run1UserTests, this, yieldContext_, user1));
        boost::asio::spawn(_ioService, std::bind(&AuthenticatorTesterBase::run1UserTests, this, yieldContext_, user2));
        boost::asio::spawn(_ioService, std::bind(&AuthenticatorTesterBase::run1UserTests, this, yieldContext_, user3));
        boost::asio::spawn(_ioService, std::bind(&AuthenticatorTesterBase::run2UserTests, this, yieldContext_, user2, user3));
        boost::asio::spawn(_ioService, std::bind(&AuthenticatorTesterBase::run2UserTests, this, yieldContext_, user1, user3));
        boost::asio::spawn(_ioService, std::bind(&AuthenticatorTesterBase::run2UserTests, this, yieldContext_, user1, user2));

    }
     void AuthenticatorTesterBase::run1UserTests(boost::asio::yield_context yieldContext_,
                                                 const SantiagoTestUser& santiagoTestUser1)
     {
         //TODO
     }
    
    void AuthenticatorTesterBase::run2UserTests(boost::asio::yield_context yieldContext_,
                                                const SantiagoTestUser& santiagoTestUser1,
                                                const SantiagoTestUser& santiagoTestUser2)
    {
        //TODO
    }
    

}}
