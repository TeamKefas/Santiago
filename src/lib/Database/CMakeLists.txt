set(Database_SRCS
  MariaDBConnections.cpp)

if(WIN32)
  set(Database_SRCS ${Database_SRCS}   
  Records.h
  MariaDBConnections.h)
endif(WIN32)

add_library(Database STATIC ${Database_SRCS})
target_link_libraries(Database Fastcgi ${Boost_LIBRARIES})
