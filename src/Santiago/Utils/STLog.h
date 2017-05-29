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
#include <cassert>

//#define BOOST_ENABLE_ASSERT_HANDLER 1

//#include <boost/assert.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/optional.hpp>

#define SOURCE_FILE (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : strrchr(__FILE__, '\\') ? \
strrchr(__FILE__, '\\') + 1 : __FILE__)

#define ST_LOG_IMPL(statement,log_level)\
    {\
        unsigned logLevelInt = static_cast<unsigned>(log_level);\
        if(logLevelInt >= static_cast<unsigned>(Santiago::Utils::STLog::GetInstance().getLogLevel())) \
        {\
            boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time(); \
            std::lock_guard<std::mutex> guard(Santiago::Utils::STLog::GetInstance().mutex()); \
            Santiago::Utils::STLog::GetInstance().stream()              \
            << SOURCE_FILE << "(LineNo:" << __LINE__ << ") ThreadNo:"<<std::this_thread::get_id() << " [" \
            << boost::posix_time::to_simple_string(now)<<"] "<<\
                Santiago::Utils::LogLevelStringMap[logLevelInt]<<": "<< statement << std::endl; \
        }\
    }\

#define ST_ASSERT(statement)\
    {\
        if(!(statement))\
        {\
            ST_LOG_IMPL(#statement,Santiago::Utils::LogLevel::ASSERT);\
            assert(statement);\
        }\
    }\

#define ST_LOG_DEBUG(statement) ST_LOG_IMPL(statement,Santiago::Utils::LogLevel::DEBUG)
#define ST_LOG_INFO(statement) ST_LOG_IMPL(statement,Santiago::Utils::LogLevel::INFO)
#define ST_LOG_ERROR(statement) ST_LOG_IMPL(statement,Santiago::Utils::LogLevel::ERROR)
#define ST_LOG_CRITICAL(statement) ST_LOG_IMPL(statement,Santiago::Utils::LogLevel::CRITICAL)

//#undef NDEBUG

namespace Santiago{ namespace Utils
{

    enum class LogLevel
    {
        DEBUG = 0,
        INFO = 1,
        ERROR = 2,
        CRITICAL = 3,
        ASSERT = 4
    };
    
    extern const std::array<std::string,5> LogLevelStringMap;
    
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

        void setLogLevel(LogLevel logLevel_) {_logLevel = logLevel_;}
        LogLevel getLogLevel() const {return _logLevel;}
        
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
        LogLevel                                             _logLevel;
    };

}} //closing namespace Santiago::Utils


#endif
