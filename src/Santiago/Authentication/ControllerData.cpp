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
            if(iter != _userNameUserDataMap.end())
            {
                return iter->second._cookieList;
            }
            return std::vector<std::string>();
        }

        void ControllerDataBase::removeCookie(const std::string& cookieString_)
        {
            auto iter = _cookieStringCookieDataPtrMap.find(cookieString_); 
            if(iter == _cookieStringCookieDataPtrMap.end())
            {
                ST_ASSERT(false);
            }
            std::string userName = iter->second->_sessionsRec._userName;
            _cookieStringCookieDataPtrMap.erase(iter);
            auto itr = _userNameUserDataMap.find(userName);
            for(auto it = itr->second._cookieList.begin(); it != itr->second._cookieList.end(); ++it)
            {
                if(*it == cookieString_)
                {
                    itr->second._cookieList.erase(it);
                    if(!itr->second._cookieList.size())
                    {
                        removeUser(userName);
                    }
                }
            }
            return;
        }
    
        void ControllerDataBase::removeUser(const std::string& userName_)
        {
            auto iter = _userNameUserDataMap.find(userName_);
            if(iter == _userNameUserDataMap.end())
            {
                ST_ASSERT(false);
            }
            std::vector<std::string> cookieList = iter->second._cookieList;
            std::vector<std::string>::iterator itr;
            for(itr = cookieList.begin(); itr != cookieList.end(); ++itr)
            {
                _cookieStringCookieDataPtrMap.erase(*itr);
            }
            _userNameUserDataMap.erase(iter);
            return;
        }

        void ControllerDataBase::updateUserEmailAddress(const std::string& userName_, const std::string& newEmailAddress_)
        {
            auto iter = _userNameUserDataMap.find(userName_);
            if(iter == _userNameUserDataMap.end())
            {
                ST_ASSERT(false);
            }
            iter->second._emailAddress = newEmailAddress_;
            return;
        }
      
        void ControllerData<None>::addCookie(const std::string& userName_,
                                             const std::string& emailAddress_,
                                             const SantiagoDBTables::SessionsRec& sessionsRec_,
                                             const boost::optional<None>&)
        {
            auto itr = _cookieStringCookieDataPtrMap.find(sessionsRec_._cookieString);
            if(itr != _cookieStringCookieDataPtrMap.end())
            {
                ST_ASSERT(false);
            }
            auto iter = _userNameUserDataMap.find(userName_);
            if(iter != _userNameUserDataMap.end())
            {
                iter->second._emailAddress = emailAddress_;
                iter->second._cookieList.push_back(sessionsRec_._cookieString);
            }
            else
            {
                UserData userData(emailAddress_);
                userData._cookieList.push_back(sessionsRec_._cookieString);
                _userNameUserDataMap.insert(std::pair<std::string, UserData>(userName_, userData));
            }
            CookieData cookieData(sessionsRec_);            
            CookieDataBasePtr cookieDataPtr(new ControllerData::CookieData(cookieData));
            _cookieStringCookieDataPtrMap[sessionsRec_._cookieString] = cookieDataPtr;
            return;
        }     
       
        void ControllerData<None>::updateCookie(const SantiagoDBTables::SessionsRec& newSessionsRec_,
                                                const boost::optional<None>&)
        {
            auto iter = _cookieStringCookieDataPtrMap.find(newSessionsRec_._cookieString);
            if(iter == _cookieStringCookieDataPtrMap.end())
            {
                ST_ASSERT(false);
            }
            iter->second->_sessionsRec = newSessionsRec_;
            return;
        }
        
        void ControllerData<unsigned>::addCookie(const std::string& userName_,
                                                 const std::string& emailAddress_,
                                                 const SantiagoDBTables::SessionsRec& sessionsRec_,
                                                 const boost::optional<unsigned>& clientId_)
        {
            auto itr = _cookieStringCookieDataPtrMap.find(sessionsRec_._cookieString);
            if(itr != _cookieStringCookieDataPtrMap.end())
            {
                ST_ASSERT(false);
            }
            auto iter = _userNameUserDataMap.find(userName_);
            if(iter != _userNameUserDataMap.end())
            {
                iter->second._emailAddress = emailAddress_;
                iter->second._cookieList.push_back(sessionsRec_._cookieString);
            }
            else
            {
                UserData userData(emailAddress_);
                userData._cookieList.push_back(sessionsRec_._cookieString);
                _userNameUserDataMap.insert(std::pair<std::string, UserData>(userName_, userData));
            }
            CookieData cookieData(sessionsRec_);
            cookieData._clientIdSet.insert(*clientId_);              
            CookieDataBasePtr cookieDataPtr(new ControllerData::CookieData(cookieData));
            _cookieStringCookieDataPtrMap[sessionsRec_._cookieString] = cookieDataPtr;
            return;
        }
        
        void ControllerData<unsigned>::updateCookie(const SantiagoDBTables::SessionsRec& newSessionsRec_,
                                                    const boost::optional<unsigned>& clientIdToBeAdded_)
        {
            auto iter = _cookieStringCookieDataPtrMap.find(newSessionsRec_._cookieString);
            if(iter == _cookieStringCookieDataPtrMap.end())
            {
                ST_ASSERT(false);
            }
            iter->second->_sessionsRec = newSessionsRec_;
            std::static_pointer_cast<CookieData>(iter->second)->_clientIdSet.insert(*clientIdToBeAdded_);
            return;               
        }
        
        std::set<unsigned> ControllerData<unsigned>::getCookieClientIds(std::string& cookieString_) const
        {
            auto iter = _cookieStringCookieDataPtrMap.find(cookieString_);
            if(iter == _cookieStringCookieDataPtrMap.end())
            {
                ST_ASSERT(false);
            }
            CookieDataBasePtr cookieDataBasePtr = iter->second;
            return std::static_pointer_cast<CookieData>(cookieDataBasePtr)->_clientIdSet;
        }
        
        std::set<unsigned> ControllerData<unsigned>::getAllClientIdsForUser(const std::string& userName_) const
        {
            auto iter =_userNameUserDataMap.find(userName_);
            if(iter == _userNameUserDataMap.end())
            {
                ST_ASSERT(false);
            }
            std::vector<std::string> cookieList = iter->second._cookieList;
            std::set<unsigned> allClientIdSets;
            std::vector<std::string>::iterator itr;
            for(itr = cookieList.begin(); itr != cookieList.end(); ++itr)
            {
                std::string cookieString = *itr;
                CookieDataBasePtr cookieDataBasePtr = _cookieStringCookieDataPtrMap.find(cookieString)->second;
                std::set<unsigned> clientIdSet = std::static_pointer_cast<CookieData>(cookieDataBasePtr)->_clientIdSet;
                allClientIdSets.insert(clientIdSet.begin(), clientIdSet.end());
            }
            return allClientIdSets;
        }
}}
