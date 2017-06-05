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
    }


}}
