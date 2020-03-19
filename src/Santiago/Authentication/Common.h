#ifndef SANTIAGO_AUTHENTICATION_COMMON_H
#define SANTIAGO_AUTHENTICATION_COMMON_H

#include <string>

namespace Santiago{ namespace Authentication
{
    struct UserInfo
    {
        /**
         * The constructor
         * @param userName_- Username from the user. 
         * @param emailAddress_- Email address from the user.
         */
        UserInfo(const std::string& userName_,const std::string& emailAddress_):
            _userName(userName_),
            _emailAddress(emailAddress_)
        {}

        std::string  _userName;
        std::string  _emailAddress;
    };
}}
#endif
