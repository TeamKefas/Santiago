#include "ControllerBase.h"

namespace Santiago{ namespace User
{

    ControllerBase::ControllerBase(boost::asio::io_service& ioService_, const boost::property_tree::ptree& config_)
        :_ioService(ioService_)
        ,_strand(_ioService)
        ,_config(config_)
    {}
}}
