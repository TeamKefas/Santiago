#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "PTimeUtils.h"

namespace Santiago{ namespace Utils
{
    boost::posix_time::ptime ConvertStringToPtime(const std::string& timeStr_)
    {
        return boost::posix_time::time_from_string(timeStr_);
    }

    std::string ConvertPtimeToString(const boost::posix_time::ptime& ptime_)
    {
        std::string timeStr = boost::gregorian::to_iso_extended_string(ptime_.date()) + ' ' +
            boost::posix_time::to_simple_string(ptime_.time_of_day());

        return timeStr;
    }
}}
