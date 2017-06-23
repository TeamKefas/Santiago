#ifndef SANTIAGO_AUTHENTICATION_MULTINODE_CLIENTCACHE_H
#define SANTIAGO_AUTHENTICATION_MULTINODE_CLIENTCACHE_H

namespace Santiago{ namespace Authentication{ namespace MultiNode
{
    class ClientCache
    {
    public:

        Optional<UserInfo> getCookieInfoFromLocalCache(const std::string& cookieString_) const;
        void addCookieInfoToCache(const std::string& cookieString_, const UserInfo& userInfo_) const;
        void removeCookieInfoFromCache(const std::string& cookieString_);
        void removeAllCookiesForUser(const std::string& userName_);

    protected:
        
        std::map<std::string,std::string>                                       _cookieStringUserNameMap;
        std::map<std::pair<std::string,std::string>,std::vector<std::string> >  _userNameEmailAddressCookieStringListMap;
    };


}}}

#endif
