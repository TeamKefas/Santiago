#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <cjose/error.h>
#include <cjose/jwk.h>

#include "../Utils/DownloadFromURL.h"

#include "OICProviderData.h"

namespace Santiago{ namespace Authentication
{
    JWKPtr OICProviderData::getJWKForKeyId(const std::string& kid_)
    {
        auto keyIter = _kidJWKPtrMap.find(kid_);
        if(keyIter == _kidJWKPtrMap.end())
        {
            ST_LOG_INFO("Public key not found, Downloading again. kid:"<<kid_<<std::endl);
            downloadAndUpdateJWKMap();
        }

        keyIter = _kidJWKPtrMap.find(kid_);
        if(keyIter == _kidJWKPtrMap.end())
        {
            ST_LOG_INFO("Key still notfound. kid:"<<kid_<<std::endl);
            return JWKPtr();
        }

        if(!keyIter->second)
        {
            ST_LOG_INFO("The requested key failed in the jwk processing step"<<std::endl);
        }
        return keyIter->second;
    }

    void OICProviderData::downloadAndUpdateJWKMap()
    {
        boost::optional<std::string> certificateJSONString =
            Utils::TypicalSettingDownloadFromURL(_certificateURL);

        if(!certificateJSONString)
        {
            ST_LOG_INFO("certficate download failed for url:"<<_certificateURL<<std::endl);
            return;
        }
        
        try
        {
            std::istringstream jsonIn(std::move(*certificateJSONString));
            boost::property_tree::ptree certificateTree;
            boost::property_tree::read_json(jsonIn, certificateTree);
        
            _kidJWKPtrMap.clear();
            for(auto iter = certificateTree.get_child("keys").begin();
                iter != certificateTree.get_child("keys").end();
                iter++)
            {
                boost::property_tree::ptree keyTree = iter->second;
                std::ostringstream os;
                write_json(os, keyTree);
                std::string jwkString = os.str();
                
                std::cout << jwkString << std::endl;
                
                cjose_err error;
                error.code = CJOSE_ERR_NONE;
                
                JWKPtr key(
                    cjose_jwk_import(jwkString.c_str(), jwkString.length(), &error),
                    [](cjose_jwk_t* ptr_)
                    {
                        if(ptr_ != NULL)
                        {
                            bool flag = cjose_jwk_release(ptr_);
                            assert(!flag);
                        }
                    });
                
                if(error.code != CJOSE_ERR_NONE)
                {
                    ST_LOG_ERROR("cjose error while creating JWK: At "
                                 << error.file
                                 <<":" << error.line
                                 << ", message: " << error.message
                                 << std::endl);
                }
                
                //add the key to the map. Even the failed key
                _kidJWKPtrMap[iter->second.get<std::string>("kid")] = key;
            }
        }
        catch(std::exception& e)
        {
            ST_LOG_ERROR("Exception in parsing JWK certificates "<<certificateJSONString<<std::endl
                         <<", message: "<<e.what() <<std::endl);
            return;
        }
        catch(...)
        {
            ST_LOG_ERROR("Exception in parsing JWK certificates "<<certificateJSONString<<std::endl);
            return;
        }
        return;
    }

}}
