#include <boost/assert.hpp>

#include "ErrorCategory.h"

namespace Santiago{ namespace Error
{
    std::error_condition ErrorCategory::default_error_condition(int code_) const noexcept
    {
        BOOST_ASSERT(_errorValueDescriptionMap.find(code_) != _errorValueDescriptionMap.end());
        return std::error_condition(code_,GetInstance());
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
             {SUCCESS,                       "Success."},
             {DATABASE_EXCEPTION,            "Database exception."},
             {DATABASE_QUERY_FAILED,         "Database query failed."},
             {USERNAME_ALREADY_EXISTS,       "Username already exists."},
             {INVALID_USERNAME_PASSWORD,     "Invalid username-password."},
             {INVALID_SESSION_COOKIE,        "Invalid session cookie."}
         }
    {}

}}
