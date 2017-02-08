set(Santiago_SRCS
  ErrorCategory.cpp
  HTTPDefinitions.cpp
  SantiagoDBTables/Definitions.cpp
  SantiagoDBTables/MariaDBConnection.cpp
  Thread/ThreadSpecificVarImpl.cpp
  User/SingleNode/Controller.cpp
  Utils/PTimeUtils.cpp
  Utils/STLog.cpp
  )

if(WIN32)
  set(Santiago_SRCS ${Santiago_SRCS}
      ErrorCategory.h
      HTTPDefinitions.h)
endif(WIN32)

add_library(Santiago STATIC ${Santiago_SRCS})
target_link_libraries(Santiago ${Boost_LIBRARIES} ${OPENSSL_CRYPTO_LIBRARY} ${OPENSSL_SSL_LIBRARY})
install(TARGETS Santiago DESTINATION /usr/local/lib)
