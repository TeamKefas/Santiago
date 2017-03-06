#ifndef SANTIAGO_UTILS_LOGLEVEL_H
#define SANTIAGO_UTILS_LOGLEVEL_H

#include <map>
#include <string>

namespace Santiago{ namespace Utils
{   
    enum logLevel{DEBUG, INFO, ERROR, CRITICAL, ASSERT};
    
    const std::map<logLevel, std::string> logLevelStringMap = {{DEBUG, "DEBUG"},
                                                               {INFO, "INFO"},
                                                               {ERROR, "ERROR"},
                                                               {CRITICAL, "CRITICAL"},
                                                               {ASSERT, "ASSERT"}};
}}

#endif
