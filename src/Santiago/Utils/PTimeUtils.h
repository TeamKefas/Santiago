#ifndef SANTIAGO_UTILS_PTIMEUTILS_H
#define SANTIAGO_UTILS_PTIMEUTILS_H

/**
 * @file PTimeUtils.h
 *
 * @section DESCRIPTION
 *
 *  Contains the ConvertStringToPtime and ConvertPtimeToString member functions
 */

#include <string>

#include <boost/date_time/posix_time/ptime.hpp>

namespace Santiago{ namespace Utils
{
     /**
      * Convert the timeStr_ to boost::posix_time::ptime equivalent and returns it.                          
      * @param timeStr_ - Is of string type
      */
    boost::posix_time::ptime ConvertStringToPtime(const std::string& timeStr_);
    /**
     * Convert the ptime_ to it's string equivalent and returns it.          
     * @param ptime_ - Is of type boost::posix_time::ptime
     */
    std::string ConvertPtimeToString(const boost::posix_time::ptime& ptime_);
}}

#endif
