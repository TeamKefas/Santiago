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

#ifndef SANTIAGO_LOCAL_ENDPOINT_H
#define SANTIAGO_LOCAL_ENDPOINT_H

#include <boost/asio.hpp>

namespace Santiago
{ 
    template<typename Protocol>
    struct LocalEndpoint;

    template<>
    struct LocalEndpoint<typename boost::asio::ip::tcp>
    {
        LocalEndpoint(unsigned short portNo_):
            _asioEndpoint(boost::asio::ip::tcp::v4(),portNo_)
        {}

        boost::asio::ip::tcp::endpoint getAsioEndpoint()
        {
            return _asioEndpoint;
        }

    private:
        boost::asio::ip::tcp::endpoint _asioEndpoint;
    };

    template<>
    struct LocalEndpoint<typename boost::asio::local::stream_protocol>
    {
        LocalEndpoint(const char* path_):
            _asioEndpoint(path_)
        {}

        boost::asio::local::stream_protocol::endpoint getAsioEndpoint()
        {
            return _asioEndpoint;
        }

    private:
        boost::asio::local::stream_protocol::endpoint _asioEndpoint;
    };
}
#endif
