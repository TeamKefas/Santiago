#include "RequestData.h"


namespace Santiago{namespace Fastcgi
{
    std::pair<std::string, std::string> RequestData::makeNameValuePairs(std::string &inString_,
                                                                        std::size_t start, std::size_t end)
    {
        std::map<std::string, std::string> escCharMap =
            {{"%20", " "},
             {"%23", "#"},
             {"%24", "$"},
             {"%25", "%"},
             {"%26", "&"},
             {"%40", "@"},
             {"%60", "`"},
             {"%2F", "/"},
             {"%3A", ":"},
             {"%3B", ";"},
             {"%3C", "<"},
             {"%3D", "="},
             {"%3E", ">"},
             {"%3F", "?"},
             {"%5B", "["},
             {"%5C", "\\"},
             {"%5D", "]"},
             {"%5E", "^"},
             {"%7B", "{"},
             {"%7C", "|"},
             {"%7D", "}"},
             {"%7E", "~"},
             {"%22", "\""},
             {"%27", "'"},
             {"%2B", "+"},
             {"%2C", "-"}};
        
        char delimiter = '=';
        auto newEnd = inString_.substr(start, end).find(delimiter);
        std::string str1 = inString_.substr(start, newEnd);
        std::string str2 = inString_.substr(start + newEnd + 1, end - (start + newEnd + 1));
        for(std::map<std::std::ring, std::string>::const_iterator it =
                escCharMap.begin(); it != escCharMap.end(); ++it)
        {
            auto str1ReplacePos = str1.find(it->first);
            while(str1ReplacePos != std::string::npos)
            {
                str1.erase(str1ReplacePos, 3);
                str1.insert(str1ReplacePos, it->second);
                auto start = str1ReplacePos + 1;
                str1ReplacePos = str1.find(it->first, start);
            }
            auto str2ReplacePos = str2.find(it->first);
            while(str2ReplacePos != std::string::npos)
            {
                str2.erase(str2ReplacePos, 3);
                str2.insert(str2ReplacePos, it->second);
                auto start = str2ReplacePos + 1;
                str2ReplacePos = str2.find(it->first, start);
            }
        }
        return make_pair(str1, str2);   
    }

    std::map<std::string,std::string> RequestData::parseNameValuePairs(const std::string& inString_) const
    {
        std::map<std::string, std::string> nameValuePairMap;
        char delimiter;
        auto start = 0U;
        auto test = inString_.find('&');
        if(test != std::string::npos)
        {
            delimiter = '&';
        }
        else
        {
            delimiter = ';';
        }
        auto end = inString_.find(delimiter);
        while(end != std::string::npos)
        {
            std::pair<std::string, std::string> strPair = makeNameValuePairs(inString_, start, end);
            nameValuePairMap.insert(strPair);
            start = end + 1;
            end = inString_.find(delimiter, start);
        }
        std::pair<std::string, std::string> strPair = makeNameValuePairs(inString_, start, end);
        nameValuePairMap.insert(strPair);
        return nameValuePairMap;    
    }

    std::string HTTPCookieData::getCookieHeaderString() const
    {
        std::string cookieString;
        cookieString.append("Set-cookie: ");
        cookieString.append(_name);
        cookieString.append("=");
        cookieString.append(_value);
        if(_expiryTime != NULL)
        {
            cookieString.append("; ");
            cookieString.append("Expires=");
            cookieString.append(_expiryTime);
        }
        if(_domain != NULL)
        {
            cookieString.append("; ");
            cookieString.append("Domain=");
            cookieString.append(_domain);
        }
        if(_path != NULL)
        {
            cookieString.append("; ");
            cookieString.append("Path=");
            cookieString.append(_path);
        }
        if(_isSecure)
        {
            cookieString.append("; ");
            cookieString.append("Secure");
        }
        if(_isHTTPOnly)
        {
            cookieString.append("; ");
            cookieString.append(HTTPOnly);
        }
        return cookieString;
    }
       
}}
