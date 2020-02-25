#ifndef SANTIAGO_AUTHENTICATION_CONTROLLERDATABASE_H
#define SANTIAGO_AUTHENTICATION_CONTROLLERDATABASE_H

#include <map>
#include <memory>
#include <set>

#include <boost/optional.hpp>

#include "../Utils/STLog.h"

#include "../SantiagoDBTables/DatabaseRecords.h"

namespace Santiago{ namespace Authentication
{
    class ControllerDataBase
    {
    public:
        
        struct UserData
        {
            UserData(const std::string& emailAddress_)
                :_emailAddress(emailAddress_)
                ,_isBeingLoggedOut(false)
            {}

            std::string                   _emailAddress;
            std::vector<std::string>      _cookieList; //std::set conceptually better but looks inefficient if consider cache and small max vector size
            bool                          _isBeingLoggedOut;
        };

        struct CookieDataBase
        {
            CookieDataBase(const SantiagoDBTables::SessionsRec& sessionsRec_)
                :_sessionsRec(sessionsRec_)
                ,_isBeingLoggedOut(false)
            {}

            SantiagoDBTables::SessionsRec _sessionsRec;
            bool                          _isBeingLoggedOut;
        };
        typedef std::shared_ptr<CookieDataBase> CookieDataBasePtr;

        ControllerDataBase() = default;
        ~ControllerDataBase() = default;
        
        boost::optional<SantiagoDBTables::SessionsRec> getCookieSessionsRec(const std::string& cookieString_) const;
        std::vector<std::string> getAllCookieStringsForUser(const std::string& userName_) const;
        boost::optional<std::string> getUserEmailAddress(const std::string& userName_) const;
        void updateUserEmailAddress(const std::string& userName_,const std::string& newEmailAddress_);

        /* Logout out conditions Controller/ControllerData should satisfy
           1) logoutCookie will not be processed when logoutUser already in progess.
           2) logoutUser will not be be processed if ALL the cookies of user has a 
              logoutCookie in progress.
           3) For each user the commands will be processed in 1 strand and each client
              processes/replies to logoutCookie/ClientFromUser in order of commands.
           4) Each client (including client receiving user commands) removes cookie/user
              only after global controller isBeingLoggedOut flag is set.
           5) loginUserForCookie and verifyCookie not allowed when logoutUser already in
              progress (or for verifyCookie it is blocked even if logoutCookie in progress).
         */

        /* Notes:
           1) asserts to see cookieData _isBeingLoggedOut is already set.
           2) if last cookie of user, asserts to see userData _isBeingLoggedOut NOT already
              set before deleting userData.
         */
        void removeCookie(const std::string& cookieString_);

        /* Notes:
           1) asserts to see userData _isBeingLoggedOut is already set.
           2) asserts to see none of the cookie has cookieData._isBeingLoggedOut set.
         */
        void removeUser(const std::string& userName_);

        /* Notes:
           1) asserts to see cookieData._isBeingLoggedOut not already set.
           2) asserts to see userData._isBeingLoggedOut not already set.
         */
        void setCookieBeingLoggedOutFlag(const std::string& cookieString_);
        void unsetCookieBeingLoggedOutFlag(const std::string& cookieString_);
        bool isCookieBeingLoggedOut(const std::string& cookieString_) const;

        /* Notes:
           1) asserts to see userData._isBeingLoggedOut not already set.
           2) asserts to see that ALL of users cookieData._isBeingLoggedOut not already set.
         */
        void setUserBeingLoggedOutFlag(const std::string& userName_);
        void unsetUserBeingLoggedOutFlag(const std::string& userName_);        
        bool isUserBeingLoggedOut(const std::string& userName_) const;
        
        
    protected:

        /* Notes
           1) Asserts to see userData._isBeingLoggedOut not set.
        */
        void addCookieImpl(const std::string& userName_,
                           const std::string& emailAddress_,
                           const CookieDataBasePtr& cookieDataPtr_);

        virtual void removeCookieClientDataImpl(const CookieDataBasePtr& cookieDataPtr_) = 0;
        std::map<std::string, UserData>            _userNameUserDataMap;
        std::map<std::string, CookieDataBasePtr>   _cookieStringCookieDataPtrMap;
    };

    struct Nil
    {};
    

    class ControllerData:public ControllerDataBase
    {
    public:

        ControllerData() = default;
        ~ControllerData() = default;
        
        typedef unsigned ClientIdType;
        struct CookieData:public CookieDataBase
        {
            CookieData(const SantiagoDBTables::SessionsRec& sessionsRec_)
                :CookieDataBase(sessionsRec_)
            {}

            std::vector<ClientIdType>     _clientIdList; //std::set conceptually better but looks inefficient if consider cache and small max vector size
        };

        //see ControllerDataBase::addCookieImpl
        void addCookie(const std::string& userName_,
                       const std::string& emailAddress_,
                       const SantiagoDBTables::SessionsRec& sessionsRec_,
                       const boost::optional<ClientIdType>& clientId_);

        /* Notes
           1) If adding client asserts to see cookieData._isBeingLoggedOut not set.
           2) If adding client asserts to see userData._isBeingLoggedOut not set.
         */
        void updateCookieAndAddClient(const SantiagoDBTables::SessionsRec& newSessionsRec_,
                                      ClientIdType clientId_);

        std::vector<ClientIdType> getCookieClientIds(const std::string& cookieString_) const;
        std::set<ClientIdType> getAllClientIdsForUser(const std::string& userName_) const;

    protected:

        void removeCookieClientDataImpl(const CookieDataBasePtr& cookieDataPtr_);

        std::map<ClientIdType,std::vector<std::string> >  _clientIdCookieListMap;
        
    };
}}

#endif
