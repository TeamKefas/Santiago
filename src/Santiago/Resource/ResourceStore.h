#ifndef SANTIAGO_RESOURCE_RESOURCESTORE_H
#define SANTIAGO_RESOURCE_RESOURCESTORE_H

#include <boost/property_tree/ptree.hpp>

#include "Resource.h"

namespace Santiago{ namespace Resource
{

    template<typename Resource>
    class ResourceStore
    {
    public:

        typedef typename Resource::TypeEnum ResourceTypeEnum;
        
        ResourceStore();

        void getResources(std::vector<std::string>& resourceIdList_,
                          const std::function<const std::vector<Resource>&,const std::error& >& callbackFn_);

        void addResources(const std::vector<std::pair<ResourceTypeEnum,boost::any> >& resourceList_,
                          const std::function<const std::vector<std::pair<std::error,std::string> >& >& callbackFn_);

        void removeResource(const std::vector<std::string>& resourceIdList_,
                            const std::function<std::vector<std::error>,const std::error& >& callbackFn_);

    protected:

        std::map<std::string,std::pair<ResourceTypeEnum,boost::any> >    _resourceIdDataMap;
        typename Resource::SerializationPolicyCommonData                 _serializationData;
        typename Resource::LoadSavePolicyCommonData                      _loadSaveData;
        

    };


}}
#endif
