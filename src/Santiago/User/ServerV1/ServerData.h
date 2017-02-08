#ifndef SANTIAGO_USER_SERVER_SERVERDATA_H
#define SANTIAGO_USER_SERVER_SERVERDATA_H

/**
 * @file ServerData.h
 *
 * @section DESCRIPTION
 *
 * Contains the ResourceData CookieData UserIdData ServerData
 */

#include <map>
#include <string>
#include <vector>

namespace Santiago{ namespace User { namespace Server
{
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
    };
}}}

#endif
