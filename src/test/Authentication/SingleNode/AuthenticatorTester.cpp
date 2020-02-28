#include "AuthenticatorTester.h"

namespace Test{ namespace Authentication{ namespace SingleNode
{
    void AuthenticatorTester::initAuthenticator(const boost::property_tree::ptree& config_)
    {
        _authenticatorBasePtr.reset(new Santiago::Authentication::SingleNode::Authenticator(
                                        _ioService,
                                        _config,
                                        _databaseConnection));
    }

}}}
