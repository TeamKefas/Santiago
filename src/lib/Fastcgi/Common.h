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
#ifndef FASTCGI_COMMON_H
#define FASTCGI_COMMON_H
/**
 * @file Common.h
 * @author  Wilson Sunny Philip
 *
 * @section DESCRIPTION
 *
 * Contains the Common definitions.
 */

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#define MAX_REQUEST_PER_CONNECTION 1

namespace Santiago{ namespace Fastcgi
{
    typedef unsigned int        uint;
    typedef unsigned char       uchar;

    enum TransceiverEventInfo
    {
        SOCKET_CLOSED,
        CONNECTION_WIND_DOWN
    };
}}
    
#endif
