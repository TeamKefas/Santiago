#ifndef SANTIAGO_ERRORCATEGORY_H
#define SANTIAGO_ERRORCATEGORY_H

#include <map>
#include <system_error>

namespace Santiago
{
    enum ErrorCode
    {
        ERR_SUCCESS = 0,
        ERR_DATABASE_EXCEPTION = 1,
        ERR_DATABASE_INVALID_USER_INPUT = 2,
        ERR_DATABASE_GET_RETURNED_ZERO_RESULTS = 3,
        ERR_USERNAME_ALREADY_EXISTS = 4,
        ERR_EMAIL_ADDRESS_ALREADY_EXISTS = 5,
        ERR_INVALID_USERNAME_PASSWORD = 6,
        ERR_INVALID_EMAIL_ADDRESS_PASSWORD = 7,
        ERR_EMAIL_ADDRESS_NOT_REGISTERED = 8,
        ERR_INVALID_EMAIL_ADDRESS_RECOVERY_STRING = 9,
        ERR_NO_ACTIVE_SESSION_FOR_USERNAME = 10,
        ERR_INVALID_SESSION_COOKIE = 11,
        ERR_INVALID_FASTCGI_REQUEST = 12,
        ERR_FASTCGI_REQUEST_ALREADY_REPLIED = 13,
        ERR_INVALID_FASTCGI_VERSION = 14,
        ERR_FASTCGI_CONNECTION_ERROR = 15,
        ERR_AUTH_SERVER_CONNECTION_ERROR = 16,
    };

    class ErrorCategory:public std::error_category
    {
    public:

        const char* name() const noexcept {return "santiago error";}
        std::error_condition default_error_condition(int code_) const noexcept;
        bool equivalent(int code_, const std::error_condition& condition_) const noexcept;
        bool equivalent(const std::error_code& code_, int condition_) const noexcept;
        std::string message(int condition_) const;

        static const ErrorCategory& GetInstance();
        virtual ~ErrorCategory(){}

    protected:

        ErrorCategory();
        
        std::map<int,std::string>  _errorValueDescriptionMap;

    };
}
#endif
