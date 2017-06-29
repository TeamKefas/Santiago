#ifndef SANTIAGO_AUTHENTICATION_MULTINODE_CLIENTCACHE_H
#define SANTIAGO_AUTHENTICATION_MULTINODE_CLIENTCACHE_H

namespace Santiago{ namespace Authentication{ namespace MultiNode
{
    class ClientCache
    {
    public:

        boost::Optional<std::string> getCookieInfoFromLocalCache(const std::string& cookieString_) const;
        void addCookieUserNameToCache(const std::string& cookieString_,
                                      const std::string& userName_,
                                      const std::string& emailAdress_) const;
        void removeCookieUsernameFromCache(const std::string& cookieString_,
                                           const std::string userName_,
                                           const std::string emailAdress_);
        void removeAllCookiesForUser(const std::string& userName_, const std::string& emailAddress_);

    protected:
        
        std::map<std::string,std::string>                                       _cookieStringUserNameMap;
        std::map<std::pair<std::string,std::string>,std::vector<std::string> >  _userNameEmailAddressCookieStringListMap;
    };


}}}

#endif
