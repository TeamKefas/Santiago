#include <boost/assert.hpp>

#include "ErrorCategory.h"

namespace Santiago
{
    std::error_condition ErrorCategory::default_error_condition(int code_) const noexcept
    {
        BOOST_ASSERT(_errorValueDescriptionMap.find(code_) != _errorValueDescriptionMap.end());
        return std::error_condition(code_, GetInstance());
    }

    bool ErrorCategory::equivalent(int code_, const std::error_condition& condition_) const noexcept
    {
        BOOST_ASSERT(&condition_.category() == &GetInstance());
        return (code_ == condition_.value());
    }

    bool ErrorCategory::equivalent(const std::error_code& code_,int condition_) const noexcept
    {
        BOOST_ASSERT(&code_.category() == &GetInstance());
        return (code_.value() == condition_);
    }

    std::string ErrorCategory::message(int condition_) const
    {
        BOOST_ASSERT(_errorValueDescriptionMap.find(condition_) != _errorValueDescriptionMap.end());
        return _errorValueDescriptionMap.find(condition_)->second;
    }

    const ErrorCategory& ErrorCategory::GetInstance()
    {
        static ErrorCategory instance;
        return instance;
    }

    ErrorCategory::ErrorCategory()
        :_errorValueDescriptionMap
         {
             {ERR_SUCCESS,                                 "Success."},
             {ERR_DATABASE_EXCEPTION,                      "Database exception."},
             {ERR_DATABASE_INVALID_USER_INPUT,             "Invalid input."},
//             {ERR_DATABASE_QUERY_FAILED,                   "Database query failed."},
             {ERR_USERNAME_ALREADY_EXISTS,                 "Username already exists."},
             {ERR_EMAIL_ADDRESS_ALREADY_EXISTS,            "Email id already exists."},
             {ERR_INVALID_USERNAME_PASSWORD,               "Invalid username password."},
             {ERR_INVALID_EMAIL_ADDRESS_PASSWORD,          "Invalid email-id password."},
             {ERR_EMAIL_ADDRESS_NOT_REGISTERED,            "Email address not registered."},
             {ERR_INVALID_EMAIL_ADDRESS_RECOVERY_STRING,   "Invalid email-id recovery string."},
             {ERR_INVALID_SESSION_COOKIE,                  "Invalid session cookie."},
             {ERR_INVALID_FASTCGI_REQUEST,                 "Invalid fastcgi request."},
             {ERR_FASTCGI_REQUEST_ALREADY_REPLIED,         "Fastcgi request already replied"}
         }
    {}

}
