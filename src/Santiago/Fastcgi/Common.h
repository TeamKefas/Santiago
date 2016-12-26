#ifndef FASTCGI_COMMON_H
#define FASTCGI_COMMON_H
/**
 * @file Common.h
 *
 * @section DESCRIPTION
 *
 * Contains the Common definitions.
 */

#include <memory>

#include <boost/assert.hpp>
#include <boost/optional.hpp>

#include "../ErrorCategory.h"
#include "../Utils/STLog.h"

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

    struct IsRequestAlive{};
}}
    
#endif
