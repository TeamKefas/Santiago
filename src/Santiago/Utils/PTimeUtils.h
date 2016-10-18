#ifndef SANTIAGO_UTILS_PTIMEUTILS_H
#define SANTIAGO_UTILS_PTIMEUTILS_H

namespace Santiago{ namespace Utils
{
    boost::posix_time::ptime ConvertStringToPtime(const std::string& timeStr_);
    std::string ConvertPtimeToString(const boost::posix_time::ptime& ptime_);
}}

#endif
