#include "ClientCache.h"

namespace Santiago{ namespace Authentication{ namespace MultiNode
{

    boost::Optional<std::string> ClientCache::getCookieInfoFromLocalCache(const std::string& cookieString_) const
    {
        std::map<std::string,std::string>::iterator cookieStringUserNameMapIter =
            _cookieStringUserNameMap.find(cookieString_);
        if(cookieStringUserNameMapIter != _cookieStringUserNameMap.end())
        {
            return cookieStringUserNameMapIter->second;
        }
        else
        {
            return boost::none;
        }
    }

    void ClientCache::addCookieUserNameToCache(const std::string& cookieString_,
                                               const std::string& userName_,
                                               const std::string& emailAdress_) const
    {
        _cookieStringUserNameMap[cookieString_] = userName_;

        auto it = _userNameEmailAddressCookieStringListMap.find(std::make_pair(userName_,emailAdress_));
        if (it != _userNameEmailAddressCookieStringListMap.end())
        {
            it->second.push_back(cookieString_);
        }
        else
        {
            std::vector<std::string> cookieStringVec;
            cookieStringVec.push_back(cookieString_);
            _userNameEmailAddressCookieStringListMap.insert(std::make_pair(userName_,emailAddress_), cookieStringVec);
        }
    }
    void ClientCache::removeCookieUsernameFromCache(const std::string& cookieString_,
                                                    const std::string& userName_,
                                                    const std::string& emailAdress_)
    {
        auto it = _cookieStringUserNameMap.find(cookieString_);
        _cookieStringUserNameMap.erase(it);

        auto iterator = _userNameEmailAddressCookieStringListMap.find(std::make_pair(userName_,emailAddress_));
        iterator->second.erase(std::remove(iterator->second.begin(),
                                           iterator->second.end(),
                                           cookieString_),
                               iterator->second.end());
    }
    void ClientCache::removeAllCookiesForUser(const std::string& userName_,const std::string& emailAddress_)
    {   
        auto it = _userNameEmailAddressCookieStringListMap.find(std::make_pair(userName_,emailAdress_));
        _userNameEmailAddressCookieStringListMap.erase(it);
    }
    
        }}}
