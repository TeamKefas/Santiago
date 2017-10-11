#include <cctype>

#include "AuthenticatorV1.h"

namespace Santiago{ namespace Authentication{ namespace SingleNode
{
    typedef std::shared_ptr<boost::asio::strand> StrandPtr;
    typedef std::shared_ptr<AuthenticatorImplBase> AuthenticatorImplBasePtr;
    
    std::pair<AuthenticatorImplBasePtr,StrandPtr> Authenticator::getAuthenticatorAndStrandForString(
        const std::string& string_,
        bool isNotEmailAddress_)
    {
        std::string userName = string_;
        std::error_code error;
        if(!isNotEmailAddress_)
        {
            boost::optional<SantiagoDBTables::UsersRec> usersRec = _databaseConnection.get().getUsersRecForEmailAddress(string_,error);
            if(usersRec)
            {
                userName = usersRec->_userName;
            }
        }
        return _authenticatorStrandPair[static_cast<int>(toupper(userName.at(0)))-65];
    }
}}}
