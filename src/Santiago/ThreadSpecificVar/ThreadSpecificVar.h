#ifndef SANTIAGO_THREADSPECIFICVAR_THREADSPECIFICVAR_H
#define SANTIAGO_THREADSPECIFICVAR_THREADSPECIFICVAR_H

#include <functional>

#include <boost/thread/tss.hpp>

namespace Santiago{ namespace ThreadSpecificVar
{
    template<typename T>
    class ThreadSpecificVar
    {
        typedef std::function<T*()> ConstructorFn;

    public:

        ThreadSpecificVar(const ConstructorFn& constructorFn_);

        ThreadSpecificVar(const ThreadSpecificVar&) = delete;
        ThreadSpecificVar operator=(const ThreadSpecificVar&) = delete;

        T& get()
        {
            if(_dataPtr.get())
            {
                _dataPtr.reset(_constructorFn());
            }

            return (*_dataPtr.get());
        }

    protected:

        boost::thread_specific_ptr<T>   _dataPtr;
        std::function<T*()>             _constructorFn;

    };

}}

#endif
