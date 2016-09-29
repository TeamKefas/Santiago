set(SantiagoDBTables_SRCS
  Maps.cpp
  MariaDBConnection.cpp)

if(WIN32)
  set(SantiagoDBTables_SRCS ${SantiagoDBTables_SRCS}   
  UserPermissions.h
  Maps.h
  Records.h
  MariaDBConnection.h)
endif(WIN32)

add_library(SantiagoDBTables STATIC ${SantiagoDBTables_SRCS})
target_link_libraries(SantiagoDBTables Fastcgi ${Boost_LIBRARIES})
