


#include "Email.h"
#include "STLog.h"

namespace {

    struct MailData 
    {
        std::string remainingMailData;
    };                

    size_t readMailDataFn(void *ptr, size_t size, size_t nmemb, void *userp)
    {
        MailData *mailData = reinterpret_cast<MailData*>(userp);
         
        // size * nmemb is the max no of bytes that must be passed to ptr by this function. Verifying that is too low
        if((size == 0) || (nmemb == 0) || ((size*nmemb) < 1)) 
        {
            return 0;
        }
        
        // Checking if no remaining mail data exists. If so return 0
        if (0 == mailData->remainingMailData.length()) return 0;
        
        // Copying mail data to buffer that was passed (ptr)
        size_t maxAllowedSize = size*nmemb;
        unsigned int readSize = std::min(mailData->remainingMailData.length(), maxAllowedSize);
        std::string data = mailData->remainingMailData.substr(0, readSize);
        std::memset(ptr, 0, maxAllowedSize);
        std::memcpy(ptr, data.c_str(), readSize);
        
        // Updating mail data
        mailData->remainingMailData = (readSize == mailData->remainingMailData.length())? "":
            mailData->remainingMailData.substr(readSize, std::string::npos);

        // Returning length of buffer that was filled
        return readSize;
    }

} // closing anonymous namespace


namespace Santiago{ namespace Utils
{
    
    bool Email::initEmail(const std::string& sendingAddress_,
                          const std::string& senderName_,
                          const std::string& password_,
                          const std::string& serverAddress_)
    {
        sendingAddress() = sendingAddress_;
        senderName() = senderName_;
        password() = password_;
        serverAddress() = "smtps://" + serverAddress_;

        if(sendEmail(sendingAddress_, "Initialization mail",  "Mail service initialized."))
        {
            return true;
        }
        else
        {
            return false;
        }
        
    }

    bool Email::sendEmail(const std::string& receivingAddress_,
                          const std::string& subject_,
                          const std::string& message_)
    {
        std::vector<std::string> receivingAddresses;
        receivingAddresses.push_back(receivingAddress_);
        
        return sendEmail(receivingAddresses, subject_, message_);
    }

    bool Email::sendEmail(const std::vector<std::string>& receivingAddresses_,
                          const std::string& subject_,
                          const std::string& message_)
    {
        // Modifing receiving addresses to appropriate format
        std::vector<std::string> modifiedReceivingAddresses;
        for(std::vector<std::string>::const_iterator it = receivingAddresses_.begin();
            it != receivingAddresses_.end();
            ++it)
        {
            modifiedReceivingAddresses.push_back("<" + *it + ">");
        }

        // Writing message
        MailData mailData;
        std::stringstream str;
        str << "From: " << senderName() << " <" << sendingAddress() << ">" << "\r\n"
            << "Subject: " << subject_ << "\r\n"
            << "\r\n"
            << message_ << "\r\n";
        mailData.remainingMailData = str.str();

        // Initializing curl structures
        CURL *curl;
        CURLcode res = CURLE_OK;
        struct curl_slist *recipients = NULL;
        curl = curl_easy_init();

        if(curl) 
        {
            // Providing values for various curl/email related params
            curl_easy_setopt(curl, CURLOPT_USERNAME, sendingAddress().c_str());
            curl_easy_setopt(curl, CURLOPT_PASSWORD, password().c_str());
            curl_easy_setopt(curl, CURLOPT_URL, serverAddress().c_str());
            curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
            curl_easy_setopt(curl, CURLOPT_MAIL_FROM, sendingAddress().c_str());

            for(std::vector<std::string>::const_iterator it = modifiedReceivingAddresses.begin();
                it != modifiedReceivingAddresses.end();
                ++it)
            {
                recipients = curl_slist_append(recipients, it->c_str());
            }

            curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
            curl_easy_setopt(curl, CURLOPT_READFUNCTION, readMailDataFn);
            curl_easy_setopt(curl, CURLOPT_READDATA, &mailData);
            curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

            res = curl_easy_perform(curl);
            if(res != CURLE_OK)
            {
                ST_LOG_ERROR("Failed sending email. Reason: " << curl_easy_strerror(res) << std::endl);
                return false;
            }

            // Freeing memory for curl and its data structures
            curl_slist_free_all(recipients);
            curl_easy_cleanup(curl);

            return true;
        }

        ST_LOG_ERROR("Failed initializing curl." << std::endl);
        return false;
    }

    bool Email::sendEmailToAdmin(const boost::property_tree::ptree& config_,
                                 const std::string subject_, 
                                 const std::string& message_)
    {
        std::vector<std::string> adminList;
        for (auto& item : config_.get_child("App.Email.AdminAddresses"))
        {
            adminList.push_back(item.second.get_value<std::string>());
        }

        if(!sendEmail(adminList, subject_, message_))
        {
            ST_LOG_ERROR("Error sending email. subject: " << subject_);
            return false;
        }

        return true;
    }
    
    std::string& Email::sendingAddress()
    {
        static std::string addr;
        return addr;
    }

    std::string& Email::senderName()
    {
        static std::string sender;
        return sender;
    }

    std::string& Email::password()
    {
        static std::string pwd;
        return pwd;
    }

    std::string& Email::serverAddress()
    {
        static std::string addr;
        return addr;
    }

} // closing namespace Utils
} // closing namespace AppServer 
} // closing namespace OptionAutomator
