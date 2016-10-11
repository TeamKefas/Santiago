#include "RequestData.h"


namespace Santiago{namespace Fastcgi
{
    std::map<std::string,std::string>  parseNameValuePairs(const std::string& inString_) const
    {
        std::map<std::string, std::string> tempMap;
        std::map<std::string, std::string> escChar =
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
        
        char delimiter1 = '&', delimiter2 = ';';
        auto start = 0U;
        auto end = s.find(delimiter1);
        if(end != std::string::npos)
        {
            while(end != std::string::npos)
            {
                char delimiter = '=';
                auto inEnd = inString_.substr(start, end).find(delimiter);
                std::string str1 = inString_.substr(start, inEnd);
                std::string str2 = inString_.substr(start + inEnd + 1, end-(start+inEnd+1));
                for(std::map<std::string, std::string>::const_iterator it = escChar.begin(); it != escChar.end(); ++it)
                {
                    auto replace1 = str1.find(it->first);
                    while(replace1 != std::string::npos)
                    {
                        str1.erase(replace1, 3);
                        str1.insert(replace1, it->second);
                        auto start = replace1 + 1;
                        replace1 = str1.find(it->first, start);
                    }
                    auto replace2 = str2.find(it->first);
                    while(replace2 != std::string::npos)
                    {
                        str2.erase(replace1, 3);
                        str2.insert(replace1, it->second);
                        auto start = replace2 + 1;
                        replace2 = str2.find(it->first, start);
                    }
                }
                tempMap.insert(make_pair(str1, str2));
                start = end + 1;
                end = inString_.find(delimiter1, start);
            }
            char delimiter = '=';
            auto inEnd = inString_.substr(start, end).find(delimiter);
            std::string str1 = inString_.substr(start, inEnd);
            std::string str2 = inString_.substr(start + inEnd + 1, end-(start+inEnd+1));
            for(std::map<std::string, std::string>::const_iterator it = escChar.begin(); it != escChar.end(); ++it)
            {
                auto replace1 = str1.find(it->first);
                while(replace1 != std::string::npos)
                {
                    str1.erase(replace1, 3);
                    str1.insert(replace1, it->second);
                    auto start = replace1 + 1;
                    replace1 = str1.find(it->first, start);
                }
                auto replace2 = str2.find(it->first);
                while(replace2 != std::string::npos)
                {
                    str2.erase(replace1, 3);
                    str2.insert(replace1, it->second);
                    auto start = replace2 + 1;
                    replace2 = str2.find(it->first, start);
                }
            }
            tempMap.insert(make_pair(str1, str2));
        }
        else
        {
            auto end = inString_.find(delimiter2);
            while(end != std::string::npos)
            {
                char delimiter = '=';
                auto inEnd = inString_.substr(start, end).find(delimiter);
                std::string str1 = inString_.substr(start, inEnd);
                std::string str2 = inString_.substr(start + inEnd + 1, end-(start+inEnd+1));
                for(std::map<std::string, std::string>::const_iterator it = escChar.begin(); it != escChar.end(); ++it)
                {
                    auto replace1 = str1.find(it->first);
                    while(replace1 != std::string::npos)
                    {
                        str1.erase(replace1, 3);
                        str1.insert(replace1, it->second);
                        auto start = replace1 + 1;
                        replace1 = str1.find(it->first, start);
                    }
                    auto replace2 = str2.find(it->first);
                    while(replace2 != std::string::npos)
                    {
                        str2.erase(replace1, 3);
                        str2.insert(replace1, it->second);
                        auto start = replace2 + 1;
                        replace2 = str2.find(it->first, start);
                    }
                }
                tempMap.insert(make_pair(str1, str2));
                start = end + 1;
                end = inString_.find(delimiter2, start);
            }
            char delimiter = '=';
            auto inEnd = inString_.substr(start, end).find(delimiter);
            std::string str1 = inString_.substr(start, inEnd);
            std::string str2 = inString_.substr(start + inEnd + 1, end-(start+inEnd+1));
            for(std::map<std::string, std::string>::const_iterator it = escChar.begin(); it != escChar.end(); ++it)
            {
                auto replace1 = str1.find(it->first);
                while(replace1 != std::string::npos)
                {
                    str1.erase(replace1, 3);
                    str1.insert(replace1, it->second);
                    auto start = replace1 + 1;
                    replace1 = str1.find(it->first, start);
                }
                auto replace2 = str2.find(it->first);
                while(replace2 != std::string::npos)
                {
                    str2.erase(replace1, 3);
                    str2.insert(replace1, it->second);
                    auto start = replace2 + 1;
                    replace2 = str2.find(it->first, start);
                }
            }
        tempMap.insert(make_pair(str1, str2));
        }
        return tempMap;    
    }
    
}}
