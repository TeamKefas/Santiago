#include "ControllerDataBase.h"

namespace Santiago{ namespace Authentication
{
    boost::optional<SantiagoDBTables::SessionsRec> ControllerDataBase::getCookieSessionsRec(const std::string& cookieString_) const
    {
        auto iter = _cookieStringCookieDataPtrMap.find(cookieString_);
        if(iter != _cookieStringCookieDataPtrMap.end())
        {
            return iter->second->_sessionsRec;
        }
        return boost::none;
    }
    
    std::vector<std::string> ControllerDataBase::getAllCookieStringsForUser(const std::string& userName_) const
    {
        auto iter = _userNameUserDataMap.find(userName_);
        ST_ASSERT(iter != _userNameUserDataMap.end());
        return iter->second._cookieList;
    }

    boost::optional<std::string> ControllerDataBase::getUserEmailAddress(const std::string& userName_) const
    {
        auto iter = _userNameUserDataMap.find(userName_);
        if(iter != _userNameUserDataMap.end())
        {
            return iter->second._emailAddress;
        }
        return boost::none;
    }
    
    void ControllerDataBase::updateUserEmailAddress(const std::string& userName_,
                                                    const std::string& newEmailAddress_)
    {
        auto iter = _userNameUserDataMap.find(userName_);
        ST_ASSERT(iter != _userNameUserDataMap.end());
        iter->second._emailAddress = newEmailAddress_;
        return;
    }

    void ControllerDataBase::removeCookie(const std::string& cookieString_)
    {
        auto cookieDataIter = _cookieStringCookieDataPtrMap.find(cookieString_);
        ST_ASSERT(cookieDataIter != _cookieStringCookieDataPtrMap.end());
        ST_ASSERT(cookieDataIter->second->_isBeingLoggedOut);

        removeCookieClientDataImpl(cookieDataIter->second);
        
        auto userDataIter = _userNameUserDataMap.find(cookieDataIter->second->_sessionsRec._userName);
        for(auto iter = userDataIter->second._cookieList.begin();
            iter != userDataIter->second._cookieList.end(); ++iter)
        {
            if(*iter == cookieString_)
            {
                userDataIter->second._cookieList.erase(iter);
                if(userDataIter->second._cookieList.empty())
                {
                    ST_ASSERT(!userDataIter->second._isBeingLoggedOut);
                    _userNameUserDataMap.erase(userDataIter);
                }
                break;
            }
        }
        _cookieStringCookieDataPtrMap.erase(cookieDataIter);
        return;
    }
    
    void ControllerDataBase::removeUser(const std::string& userName_)
    {
        auto userDataIter = _userNameUserDataMap.find(userName_);
        ST_ASSERT(userDataIter != _userNameUserDataMap.end());
        ST_ASSERT(userDataIter->second._isBeingLoggedOut);
        
        for(auto& cookieString: userDataIter->second._cookieList)
        {
            auto cookieDataIter = _cookieStringCookieDataPtrMap.find(cookieString);
            ST_ASSERT(cookieDataIter != _cookieStringCookieDataPtrMap.end());
            ST_ASSERT(!cookieDataIter->second->_isBeingLoggedOut);
            
            removeCookieClientDataImpl(cookieDataIter->second);            
            _cookieStringCookieDataPtrMap.erase(cookieDataIter);
        }
        _userNameUserDataMap.erase(userDataIter);
        return;
    }
    
    void ControllerDataBase::setCookieBeingLoggedOutFlag(const std::string& cookieString_)
    {
        auto cookieDataIter = _cookieStringCookieDataPtrMap.find(cookieString_);
        ST_ASSERT(cookieDataIter != _cookieStringCookieDataPtrMap.end());
        ST_ASSERT(!cookieDataIter->second->_isBeingLoggedOut);

        auto userDataIter = _userNameUserDataMap.find(cookieDataIter->second->_sessionsRec._userName);
        ST_ASSERT(userDataIter != _userNameUserDataMap.end());
        ST_ASSERT(!userDataIter->second._isBeingLoggedOut);
                
        cookieDataIter->second->_isBeingLoggedOut = true;
    }

    void ControllerDataBase::unsetCookieBeingLoggedOutFlag(const std::string& cookieString_)
    {
        auto cookieDataIter = _cookieStringCookieDataPtrMap.find(cookieString_);
        ST_ASSERT(cookieDataIter != _cookieStringCookieDataPtrMap.end());
        ST_ASSERT(cookieDataIter->second->_isBeingLoggedOut);
                
        cookieDataIter->second->_isBeingLoggedOut = false;
    }

    bool ControllerDataBase::isCookieBeingLoggedOut(const std::string& cookieString_) const
    {
        auto cookieDataIter = _cookieStringCookieDataPtrMap.find(cookieString_);
        ST_ASSERT(cookieDataIter != _cookieStringCookieDataPtrMap.end());
        
        return cookieDataIter->second->_isBeingLoggedOut;
    }

    void ControllerDataBase::setUserBeingLoggedOutFlag(const std::string& userName_)
    {
        auto userDataIter = _userNameUserDataMap.find(userName_);
        ST_ASSERT(userDataIter != _userNameUserDataMap.end());
        ST_ASSERT(!userDataIter->second._isBeingLoggedOut);

#ifndef NDEBUG        
        bool hasANonLogoutCookie = false;
        for(auto& cookieString: userDataIter->second._cookieList)
        {
            auto cookieDataIter = _cookieStringCookieDataPtrMap.find(cookieString);
            ST_ASSERT(cookieDataIter != _cookieStringCookieDataPtrMap.end());
            hasANonLogoutCookie |= !cookieDataIter->second->_isBeingLoggedOut;
        }
        ST_ASSERT(hasANonLogoutCookie);
#endif
        userDataIter->second._isBeingLoggedOut = true;
    }

    void ControllerDataBase::unsetUserBeingLoggedOutFlag(const std::string& userName_)
    {
        auto userDataIter = _userNameUserDataMap.find(userName_);
        ST_ASSERT(userDataIter != _userNameUserDataMap.end());
        ST_ASSERT(userDataIter->second._isBeingLoggedOut);
        
        userDataIter->second._isBeingLoggedOut = false;
    }

    bool ControllerDataBase::isUserBeingLoggedOut(const std::string& userName_) const
    {
        auto userDataIter = _userNameUserDataMap.find(userName_);
        if(userDataIter == _userNameUserDataMap.end())
        {
            return false;
        }
        
        return userDataIter->second._isBeingLoggedOut;
    }

    void ControllerDataBase::addCookieImpl(const std::string& userName_,
                                           const std::string& emailAddress_,
                                           const CookieDataBasePtr& cookieDataPtr_)
    {
        ST_ASSERT(_cookieStringCookieDataPtrMap.find(cookieDataPtr_->_sessionsRec._cookieString) ==
                  _cookieStringCookieDataPtrMap.end());

        _cookieStringCookieDataPtrMap.insert(std::make_pair(cookieDataPtr_->_sessionsRec._cookieString,
                                                            cookieDataPtr_));

        auto iter = _userNameUserDataMap.find(userName_);
        if(iter != _userNameUserDataMap.end())
        {
            ST_ASSERT(!iter->second._isBeingLoggedOut);
            ST_ASSERT(iter->second._emailAddress == emailAddress_);
            iter->second._cookieList.push_back(cookieDataPtr_->_sessionsRec._cookieString);
        }
        else
        {
            UserData userData(emailAddress_);
            userData._cookieList.push_back(cookieDataPtr_->_sessionsRec._cookieString);
            _userNameUserDataMap.insert(std::make_pair(userName_, userData));
        }

        return;
    }

    void ControllerData::removeCookieClientDataImpl(const CookieDataBasePtr& cookieDataPtr_)
    {
        for(auto& clientId:static_cast<CookieData&>((*cookieDataPtr_))._clientIdList)
        {
            auto cookieListIter = _clientIdCookieListMap.find(clientId);
            ST_ASSERT(cookieListIter != _clientIdCookieListMap.end());

            auto cookieIter = std::find(cookieListIter->second.begin(),
                                        cookieListIter->second.end(),
                                        cookieDataPtr_->_sessionsRec._cookieString);
            ST_ASSERT(cookieIter != cookieListIter->second.end());

            cookieListIter->second.erase(cookieIter);

            if(cookieListIter->second.empty())
            {
                _clientIdCookieListMap.erase(cookieListIter);
            }
        }
    }

    void ControllerData::addCookie(const std::string& userName_,
                                   const std::string& emailAddress_,
                                   const SantiagoDBTables::SessionsRec& sessionsRec_,
                                   const boost::optional<ClientIdType>& clientId_)
    {
        CookieDataBasePtr cookieDataPtr(new ControllerData::CookieData(sessionsRec_));
        if(*clientId_)
        {
            std::static_pointer_cast<CookieData>(cookieDataPtr)->_clientIdList.push_back(*clientId_);
        }
        addCookieImpl(userName_,emailAddress_,cookieDataPtr);

        ST_ASSERT(clientId_);
        _clientIdCookieListMap[*clientId_].push_back(sessionsRec_._cookieString);
        return;
    }
        
    void ControllerData::updateCookieAndAddClient(const SantiagoDBTables::SessionsRec& newSessionsRec_,
                                                  ClientIdType clientId_)
    {
        auto cookieDataIter = _cookieStringCookieDataPtrMap.find(newSessionsRec_._cookieString);
        ST_ASSERT(cookieDataIter != _cookieStringCookieDataPtrMap.end());
        ST_ASSERT(!cookieDataIter->second->_isBeingLoggedOut);

#ifndef NDEBUG
        auto userDataIter = _userNameUserDataMap.find(cookieDataIter->second->_sessionsRec._userName);
        ST_ASSERT(userDataIter != _userNameUserDataMap.end());
        ST_ASSERT(!userDataIter->second._isBeingLoggedOut);
#endif

        cookieDataIter->second->_sessionsRec = newSessionsRec_;
        std::static_pointer_cast<CookieData>(cookieDataIter->second)->_clientIdList.push_back(clientId_);

        std::vector<std::string> &cookieList = _clientIdCookieListMap[clientId_];
        ST_ASSERT(std::find(cookieList.begin(),cookieList.end(),newSessionsRec_._cookieString) == cookieList.end());
        cookieList.push_back(newSessionsRec_._cookieString);
        return;
    }
        
    std::vector<unsigned> ControllerData::getCookieClientIds(const std::string& cookieString_) const
    {
        auto iter = _cookieStringCookieDataPtrMap.find(cookieString_);
        ST_ASSERT(iter != _cookieStringCookieDataPtrMap.end());

        CookieDataBasePtr cookieDataBasePtr = iter->second;
        return std::static_pointer_cast<CookieData>(cookieDataBasePtr)->_clientIdList;
    }
        
    std::set<unsigned> ControllerData::getAllClientIdsForUser(const std::string& userName_) const
    {
        auto userDataIter =_userNameUserDataMap.find(userName_);
        ST_ASSERT(userDataIter != _userNameUserDataMap.end());

        const std::vector<std::string>& cookieList = userDataIter->second._cookieList;
        std::set<unsigned> allClientIdSet;
        
        for(auto& cookieString:cookieList)
        {
            CookieDataBasePtr cookieDataBasePtr = _cookieStringCookieDataPtrMap.find(cookieString)->second;
            std::vector<ClientIdType>& clientIdList =
                std::static_pointer_cast<CookieData>(cookieDataBasePtr)->_clientIdList;
            allClientIdSet.insert(clientIdList.begin(), clientIdList.end());
        }
        return allClientIdSet;
    }
}}
