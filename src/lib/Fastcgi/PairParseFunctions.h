#ifndef FASTCGI_PARSE_PAIRS_H
#define FASTCGI_PARSE_PAIRS_H
/**
 * @file PairParseFunctions.h
 *
 * @section DESCRIPTION
 *
 * Contains functions for parsing fastcgi params. Most of it is lifted from the FCGI
 * library
 */

#include <string>
#include <cstring>

#include "fastcgi_devkit/fastcgi.h"

#include "Common.h"

namespace Santiago{ namespace Fastcgi
{
/**
 * Parses fastcgi params and stores it in a map
 * @param data- character array
 * @param size of the character array
 * @param pairs passed as reference
 */
inline void ParsePairs(const char* data, unsigned int n,std::map<std::string,std::string>& pairs)
{
    typedef std::map<std::string,std::string> Pairs; 

    const unsigned char* u = reinterpret_cast<const unsigned char*>(data);

    for (std::string::size_type m = 0; m < n;) {
        std::string::size_type name_length, value_length;

        if (u[m] >> 7) {
            if (n - m < 4)
                break;
            name_length = ((u[m] & 0x7f) << 24) + (u[m + 1] << 16) +
                (u[m + 2] << 8) + u[m + 3];
            m += 4;
        } else
            name_length = u[m++];
        if (m >= n)
            break;

        if (u[m] >> 7) {
            if (n - m < 4)
                break;
            value_length = ((u[m] & 0x7f) << 24) + (u[m + 1] << 16) +
                (u[m + 2] << 8) + u[m + 3];
            m += 4;
        } else
            value_length = u[m++];

        if (n - m < name_length)
            break;
        std::string key(data + m, name_length);
        m += name_length;

        if (n - m < value_length)
            break;
        pairs.insert(Pairs::value_type(
            key, std::string(data + m, value_length)));
        m += value_length;
    }

}

/**
 * Writes a pair to a buffer
 * @param buffer- output buffer
 * @param key
 * @param value
 */
inline void WritePair(std::string& buffer,const std::string& key, const std::string& value)
{
    if (key.size() > 0x7f) {
        buffer.push_back(0x80 + ((key.size() >> 24) & 0x7f));
        buffer.push_back((key.size() >> 16) & 0xff);
        buffer.push_back((key.size() >> 8) & 0xff);
        buffer.push_back(key.size() & 0xff);
    } else
        buffer.push_back(key.size());

    if (value.size() > 0x7f) {
        buffer.push_back(0x80 + ((value.size() >> 24) & 0x7f));
        buffer.push_back((value.size() >> 16) & 0xff);
        buffer.push_back((value.size() >> 8) & 0xff);
        buffer.push_back(value.size() & 0xff);
    } else
        buffer.push_back(value.size());

    buffer.append(key);
    buffer.append(value);
}

}}

#endif
