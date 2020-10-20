#ifndef SANTIAGO_AUTHENTICATION_COMMON_H
#define SANTIAGO_AUTHENTICATION_COMMON_H

#include <memory>
#include <string>

#include "../Common.h"
#include "../ErrorCategory.h"
#include "../Utils/STLog.h"

struct _cjose_jwk_int;
struct _cjose_jws_int;

namespace Santiago{ namespace Authentication
{
    typedef _cjose_jwk_int JWK;
    typedef std::shared_ptr<JWK> JWKPtr;

    typedef struct _cjose_jws_int JWS;
    typedef std::shared_ptr<JWS> JWSPtr;
        
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
