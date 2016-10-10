#include "RequestData.h"


namespace Santiago{namespace Fastcgi
{
    std::map<std::string,std::string>  parseNameValuePairs(const std::string& inString_) const
    {
        std::map<std::string,std::string> tempMap;
        char delimiter1 = '&', delimiter2 = ';';
        auto start = 0U;
        auto end = inString_.find(delimiter1);
        if(end != std::string::npos)
        {
            while(end != std::string::npos)
            {
                char delimiter = '=';
                auto inEnd = inString_.substr(start, end).find(delimiter);
                tempMap.insert(make_pair(inString_.substr(start, inEnd), inString_.substr(start + inEnd + 1, end-(start+inEnd+1))));
                start = end + 1;
                end = inString_.find(delimiter1, start);
            }
            char delimiter = '=';
            auto inEnd = inString_.substr(start, end).find(delimiter);
            tempMap.insert(make_pair(inString_.substr(start, inEnd), inString_.substr(start + inEnd + 1, end-(start+inEnd+1))));
        }
        else
        {
            auto end = inString_.find(delimiter2);
            while(end != std::string::npos)
            {
                char delimiter = '=';
                auto inEnd = inString_.substr(start, end).find(delimiter);
                tempMap.insert(make_pair(inString_.substr(start, inEnd), inString_.substr(start + inEnd + 1, end-(start+inEnd+1))));
                start = end + 1;end = inString_.find(delimiter2, start);
            }
            char delimiter = '=';auto inEnd = inString_.substr(start, end).find(delimiter);
            tempMap.insert(make_pair(inString_.substr(start, inEnd), inString_.substr(start + inEnd + 1, end-(start+inEnd+1))));}
    }

    return tempMap;
    
}}
