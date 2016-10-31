set(Utils_SRCS PTimeUtils.cpp
 )

if(WIN32)
  set(Utils_SRCS
    STLog.h
    PTimeUtils.h)
endif(WIN32)

add_library(SantiagoUtils STATIC ${Utils_SRCS})
target_link_libraries(SantiagoUtils boost_date_time ${Boost_LIBRARIES})
install(TARGETS SantiagoUtils DESTINATION /usr/local/lib)
