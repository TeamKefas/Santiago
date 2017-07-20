#ifndef SANTIAGO_AUTHENTICATION_CONTROLLERDATA_H
#define SANTIAGO_AUTHENTICATION_CONTROLLERDATA_H

#include <map>
#include <memory>

#include <boost/optional.hpp>

#include "Santiago/Utils/STLog.h"

#include "Santiago/SantiagoDBTables/DatabaseRecords.h"

namespace Santiago{ namespace Authentication
{
    class ControllerDataBase
    {
    public:
        
        struct UserData
        {
            UserData(const std::string& emailAddress_):
                _emailAddress(emailAddress_),
                _cookieList()
            {}

            std::string                   _emailAddress;
            std::vector<std::string>      _cookieList;
        };

        struct CookieDataBase
        {
            CookieDataBase(const SantiagoDBTables::SessionsRec& sessionsRec_)
                :_sessionsRec(sessionsRec_)
            {}

            SantiagoDBTables::SessionsRec _sessionsRec;
        };
        typedef std::shared_ptr<CookieDataBase> CookieDataBasePtr;

        Optional<SantiagoDDTables::SessionsRec> getCookieSessionsRec(const std::string& cookieString_) const;
        std::vector<std::string> getAllCookieStringsForUser(const std::string& userName_) const;
        
        void removeCookie(const std::string& cookieString_);
        void removeUser(const std::string& userName_);
        void updateUserEmailAddress(const std::string& userName_,const std::string& newEmailAddress_);
        
    protected:

        std::map<std::string, UserData>            _userNameUserDataMap;
        std::map<std::string, CookieDataBasePtr>   _cookieStringCookeDataPtrMap;
    };

    struct None
    {};
    
    template<typename ClientIdType>
    class ControllerData;

    template<>
    class ControllerData<None>:public ControllerDataBase
    {
    public:
        typedef ControllerDataBase::CookieDataBase CookieData;
        
        void addCookie(const std::string& usersName_,
                       const std::string& emailAddress_,
                       const SantiagoDBTables::SessionsRec& sessionsRec_,
                       const boost::optional<None>&);
        
        void updateCookie(const SantiagoDBTable::SessionsRec& newSessionsRec_,
                          const boost::optional<None>&) const;
        
    };

    template<>
    class ControllerData<unsigned>:public ControllerDataBase
    {
    public:
        
        struct CookieData:public CookieDataBase
        {
            CookieData(const SantiagoDBTables::SessionsRec& sessionsRec_)
                :CookieDataBase(sessionsRec_)
            {}

            std::set<unsigned>         _clientIdSet;
        };

        void addCookie(const std::string& usersName_,
                       const std::string& emailAddress_,
                       const SantiagoDBTables::SessionsRec& sessionsRec_,
                       const boost::optional<unsigned>& clientId_);
        
        void updateCookie(const SantiagoDBTable::SessionsRec& newSessionsRec_,
                          const boost::optional<unsigned>& clientIdToBeAdded_) const;

        std::set<unsigned> getCookieClientIds(std::string& cookieString_) const;
        std::set<unsigned> getAllClientIdsForUser(const std::string& userName_) const;
        
    };
}}

#endif
