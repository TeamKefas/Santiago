#include <functional>
#include <thread>

#include "Santiago/Utils/STLog.h"

#include "ThreadSpecificVarImpl.h"

namespace Santiago{ namespace Thread
{
    /**************************************************
     * ThreadLocalStore
     **************************************************/
    class ThreadLocalStore
    {
        typedef std::function<void(const std::thread::id&)> ThreadEndCallbackFn;
        typedef std::map<void*,ThreadEndCallbackFn> VoidPtrThreadEndCallbackFnMap;

    public:
        ThreadLocalStore():
            _varPtrThreadEndCallbackFnMap(),
            _mutex()
        {}

        ~ThreadLocalStore()
        {
            std::thread::id threadId = std::this_thread::get_id();
            for(const VoidPtrThreadEndCallbackFnMap::value_type& element:
                    _varPtrThreadEndCallbackFnMap)
            {
                element.second(threadId);
            }
        }

        void addVar(void* varPtr_,const ThreadEndCallbackFn& threadEndCallbackFn_)
        {
            std::lock_guard<std::mutex> guard(_mutex);
            bool flag =
                _varPtrThreadEndCallbackFnMap.insert(std::make_pair(varPtr_,
                                                                    threadEndCallbackFn_)).second;
            BOOST_ASSERT(flag);
        }

        void removeVar(void* varPtr_)
        {
            std::lock_guard<std::mutex> guard(_mutex);
            size_t noOfElementsRemoved = _varPtrThreadEndCallbackFnMap.erase(varPtr_);
            BOOST_ASSERT(1 == noOfElementsRemoved);
        }

    protected:
        VoidPtrThreadEndCallbackFnMap      _varPtrThreadEndCallbackFnMap;
        std::mutex                         _mutex;
    };

    thread_local std::shared_ptr<ThreadLocalStore> ThreadLocalStorePtr;

    /**************************************************
     * ThreadSpecificVarImpl functions
     **************************************************/
    boost::optional<boost::any> ThreadSpecificVarImpl::getTData()
    {
        std::thread::id threadId = std::this_thread::get_id();

        std::lock_guard<std::mutex> guard(_mutex);
        ThreadIdThreadDataMap::const_iterator iter = _threadIdThreadDataMap.find(threadId);
        return (_threadIdThreadDataMap.end() == iter)?boost::none:iter->second._tData;
    }

    void ThreadSpecificVarImpl::setTData(const boost::any& tData_)
    {
        std::thread::id threadId = std::this_thread::get_id();

        std::lock_guard<std::mutex> guard(_mutex);
        ThreadIdThreadDataMap::iterator iter = _threadIdThreadDataMap.find(threadId);
        BOOST_ASSERT(_threadIdThreadDataMap.end() == iter);
        if(!ThreadLocalStorePtr)
        {
            ThreadLocalStorePtr.reset(new ThreadLocalStore());
        }

        bool flag = _threadIdThreadDataMap.insert(
            std::make_pair(threadId,
                           PerThreadDataType{tData_,std::weak_ptr<ThreadLocalStore>(ThreadLocalStorePtr)})).second;

        BOOST_ASSERT(flag);
        ThreadLocalStorePtr->addVar(reinterpret_cast<void*>(this),
                                        std::bind(&ThreadSpecificVarImpl::removeThreadData,
                                                  this->shared_from_this(),
                                                  std::placeholders::_1));
    }

    void ThreadSpecificVarImpl::removeAllReachableThreadData()
    {
        std::lock_guard<std::mutex> guard(_mutex);
        for(ThreadIdThreadDataMap::iterator iter = _threadIdThreadDataMap.begin();
            iter != _threadIdThreadDataMap.end();)
        {
            std::shared_ptr<ThreadLocalStore> threadLocalStorePtr = iter->second._threadLocalStoreWeakPtr.lock();
            if(!threadLocalStorePtr)
            {
                ++iter;
            }
            else
            {
                threadLocalStorePtr->removeVar(reinterpret_cast<void*>(this));
                iter = _threadIdThreadDataMap.erase(iter);
            }
        }
    }

    ThreadSpecificVarImpl::~ThreadSpecificVarImpl()
    {
        BOOST_ASSERT(0 == _threadIdThreadDataMap.size());
    }

    void ThreadSpecificVarImpl::removeThreadData(const std::thread::id& threadId_)
    {
        std::lock_guard<std::mutex> guard(_mutex);
        size_t noOfElementsRemoved = _threadIdThreadDataMap.erase(threadId_);
        BOOST_ASSERT(1 == noOfElementsRemoved);
    }

}}
