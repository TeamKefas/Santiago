#ifndef SANTIAGO_THREAD_THREADSPECIFICVARIMPL_H
#define SANTIAGO_THREAD_THREADSPECIFICVARIMPL_H

#include <map>
#include <memory>
#include <mutex>
#include <thread>

#include <boost/any.hpp>
#include <boost/optional.hpp>

#include "../Utils/STLog.h"

namespace Santiago{ namespace Thread
{
    class ThreadLocalStore;

    class ThreadSpecificVarImpl:public std::enable_shared_from_this<ThreadSpecificVarImpl>
    {
        struct PerThreadDataType
        {
            boost::any                              _tData;
            std::weak_ptr<ThreadLocalStore>         _threadLocalStoreWeakPtr;
        };
        typedef std::map<std::thread::id,PerThreadDataType> ThreadIdThreadDataMap;

    public:
        
        boost::optional<boost::any> getTData();
        void setTData(const boost::any& tData_);
        void removeAllReachableThreadData();
        ~ThreadSpecificVarImpl();

    private:

        void removeThreadData(const std::thread::id& threadId_);

        ThreadIdThreadDataMap         _threadIdThreadDataMap;
        std::mutex                    _mutex;
    };
}}

#endif
