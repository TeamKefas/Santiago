#include "PTimeUtils.h"

namespace Santiago{ namespace Utils
{
    boost::posix_time::ptime ConvertStringToPTime(const std::string& timeStr_) const
    {
        return boost::posix_time::time_from_string(timeStr_);
    }

    std::string ConvertPTimeToString(const boost::posix_time::ptime& ptime_) const
    {
        std::string timeStr = boost::gregorian::to_iso_extended_string(ptime_.date()) + ' ' +
            boost::posix_time::to_simple_string(ptime_.time_of_day());

        return timeStr;
    }
}}
