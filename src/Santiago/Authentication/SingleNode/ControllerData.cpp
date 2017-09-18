#include "ControllerData.h"

namespace Santiago{ namespace Authentication{ namespace SingleNode
{
    void ControllerData::addCookie(const std::string& userName_,
                                   const std::string& emailAddress_,
                                   const SantiagoDBTables::SessionsRec& sessionsRec_,
                                   const boost::optional<ClientIdType>&)
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
    
    void ControllerData::updateCookie(const SantiagoDBTables::SessionsRec& newSessionsRec_,
                                      const ClientIdType&)
    {
        auto iter = _cookieStringCookieDataPtrMap.find(newSessionsRec_._cookieString);
        ST_ASSERT(iter != _cookieStringCookieDataPtrMap.end());
        iter->second->_sessionsRec = newSessionsRec_;
        return;
    }
}}}
