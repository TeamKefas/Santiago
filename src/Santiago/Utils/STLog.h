#ifndef SANTIAGO_UTILS_STLOG_H
#define SANTIAGO_UTILS_STLOG_H

/**
 * @file STLog.h
 *
 * @section DESCRIPTION
 *
 *  Contains STLog class which logs data.
 */

#include <string>
#include <ostream>
#include <memory>
#include <mutex>
#include <thread>
#undef NDEBUG
#include <cassert>

//#define BOOST_ENABLE_ASSERT_HANDLER 1

//#include <boost/assert.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/optional.hpp>

namespace Santiago{ namespace Utils
{
    class STLog
    {
    public:

         /**
          * It returns an istance of STLog class. 
          */
        static STLog& GetInstance();
         /**
          * It returns a reference to the shared pointer of ostream type.
          */
        std::ostream& stream();
         /**
          *  It returns the std::mutex reference.
          */
        std::mutex& mutex() {return _mutex;}

         /**
          * It creates a new file
          * @param  fileName_ -  It is used to create a new Log file.
          */
        void setOutputFile(const std::string& fileName_);
        /**
         * The destructor
         */
        ~STLog()
        {}

    protected:
        /**
         * The constructor
         */
        STLog();
        /**
         *  It creates a new Log file with the file name we provided. 
         * @param  fileName_ - It is used to create new Log file.
         */

        void initFile(const std::string& fileName_);

        typedef std::shared_ptr<std::ostream> OStreamPtr;

        boost::optional<std::pair<std::string,OStreamPtr> >  _fileNameStreamPair;
        std::mutex                                           _mutex;
    };

}} //closing namespace Santiago::Utils

#define SOURCE_FILE (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : strrchr(__FILE__, '\\') ? \
strrchr(__FILE__, '\\') + 1 : __FILE__)

#define ST_LOG_IMPL(statement,log_type)\
    {\
        boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();\
        std::lock_guard<std::mutex> guard(Santiago::Utils::STLog::GetInstance().mutex());\
        Santiago::Utils::STLog::GetInstance().stream()\
            << SOURCE_FILE << "(LineNo:" << __LINE__ << ") ThreadNo:"<<std::this_thread::get_id() << " ["\
            << boost::posix_time::to_simple_string(now)<<"] "<< log_type<<": "<< statement << std::endl; \
    }\

//#define NDEBUG
#define ST_ASSERT_IMPL(statement,log_type)\
    {\
        if(!(statement))\
        {\
            boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time(); \
            std::lock_guard<std::mutex> guard(Santiago::Utils::STLog::GetInstance().mutex()); \
            Santiago::Utils::STLog::GetInstance().stream()              \
            << SOURCE_FILE << "(LineNo:" << __LINE__ << ") ThreadNo:"<<std::this_thread::get_id() << " [" \
            << boost::posix_time::to_simple_string(now)<<"] "<< log_type<<": "<< "Assertion failed." << std::endl; \
            assert(statement);                                          \
        }\
    }\

#define ST_LOG_DEBUG(statement) ST_LOG_IMPL(statement,"DEBUG")
#define ST_LOG_INFO(statement) ST_LOG_IMPL(statement,"INFO")
#define ST_LOG_ERROR(statement) ST_LOG_IMPL(statement,"ERROR")
#define ST_LOG_CRITICAL(statement) ST_LOG_IMPL(statement,"CRITICAL")
#define ST_ASSERT(statement) ST_ASSERT_IMPL(statement, "ASSERT")

//#undef NDEBUG
#endif
