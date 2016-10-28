#ifndef SANTIAGO_HTTPDEFINITIONS_H
#define SANTIAGO_HTTPDEFINITIONS_H

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/optional.hpp>

namespace Santiago
{

/**
 * Enum for MIME type
 */
    enum class MIMEType
    {
        TEXT,
        HTML,
        XML,
	JSON
    };

    const std::map<MIMEType,std::string>& GetMIMETypeStringMap();

/**
 * Thic structure represents 1 HTTP cookie to be sent to the client
 */
    struct HTTPCookieData
    {
        HTTPCookieData():
        _isSecure(false),
        _isHTTPOnly(false)
        {}


        std::string                                     _name;
        std::string                                     _value;
        boost::optional<boost::posix_time::ptime>       _expiryTime;
        boost::optional<std::string>                    _domain;
        boost::optional<std::string>                    _path;
        bool                                            _isSecure;
        bool                                            _isHTTPOnly;

        std::string    getSetCookieHeaderString() const; //TODO
    };

    bool operator < (const HTTPCookieData& lhs_ , const HTTPCookieData& rhs_);

    std::map<std::string,std::string> ParseGetPostNameValuePairs(const std::string& inString_);

}//closing namespace Santiago

#endif
