#include "AuthenticatorTester.h"

namespace Test{ namespace Authenticator{ namespace SingleNode
{
    void AuthenticatorTester::initAuthenticator(const boost::property_tree::ptree& config_)
    {
        _authenticatorBasePtr.reset(new Santiago::Authentication::SingleNode::Authenticator(
                                        _databaseConnection,
                                        _ioService,
                                        _config));
    }

}}}
