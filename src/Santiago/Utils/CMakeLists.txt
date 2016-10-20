set(Utils_SRCS PTimeUtils.cpp
 )

if(WIN32)
  set(Utils_SRCS
    STLog.h
    PTimeUtils.h)
endif(WIN32)

add_library(Utils STATIC ${Utils_SRCS})
target_link_libraries(Utils boost_date_time ${Boost_LIBRARIES})
