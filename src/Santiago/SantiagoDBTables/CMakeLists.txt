set(SantiagoDBTables_SRCS
  Definitions.cpp
  MariaDBConnectionV1.cpp)

if(WIN32)
  set(SantiagoDBTables_SRCS ${SantiagoDBTables_SRCS}   
  DatabaseRecords.h
  Definitions.h
  MariaDBConnectionV1.h)
endif(WIN32)

add_library(SantiagoDBTables STATIC ${SantiagoDBTables_SRCS})
target_link_libraries(SantiagoDBTables Fastcgi ${Boost_LIBRARIES})
