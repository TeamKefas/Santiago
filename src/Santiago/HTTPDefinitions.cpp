#include "HTTPDefinitions.h"

namespace Santiago
{
    namespace
    {
        std::map<MIMEType,std::string>  mimeTypeStringMap
        {
            {MIMEType::TEXT,"text/plain"},
            {MIMEType::HTML,"text/html"},
            {MIMEType::XML,"application/xml"},
            {MIMEType::JSON,"application/json"},
            {MIMEType::PDF,"application/pdf"}
        };

        std::pair<std::string, std::string> MakeNameValuePairs(const std::string &inString_,
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
            for(std::map<std::string, std::string>::const_iterator it =
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

            if(str1[0]==' ')
            {
		str1.erase(0,1);
            }
            if(str2[0]==' ')
            {
                str2.erase(0,1);
            }
            return make_pair(str1, str2);   
        }

    }

    const std::map<MIMEType,std::string>& GetMIMETypeStringMap(){return mimeTypeStringMap;}

    bool operator < (const HTTPCookieData& lhs_ , const HTTPCookieData& rhs_) {return (lhs_._name < rhs_._name);}

    std::string HTTPCookieData::getSetCookieHeaderString() const
    {
        std::string cookieString;
        cookieString.append("Set-cookie:");
        cookieString.append(_name);
        cookieString.append("=");
        cookieString.append(_value);
        if(_expiryTime)
        {
            cookieString.append("; ");
            cookieString.append("Expires=");
            std::stringstream stream;
            stream << *_expiryTime;
            cookieString.append(stream.str());
        }
        if(_domain)
        {
            cookieString.append("; ");
            cookieString.append("Domain=");
            cookieString.append(*_domain);
        }
        if(_path)
        {
            cookieString.append("; ");
            cookieString.append("Path=");
            cookieString.append(*_path);
        }
        if(_isSecure)
        {
            cookieString.append("; ");
            cookieString.append("Secure");
        }
        if(_isHTTPOnly)
        {
            cookieString.append("; ");
            cookieString.append("HTTPOnly");
        }
        cookieString.append("\n");
        return cookieString;
    }


    std::map<std::string,std::string> ParseGetPostNameValuePairs(const std::string& inString_)
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
            std::pair<std::string, std::string> strPair = MakeNameValuePairs(inString_, start, end);
            nameValuePairMap.insert(strPair);
            start = end + 1;
            end = inString_.find(delimiter, start);
        }
        std::pair<std::string, std::string> strPair = MakeNameValuePairs(inString_, start, end);
        nameValuePairMap.insert(strPair);
        return nameValuePairMap;    
    }

}
