/*
    Copyright 2012, 2013 Wilson Sunny Philip [wil.pazhorickal<at>gmail<dot>com]

    This file is part of the Santiago library.

    Santiago is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Santiago is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with Santiago.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef FASTCGI_REQUEST_DATA_H
#define FASTCGI_REQUEST_DATA_H
/**
 * @file RequestData.h
 * @author  Wilson Sunny Philip
 *
 * @section DESCRIPTION
 *
 * Contains the RequestData class.
 */

#include <string>
#include <map>
#include <ostream>

#include <boost/asio.hpp>
#include <boost/asio/streambuf.hpp>


#include "Common.h"
#include "PairParseFunctions.h"

namespace Santiago{namespace Fastcgi
{
/**
 * Thic structure stores the input (from web network) and to be output (from the user
 * app) data of a request.
 */
    struct RequestData
    {
        typedef std::map<std::string,std::string> ParamsMap;
        ParamsMap               _paramsMap;
        std::string             _paramsBuffer;

        std::string             _in;
        
        boost::asio::streambuf  _outBuffer;
        boost::asio::streambuf  _errBuffer;
        std::ostream            _out;
        std::ostream            _err;
        int                     _appStatus;

        /**
         *The constructor
         */
        RequestData():_out(&_outBuffer),_err(&_errBuffer)
        {}

        /**
         *Parse the data in param buffer and store it as map
         */
        void parseFCGIParams()
        {
            ParsePairs(_paramsBuffer.data(),_paramsBuffer.size(),_paramsMap);
        }        
    };
}}

#endif
