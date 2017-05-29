#include <fstream>

#include "STLog.h"

namespace Santiago{ namespace Utils
{
    const std::array<std::string, 5> LogLevelStringMap = {"DEBUG",
                                                          "INFO",
                                                          "ERROR",
                                                          "CRITICAL",
                                                          "ASSERT FAILED"};
                                                               
    STLog& STLog::GetInstance()
    {
        static STLog instance;
        return instance;
    }

    STLog::STLog():
        _fileNameStreamPair(boost::none),
        _mutex(),
        _logLevel(LogLevel::INFO)
    {
#ifndef NDEBUG
        _logLevel = LogLevel::DEBUG;
#endif
    }

    void STLog::setOutputFile(const std::string& fileName_)
    {
        if(_fileNameStreamPair && (fileName_ == _fileNameStreamPair->first))
        {
            return;
        }
        initFile(fileName_);
    }

    std::ostream& STLog::stream()
    {
        if(_fileNameStreamPair)
        {
            return *_fileNameStreamPair->second;
        }
        else
        {
            initFile("STLog_" +
                     boost::posix_time::to_iso_string(boost::posix_time::microsec_clock::local_time()) +
                     ".txt");
            return *_fileNameStreamPair->second;
        }
    }

    void STLog::initFile(const std::string& fileName_)
    {
        _fileNameStreamPair.reset(std::make_pair(fileName_,
                                                 OStreamPtr(new std::ofstream(fileName_,
                                                                              std::ios_base::out|std::ios_base::app))));
        
        if(!std::static_pointer_cast<std::ofstream>(_fileNameStreamPair->second)->is_open())
        {
//            std::cerr<<"Failed to open log file"<<std::endl;
            throw std::runtime_error("Failed to open log file.");
        }
    }

}}
