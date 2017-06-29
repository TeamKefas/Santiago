#include "ClientCache.h"

namespace Santiago{ namespace Authentication{ namespace MultiNode
{

    boost::optional<UserInfo> ClientCache::getCookieInfoFromLocalCache(const std::string& cookieString_) const
    {
        std::map<std::string,std::string>::iterator cookieStringUserInfoMapIter =
            _cookieStringUserInfoMap.find(cookieString_);
        if(cookieStringUserInfoMapIter != _cookieStringUserInfoMap.end())
        {
            return cookieStringUserInfoMapIter->second;
            
        }
        else
        {
            return boost::none;
        }
    }

    void ClientCache::addCookieUserInfoToCache(const std::string& cookieString_,
                                               const std::string& userName_,
                                               const std::string& emailAdress_) const
    {
        _cookieStringUserNameMap[cookieString_] = UserInfo(userName_,emailAddress_);

        auto it = _userNameCookieStringListMap.find(userName_);
        if (it != _userNameCookieStringListMap.end())
        {
            it->second.push_back(cookieString_);
        }
        else
        {
            std::vector<std::string> cookieStringVec;
            cookieStringVec.push_back(cookieString_);
            _userNameCookieStringListMap.insert(userName_, cookieStringVec);
        }
    }
    void ClientCache::removeCookieUsernameFromCache(const std::string& cookieString_,
                                                    const std::string& userName_,
                                                    const std::string& emailAdress_)
    {
        auto it = _cookieStringUserNameMap.find(cookieString_);
        _cookieStringUserNameMap.erase(it);

        auto iterator = _userNameCookieStringListMap.find(userName_);
        iterator->second.erase(std::remove(iterator->second.begin(),
                                           iterator->second.end(),
                                           cookieString_),
                               iterator->second.end());
    }
    void ClientCache::removeAllCookiesForUser(const std::string& userName_,const std::string& emailAddress_)
    {
        
        auto it = _userNameCookieStringListMap.find(userName_);
        _userNameCookieStringListMap.erase(it);
    }
    
        }}}
