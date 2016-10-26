#ifndef SANTIAGO_THREAD_SPECIFIC_VAR_H
#define SANTIAGO_THREAD_SPECIFIC_VAR_H

#include <functional>
#include <memory>

#include <boost/thread/tss.hpp>

namespace Santiago
{
    template<typename T>
    class ThreadSpecificVar
    {
        typedef std::shared_ptr<T> TPtr;
        typedef std::function<TPtr()> ConstructorFn;

    public:

        ThreadSpecificVar(const ContructorFn& constructorFn_);

        ThreadSpecificVar(const ThreadSpecificVar&) = delete;
        ThreadSpecificVar operator=(const ThreadSpecificVar&) = delete;

        T& get()
        {
            if(!(*_dataPtr.get()))
            {
                _dataPtr.reset(new TPtr(constructorFn_()));
            }

            return *(*_dataPtr.get());
        }

    protected:

        boost::thread_specific_ptr<TPtr>  _dataPtr;
        std::function<TPtr()>             _constructorFn;

    };

}

#endif
