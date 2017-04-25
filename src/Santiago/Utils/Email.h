#ifndef SANTIAGO_UTILS_EMAIL_H
#define SANTIAGO_UTILS_EMAIL_H

#include <iostream>
#include <string>
#include <vector>

#include <algorithm>
#include <cstring>
#include <sstream>

#include <curl/curl.h>

#include <boost/property_tree/json_parser.hpp>
namespace Santiago{ namespace Utils
{

    class Email
    {
      public:
        static bool initEmail(const std::string& sendingAddress_,
            const std::string& senderName_,
            const std::string& password_,
            const std::string& serverAddress_);

        static bool sendEmail(const std::string& receivingAddress_,
                              const std::string& subject_,
                              const std::string& message_);

        static bool sendEmail(const std::vector<std::string>& receivingAddresses_,
                              const std::string& subject_,
                              const std::string& message_);
        static bool sendEmailToAdmin(const boost::property_tree::ptree& config_,
                                     const std::string subject_,
                                     const std::string& message_);


      private:
        static std::string& sendingAddress();
        static std::string& senderName();
        static std::string& password();
        static std::string& serverAddress();
    };

} // closing namespace Utils
} // closing namespace Santiago 

#endif
