#ifndef SANTIAGO_UTILS_DYNAMICTOSTATIC_H
#define SANTIAGO_UTILS_DYNAMICTOSTATIC_H

#include <vector>

//#include <boost/mpl/int.hpp>
//#include <boost/mpl/assert.hpp>
//#include <boost/type_traits/is_same.hpp>
//#include <boost/mpl/next_prior.hpp>
//#include <boost/mpl/vector.hpp>
//#include <boost/mpl/vector_c.hpp>
//#include <boost/mpl/size.hpp>
//#include <boost/mpl/at.hpp>
#include <boost/mpl/for_each.hpp>

//#include <boost/tuple/tuple.hpp>
//#include <boost/mpl/vector.hpp>
//#include <boost/mpl/copy.hpp>
//#include <boost/type_traits/is_same.hpp>
//#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace Santiago{ namespace Utils{ namespace MPL
{

namespace mpl = boost::mpl;

//----------------------dynamic_to_static-------------------------------------//
template<typename List,
         typename FunctionClass,
         typename RetType = void>
class dynamic_to_static
{
public:
    static dynamic_to_static& Instance()
    {
        static dynamic_to_static instance;
        return instance;
    }

    RetType dynamic_call(FunctionClass &f,int index_)
    {
        return _dispatcherFunctions[index_](&f);
    }

private:
    struct DispatcherInitializer
    {
        dynamic_to_static& _parent;
        DispatcherInitializer(dynamic_to_static &parent_):
            _parent(parent_){}

        template<typename U>
        void operator()(U x)
        {
            //typedef FunctionClass::foo<U::value> TargetFunction;
            _parent._dispatcherFunctions.push_back
                (boost::bind(&FunctionClass::template foo<U::value>,_1) );
        }
    };
    
    std::vector<boost::function<RetType(FunctionClass*)> > _dispatcherFunctions;

    dynamic_to_static()
    {
        mpl::for_each<List>(DispatcherInitializer(*this));
    }
};

}}}
#endif
