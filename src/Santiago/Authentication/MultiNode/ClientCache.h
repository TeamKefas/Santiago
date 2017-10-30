#ifndef SANTIAGO_AUTHENTICATION_MULTINODE_CLIENTCACHE_H
#define SANTIAGO_AUTHENTICATION_MULTINODE_CLIENTCACHE_H

#include <boost/optional/optional.hpp>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include "Santiago/Authentication/AuthenticatorBaseV1.h"

namespace Santiago{ namespace Authentication{ namespace MultiNode
{
    class ClientCache
    {
    public:

        boost::optional<UserInfo> getCookieInfoFromLocalCache(const std::string& cookieString_) const;
        void addCookieUserInfoToCache(const std::string& cookieString_,
                                      const std::string& userName_,
                                      const std::string& emailAdress_);
        void removeCookieUsernameFromCache(const std::string& cookieString_,
                                           const std::string& userName_,
                                           const std::string& emailAdress_);
        void removeAllCookiesForUser(const std::string& userName_, const std::string& emailAddress_);

    protected:
        
        std::map<std::string,UserInfo>                                       _cookieStringUserInfoMap;
        std::map<std::string,std::vector<std::string> >                      _userNameCookieStringListMap;
    };


}}}

#endif
