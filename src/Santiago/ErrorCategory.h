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
//        ERR_DATABASE_QUERY_FAILED = 2,
        ERR_USERNAME_ALREADY_EXISTS = 3,
        ERR_INVALID_USERNAME_PASSWORD = 4,
        ERR_INVALID_SESSION_COOKIE = 5,
        ERR_INVALID_FASTCGI_REQUEST = 6,
        ERR_FASTCGI_REQUEST_ALREADY_REPLIED = 7
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