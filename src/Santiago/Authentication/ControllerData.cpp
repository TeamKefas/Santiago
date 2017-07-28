#include "ControllerData.h"

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
         return boost ::none;
     }
    
    void ControllerDataBase::removeCookie(const std::string& cookieString_)
    {
        auto cookieDataIter = _cookieStringCookieDataPtrMap.find(cookieString_);
        ST_ASSERT(cookieDataIter != _cookieStringCookieDataPtrMap.end());

        _cookieStringCookieDataPtrMap.erase(cookieDataIter);

        auto userDataIter = _userNameUserDataMap.find(cookieDataIter->second->_sessionsRec._userName);
        for(auto iter = userDataIter->second._cookieList.begin();
            iter != userDataIter->second._cookieList.end(); ++iter)
        {
            if(*iter == cookieString_)
            {
                userDataIter->second._cookieList.erase(iter);
                if(userDataIter->second._cookieList.empty())
                {
                    _userNameUserDataMap.erase(userDataIter);
                    break;
                }
            }
        }
        return;
    }

    /*
    void ControllerDataBase::removeUser(const std::string& userName_)
    {
        auto userDataIter = _userNameUserDataMap.find(userName_);
        ST_ASSERT(userDataIter != _userNameUserDataMap.end());

        std::vector<std::string> cookieList = userDataIter->second._cookieList;
        std::vector<std::string>::iterator iter;
        for(iter = cookieList.begin(); iter != cookieList.end(); ++iter)
        {
            _cookieStringCookieDataPtrMap.erase(*iter);
        }
        _userNameUserDataMap.erase(userDataIter);
        return;
    }
    */

    void ControllerDataBase::updateUserEmailAddress(const std::string& userName_,
                                                    const std::string& newEmailAddress_)
    {
        auto iter = _userNameUserDataMap.find(userName_);
        ST_ASSERT(iter != _userNameUserDataMap.end());
        iter->second._emailAddress = newEmailAddress_;
        return;
    }
      
    void ControllerData<None>::addCookie(const std::string& userName_,
                                         const std::string& emailAddress_,
                                         const SantiagoDBTables::SessionsRec& sessionsRec_,
                                         const boost::optional<None>&)
    {
        ST_ASSERT(_cookieStringCookieDataPtrMap.find(sessionsRec_._cookieString) ==
                  _cookieStringCookieDataPtrMap.end());

        CookieDataBasePtr cookieDataPtr(new ControllerData::CookieData(sessionsRec_));
        _cookieStringCookieDataPtrMap[sessionsRec_._cookieString] = cookieDataPtr;

        auto iter = _userNameUserDataMap.find(userName_);
        if(iter != _userNameUserDataMap.end())
        {
            ST_ASSERT(iter->second._emailAddress == emailAddress_);
            iter->second._cookieList.push_back(sessionsRec_._cookieString);
        }
        else
        {
            UserData userData(emailAddress_);
            userData._cookieList.push_back(sessionsRec_._cookieString);
            _userNameUserDataMap.insert(std::pair<std::string, UserData>(userName_, userData));
        }
        return;
    }     
    
    void ControllerData<None>::updateCookie(const SantiagoDBTables::SessionsRec& newSessionsRec_,
                                            const None&)
    {
        auto iter = _cookieStringCookieDataPtrMap.find(newSessionsRec_._cookieString);
        ST_ASSERT(iter != _cookieStringCookieDataPtrMap.end());
        iter->second->_sessionsRec = newSessionsRec_;
        return;
    }
        
    void ControllerData<unsigned>::addCookie(const std::string& userName_,
                                             const std::string& emailAddress_,
                                             const SantiagoDBTables::SessionsRec& sessionsRec_,
                                             const boost::optional<unsigned>& clientId_)
    {
        ST_ASSERT(_cookieStringCookieDataPtrMap.find(sessionsRec_._cookieString) ==
                  _cookieStringCookieDataPtrMap.end());
        CookieDataBasePtr cookieDataPtr(new ControllerData::CookieData(sessionsRec_));
        if(*clientId_)
        {
            std::static_pointer_cast<CookieData>(cookieDataPtr)->_clientIdSet.insert(*clientId_);
        }   
        _cookieStringCookieDataPtrMap[sessionsRec_._cookieString] = cookieDataPtr;
        
        auto iter = _userNameUserDataMap.find(userName_);
        if(iter != _userNameUserDataMap.end())
        {
            ST_ASSERT(iter->second._emailAddress == emailAddress_);
            iter->second._cookieList.push_back(sessionsRec_._cookieString);
        }
        else
        {
            UserData userData(emailAddress_);
            userData._cookieList.push_back(sessionsRec_._cookieString);
            _userNameUserDataMap.insert(std::pair<std::string, UserData>(userName_, userData));
        }
        return;
    }
        
    void ControllerData<unsigned>::updateCookie(const SantiagoDBTables::SessionsRec& newSessionsRec_,
                                                unsigned clientIdToBeAdded_)
    {
        auto iter = _cookieStringCookieDataPtrMap.find(newSessionsRec_._cookieString);
        ST_ASSERT(iter != _cookieStringCookieDataPtrMap.end());
        iter->second->_sessionsRec = newSessionsRec_;
        std::static_pointer_cast<CookieData>(iter->second)->_clientIdSet.insert(clientIdToBeAdded_);
        return;               
    }
        
    std::set<unsigned> ControllerData<unsigned>::getCookieClientIds(std::string& cookieString_) const
    {
        auto iter = _cookieStringCookieDataPtrMap.find(cookieString_);
        ST_ASSERT(iter != _cookieStringCookieDataPtrMap.end());

        CookieDataBasePtr cookieDataBasePtr = iter->second;
        return std::static_pointer_cast<CookieData>(cookieDataBasePtr)->_clientIdSet;
    }
        
    std::set<unsigned> ControllerData<unsigned>::getAllClientIdsForUser(const std::string& userName_) const
    {
        auto userDataIter =_userNameUserDataMap.find(userName_);
        ST_ASSERT(userDataIter != _userNameUserDataMap.end());

        std::vector<std::string> cookieList = userDataIter->second._cookieList;
        std::set<unsigned> allClientIdSets;
        
        for(std::vector<std::string>::iterator iter = cookieList.begin(); iter != cookieList.end(); ++iter)
        {
            std::string cookieString = *iter;
            CookieDataBasePtr cookieDataBasePtr = _cookieStringCookieDataPtrMap.find(cookieString)->second;
            std::set<unsigned> clientIdSet = std::static_pointer_cast<CookieData>(cookieDataBasePtr)->_clientIdSet;
            allClientIdSets.insert(clientIdSet.begin(), clientIdSet.end());
        }
        return allClientIdSets;
    }
}}
