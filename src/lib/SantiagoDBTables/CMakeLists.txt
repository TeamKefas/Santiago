set(SantiagoDBTables_SRCS
  MariaDBConnection.cpp)

if(WIN32)
  set(SantiagoDBTables_SRCS ${Database_SRCS}   
  Records.h
  MariaDBConnection.h)
endif(WIN32)

add_library(SantiagoDBTables STATIC ${SantiagoDBTables_SRCS})
target_link_libraries(SantiagoDBTables Fastcgi ${Boost_LIBRARIES})
