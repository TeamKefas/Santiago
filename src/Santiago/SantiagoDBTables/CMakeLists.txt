set(SantiagoDBTables_SRCS
Definitions.cpp
MariaDBConnection.cpp)

if(WIN32)
  set(SantiagoDBTables_SRCS ${SantiagoDBTables_SRCS}   
  DatabaseRecords.h
  Definitions.h
  MariaDBConnection.h)
endif(WIN32)

add_library(SantiagoDBTables STATIC ${SantiagoDBTables_SRCS})
target_link_libraries(SantiagoDBTables SantiagoFastcgi mariadbclient Santiago ${Boost_LIBRARIES})
install(TARGETS SantiagoDBTables DESTINATION /usr/local/lib)
