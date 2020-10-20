#ifndef SANTIAGO_COMMON_H
#define SANTIAGO_COMMON_H

#include "boost/version.hpp"
#include "boost/asio/io_context_strand.hpp"

namespace Santiago
{

#if BOOST_VERSION >= 106600
  typedef boost::asio::io_context::strand AsioStrand;
#else
  typedef boost::asio::strand AsioStrand;
#endif
    
}

#endif
