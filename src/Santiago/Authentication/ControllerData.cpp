#include "ControllerData.h"

namespace Santiago{ namespace Authentication
    {
        boost::optional<SantiagoDBTables::SessionsRec> ControllerDataBase::getCookieSessionsRec(const std::string& cookieString_) const
        {
            if(_cookieStringCookieDataPtrMap.find(cookieString_) != _cookieStringCookieDataPtrMap.end())
            {
                return _cookieStringCookieDataPtrMap.find(cookieString_)->second->_sessionsRec;
            }
            return SantiagoDBTables::SessionsRec();
        }
    
        std::vector<std::string> ControllerDataBase::getAllCookieStringsForUser(const std::string& userName_) const
        {
            if(_userNameUserDataMap.find(userName_) != _userNameUserDataMap.end())
            {
                return _userNameUserDataMap.find(userName_)->second._cookieList;
            }
            return {};
        }

        void ControllerDataBase::removeCookie(const std::string& cookieString_)
        {
            if(_cookieStringCookieDataPtrMap.find(cookieString_) != _cookieStringCookieDataPtrMap.end())
            {
                std::string userName = _cookieStringCookieDataPtrMap.find(cookieString_)->second->_sessionsRec._userName;
                _cookieStringCookieDataPtrMap.erase(cookieString_);
                std::vector<std::string>::iterator iter;
                for(iter = _userNameUserDataMap.find(userName)->second._cookieList.begin(); iter != _userNameUserDataMap.find(userName)->second._cookieList.end(); ++iter)
                {
                    if(*iter == cookieString_)
                    {
                        _userNameUserDataMap.find(userName)->second._cookieList.erase(iter);
                        if(!_userNameUserDataMap.find(userName)->second._cookieList.size())
                        {
                            removeUser(userName);
                        }
                    }
                }
            }
            return;
        }
    
        void ControllerDataBase::removeUser(const std::string& userName_)
        {
            if(_userNameUserDataMap.find(userName_) != _userNameUserDataMap.end())
            {
                _userNameUserDataMap.erase(userName_);
            }
            return;
        }

        void ControllerDataBase::updateUserEmailAddress(const std::string& userName_, const std::string& newEmailAddress_)
        {
            if(_userNameUserDataMap.find(userName_) != _userNameUserDataMap.end())
            {
                _userNameUserDataMap.find(userName_)->second._emailAddress = newEmailAddress_;
            }
            return;
        }
      
        void ControllerData<None>::addCookie(const std::string& userName_,
                                             const std::string& emailAddress_,
                                             const SantiagoDBTables::SessionsRec& sessionsRec_,
                                             const boost::optional<None>&)
        {
            if(_userNameUserDataMap.find(userName_) != _userNameUserDataMap.end())
            {
                _userNameUserDataMap.find(userName_)->second._emailAddress = emailAddress_;
                _userNameUserDataMap.find(userName_)->second._cookieList.push_back(sessionsRec_._cookieString);
            }
            else
            {
                UserData userData(emailAddress_);
                userData._cookieList.push_back(sessionsRec_._cookieString);
                _userNameUserDataMap.insert(std::pair<std::string, UserData>(userName_, userData));
            }
            CookieData cookieData(sessionsRec_);
            if(_cookieStringCookieDataPtrMap.find(sessionsRec_._cookieString) != _cookieStringCookieDataPtrMap.end())
            {
                _cookieStringCookieDataPtrMap.find(sessionsRec_._cookieString)->second.reset(new ControllerData::CookieData(cookieData));
            }
            else
            {                
                std::shared_ptr<ControllerDataBase::CookieDataBase> cookieDataPtr(new ControllerData::CookieData(cookieData));
                _cookieStringCookieDataPtrMap[sessionsRec_._cookieString] = cookieDataPtr;
            }
            return;
        }     
       
        void ControllerData<None>::updateCookie(const SantiagoDBTables::SessionsRec& newSessionsRec_,
                                                const boost::optional<None>&)
        {
            if(_cookieStringCookieDataPtrMap.find(newSessionsRec_._cookieString) != _cookieStringCookieDataPtrMap.end())
            {
                CookieData cookieData(newSessionsRec_);
                _cookieStringCookieDataPtrMap.find(newSessionsRec_._cookieString)->second.reset(new ControllerData::CookieData(cookieData));
            }
            return;
        }
        
        void ControllerData<unsigned>::addCookie(const std::string& userName_,
                                                 const std::string& emailAddress_,
                                                 const SantiagoDBTables::SessionsRec& sessionsRec_,
                                                 const boost::optional<unsigned>& clientId_)
        {
            if(_userNameUserDataMap.find(userName_) != _userNameUserDataMap.end())
            {
                _userNameUserDataMap.find(userName_)->second._emailAddress = emailAddress_;
                _userNameUserDataMap.find(userName_)->second._cookieList.push_back(sessionsRec_._cookieString);
            }
            else
            {
                UserData userData(emailAddress_);
                userData._cookieList.push_back(sessionsRec_._cookieString);
                _userNameUserDataMap.insert(std::pair<std::string, UserData>(userName_, userData));
            }
            CookieData cookieData(sessionsRec_);
            cookieData._clientIdSet.insert(*clientId_);
            if(_cookieStringCookieDataPtrMap.find(sessionsRec_._cookieString) != _cookieStringCookieDataPtrMap.end())
            {
                _cookieStringCookieDataPtrMap.find(sessionsRec_._cookieString)->second.reset(new ControllerData::CookieData(cookieData));
            }
            else
            {
                std::shared_ptr<ControllerDataBase::CookieDataBase> cookieDataPtr(new ControllerData::CookieData(cookieData));
                _cookieStringCookieDataPtrMap[sessionsRec_._cookieString] = cookieDataPtr;
            }
            return;
        }
        
        void ControllerData<unsigned>::updateCookie(const SantiagoDBTables::SessionsRec& newSessionsRec_,
                                                              const boost::optional<unsigned>& clientIdToBeAdded_)
        {
            if(_cookieStringCookieDataPtrMap.find(newSessionsRec_._cookieString) != _cookieStringCookieDataPtrMap.end())
            {
                CookieData cookieData(newSessionsRec_);
                cookieData._clientIdSet.insert(*clientIdToBeAdded_);
                _cookieStringCookieDataPtrMap.find(newSessionsRec_._cookieString)->second.reset(new ControllerData::CookieData(cookieData));
            }
            return;
        }
        
        std::set<unsigned> ControllerData<unsigned>::getCookieClientIds(std::string& cookieString_) const
        {
            if(_cookieStringCookieDataPtrMap.find(cookieString_) != _cookieStringCookieDataPtrMap.end())
            {
                CookieDataBasePtr cookieDataBasePtr = _cookieStringCookieDataPtrMap.find(cookieString_)->second;
                return std::static_pointer_cast<CookieData>(cookieDataBasePtr)->_clientIdSet;
            }
            return std::set<unsigned>();
        }
        
        std::set<unsigned> ControllerData<unsigned>::getAllClientIdsForUser(const std::string& userName_) const
        {
            if(_userNameUserDataMap.find(userName_) != _userNameUserDataMap.end())
            {
                std::vector<std::string> cookieList = _userNameUserDataMap.find(userName_)->second._cookieList;
                std::set<unsigned> allClientIdSets;
                std::vector<std::string>::iterator iter;
                for(iter = cookieList.begin(); iter != cookieList.end(); ++iter)
                {
                    std::string cookieString = *iter;
                    CookieDataBasePtr cookieDataBasePtr = _cookieStringCookieDataPtrMap.find(cookieString)->second;
                    std::set<unsigned> clientIdSet = std::static_pointer_cast<CookieData>(cookieDataBasePtr)->_clientIdSet;
                    allClientIdSets.insert(clientIdSet.begin(), clientIdSet.end());
                }
                return allClientIdSets;
            }
            return std::set<unsigned>();
        }
}}
