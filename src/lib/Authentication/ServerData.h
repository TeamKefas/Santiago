#ifndef SANTIAGO_AUTHENTICATION_SERVERDATA_H
#define SANTIAGO_AUTHENTICATION_SERVERDATA_H

#include <map>
#include <string>
#include <vector>

namespace Santiago{ namespace Authentication
{
    struct ResourceData
    {
        std::vector<std::pair<unsigned,std::string> >  _connectionIdCookiePairList;
    };
    
    struct CookieData
    {
        std::string            _userId;
        std::vector<unsigned>  _connectionIds;
    };
    
    struct UserIdData
    {
        std::vector<std::string>  _cookieList;
    };
    
    
    struct ServerData
    {
        std::map<std::string,CookieData>                 _cookieCookieDataMap;
        std::map<std::string,UserIdData>                 _userIdUserIdDataMap;
        std::map<std::string,ResourceData>               _resourceIdResourceDataMap;
        std::map<std::string,std::vector<std::string> >  _cookieIdResourceIdMap;
    };
}}

#endif
