#include "ControllerData.h"

namespace Santiago{ namespace Authentication{ namespace SingleNode
{
    void ControllerData::addCookie(const std::string& userName_,
                                   const std::string& emailAddress_,
                                   const SantiagoDBTables::SessionsRec& sessionsRec_,
                                   const boost::optional<ClientIdType>&)
    {
        CookieDataBasePtr cookieDataPtr(new ControllerData::CookieData(sessionsRec_));
        addCookieImpl(userName_,emailAddress_,cookieDataPtr);
        return;
    }
    
    void ControllerData::updateCookieAndAddClient(const SantiagoDBTables::SessionsRec& newSessionsRec_,
                                                  const ClientIdType&)
    {
        auto cookieDataIter = _cookieStringCookieDataPtrMap.find(newSessionsRec_._cookieString);
        ST_ASSERT(cookieDataIter != _cookieStringCookieDataPtrMap.end());
        ST_ASSERT(!cookieDataIter->second->_isBeingLoggedOut);

#ifndef NDEBUG_
        auto userDataIter = _userNameUserDataMap.find(cookieDataIter->second->_sessionsRec._userName);
        ST_ASSERT(userDataIter != _userNameUserDataMap.end());
        ST_ASSERT(!userDataIter->second._isBeingLoggedOut);
#endif
        
        cookieDataIter->second->_sessionsRec = newSessionsRec_;
        return;
    }
}}}
