#ifndef SANTIAGO_RESOURCE_RESOURCE_H
#define SANTIAGO_RESOURCE_RESOURCE_H

#include <boost/property_tree/ptree.hpp>

#include "Resource.h"

namespace Santiago{ namespace Resource
{

    template<typename ResourceTypesInfo,
             typename SerializationPolicy,
             typename LoadSavePolicy>
    class SingleNodeResource
    {
    public:

        template<typename T>
        T& get();

        template<typename T>
        const T& get() const;

        std::error getError();

        void saveToDisk(std::error& error_);
        
    protected:

        SingleNodeResource(const boost::any& resourceObject_);
        SingleNodeResource(const SingleNodeResource&) = delete;
        SingleNodeResource operator=(const SingleNodeResource&) = delete;        
        
    };

}}
