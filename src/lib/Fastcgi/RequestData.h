#ifndef FASTCGI_REQUEST_DATA_H
#define FASTCGI_REQUEST_DATA_H
/**
 * @file RequestData.h
 *
 * @section DESCRIPTION
 *
 * Contains the RequestData class.
 */

#include <string>
#include <set>
#include <ostream>
#include <sstream>

#include <boost/asio.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "Common.h"
#include "PairParseFunctions.h"

namespace Santiago{namespace Fastcgi
{
    enum class MIMEType
    {
        TEXT,
        HTML,
        XML
    };


/**
 * Thic structure represents 1 HTTP cookie to be sent to the client
 */
    struct HTTPCookieData
    {
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
    


/**
 * Thic structure stores the input (from web network) and to be output (from the user
 * app) data of a request.
 */
    struct RequestData
    {

//        static const std::map<MIMEType,std::string>& getMIMETypeStringMap();

        typedef std::map<std::string,std::string> ParamsMap;
        typedef std::weak_ptr<IsRequestAlive> IsRequestAliveWeakPtr;

        IsRequestAliveWeakPtr   _isRequestAliveWeakPtr;
        ParamsMap               _paramsMap;
        std::string             _paramsBuffer;

        std::string             _in;
        
        boost::asio::streambuf  _outBuffer;
        boost::asio::streambuf  _errBuffer;

        std::ostream            _out;
        std::ostream            _err;
        int                     _appStatus;

        std::map<std::string, std::string>     _requestGetData;
        std::map<std::string, std::string>     _requestPostData;
        std::map<std::string, std::string>     _requestHTTPCookies;

        MIMEType                               _responseContentType;
        std::set<HTTPCookieData>               _responseHTTPCookies;

        std::map<MIMEType,std::string>         _mimeTypeStringMap;

        /**
         *The constructor
         */
        RequestData(const IsRequestAliveWeakPtr& isRequestAliveWeakPtr_):
            _isRequestAliveWeakPtr(isRequestAliveWeakPtr_),
            _out(&_outBuffer),
            _err(&_errBuffer),
            _mimeTypeStringMap{{MIMEType::TEXT,"text/plain"},
                               {MIMEType::HTML,"text/html"}}
        {           
        }

        /**
         *Parse the data in param buffer and store it as map
         */
        void parseFCGIParams()
        {
            ParsePairs(_paramsBuffer.data(),_paramsBuffer.size(),_paramsMap);
            parseRequestGetData();
            parseRequestPostData();
            parseRequestHTTPCookies();
        }

        std::pair<std::string, std::string> makeNameValuePairs(const std::string &inString_,
                                                               std::size_t start, std::size_t end) const;

        std::map<std::string,std::string> parseNameValuePairs(const std::string& inString_) const;

        void parseRequestGetData()
        {
            _requestGetData = parseNameValuePairs(_paramsMap["QUERY_STRING"]);
        }

        void parseRequestPostData() 
        {
            _requestPostData = parseNameValuePairs(_in);
        }

        void parseRequestHTTPCookies()
        {
            _requestHTTPCookies = parseNameValuePairs(_paramsMap["HTTP_COOKIE"]);
        }

        void fillHTTPHeaderData(std::ostream& out_) const
        {
            for(std::set<HTTPCookieData>::const_iterator iter = _responseHTTPCookies.begin();
                iter != _responseHTTPCookies.end();
                ++iter)
            {
                out_<<iter->getSetCookieHeaderString();
            }

            out_<<"Content-Type: "<<_mimeTypeStringMap.find(_responseContentType)->second<<"\r\n\r\n";
            std::cout<<boost::asio::buffer_cast<const char*>(static_cast<boost::asio::streambuf*>(out_.rdbuf())->data());
            //std::stringstream ss;
            //ss << out_.rdbuf();
            //std::cout<<ss.str();
            
        }
        

    };



}}

#endif
