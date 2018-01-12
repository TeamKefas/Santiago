#ifndef SANTIAGO_AUTHENTICATION_MULTINODE_SERVER_CONTROLLERDATA_H
#define SANTIAGO_AUTHENTICATION_MULTINODE_SERVER_CONTROLLERDATA_H

#include <map>
#include <memory>
#include <set>

#include <boost/optional.hpp>

#include "Santiago/Utils/STLog.h"
#include "Santiago/SantiagoDBTables/DatabaseRecords.h"

#include "Santiago/Authentication/ControllerDataBase.h"
#include "Santiago/Authentication/MultiNode/ConnectionMessage.h"

namespace Santiago{ namespace Authentication{ namespace MultiNode{ namespace Server
{
    struct ControllerTypes
    {
        struct ClientRequestData
        {
            ClientRequestData(const Authentication::MultiNode::RequestId& requestId_)
                :_requestId(requestId_)
            {}
            
            unsigned getClientId() const {return _requestId._initiatingConnectionId;}

            RequestId   _requestId;
        };
        
        typedef ControllerData LocalData;
    };

    class ControllerData:public ControllerDataBase
    {
    public:

        typedef ControllerData LocalData;
        typedef typename ControllerTypes::ClientRequestData ClientRequestData;
        typedef unsigned ClientIdType;
        struct CookieData:public CookieDataBase
        {
            CookieData(const SantiagoDBTables::SessionsRec& sessionsRec_)
                :CookieDataBase(sessionsRec_)
            {}

            std::set<ClientIdType>         _clientIdSet;
        };

        void addCookie(const std::string& userName_,
                       const std::string& emailAddress_,
                       const SantiagoDBTables::SessionsRec& sessionsRec_,
                       const boost::optional<ClientIdType>& clientId_);
        
        void updateCookie(const SantiagoDBTables::SessionsRec& newSessionsRec_,
                          ClientIdType clientIdToBeAdded_);

        std::set<ClientIdType> getCookieClientIds(const std::string& cookieString_) const;
        std::set<ClientIdType> getAllClientIdsForUser(const std::string& userName_) const;
        
    };
}}}}

#endif
