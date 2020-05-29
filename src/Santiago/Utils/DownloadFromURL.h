#ifndef SANTIAGO_UTILS_DOWNLOADFROMURL_H
#define SANTIAGO_UTILS_DOWNLOADFROMURL_H

#include <string>
#include <boost/optional.hpp>

namespace Santiago{ namespace Utils
{
    boost::optional<std::string> TypicalSettingDownloadFromURL(const std::string& url_);
}}

#endif
