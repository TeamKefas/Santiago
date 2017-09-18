#ifndef SANTIAGO_AUTHENTICATION_SINGLENODE_CONTROLLERDATA_H
#define SANTIAGO_AUTHENTICATION_SINGLENODE_CONTROLLERDATA_H

#include <map>
#include <memory>
#include <set>

#include <boost/optional.hpp>

#include "Santiago/Utils/STLog.h"

#include "Santiago/SantiagoDBTables/DatabaseRecords.h"

namespace Santiago{ namespace Authentication{ namespace SingleNode
{
    struct None
    {};
    
    class ControllerData:public ControllerDataBase
    {
    public:
        typedef None ClientIdType;
        typedef ControllerDataBase::CookieDataBase CookieData;
        
        void addCookie(const std::string& userName_,
                       const std::string& emailAddress_,
                       const SantiagoDBTables::SessionsRec& sessionsRec_,
                       const boost::optional<ClientIdType>&);
        
        void updateCookie(const SantiagoDBTables::SessionsRec& newSessionsRec_,
                          const ClientIdType&);
        
    };
}}}

#endif
