#ifndef SANTIAGO_AUTHENTICATION_OICPROVIDERDATA_H
#define SANTIAGO_AUTHENTICATION_OICPROVIDERDATA_H

#include "Common.h"

namespace Santiago{ namespace Authentication
{
    class OICProviderData
    {
    public:

        OICProviderData(const std::string& certificateURL_)
            :_certificateURL(certificateURL_)
        {}

        JWKPtr getJWKForKeyId(const std::string& kid_);

    protected:
        
        void downloadAndUpdateJWKMap();

        std::string                  _certificateURL;
        std::map<std::string,JWKPtr> _kidJWKPtrMap;
    };

}}

#endif
