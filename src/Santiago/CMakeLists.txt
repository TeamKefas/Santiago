set(Santiago_SRCS
  ErrorCategory.cpp
  HTTPDefinitions.cpp
  SantiagoDBTables/Definitions.cpp
  SantiagoDBTables/MariaDBConnection.cpp
  Authentication/AuthenticatorBase.cpp
  Authentication/SingleNode/Authenticator.cpp
  Thread/ThreadSpecificVarImpl.cpp
  Utils/PTimeUtils.cpp
  Utils/STLog.cpp
  Utils/Email.cpp
  )

if(WIN32)
  set(Santiago_SRCS ${Santiago_SRCS}
      ErrorCategory.h
      HTTPDefinitions.h
      SantiagoDBTables/Definitions.h
      SantiagoDBTables/MariaDBConnection.h
      Authentication/SingleNode/Authenticator.h
      Utils/PTimeUtils.h
      Utils/STLog.h
      Utils/LogLevel.h
      Utils/Email.h
      FastCGI/Acceptor.h
      FastCGI/Common.h
      FastCGI/Connection.h
      FastCGI/ConnectionData.h
      FastCGI/PacketSocket.h
      FastCGI/PairParseFunctions.h
      FastCGI/RecordSocket.h
      FastCGI/Request.h
      FastCGI/RequestData.h)
endif(WIN32)

add_library(Santiago STATIC ${Santiago_SRCS})
target_link_libraries(Santiago ${Boost_LIBRARIES} ${MARIADB_LIBRARIES} ${OPENSSL_CRYPTO_LIBRARY} ${OPENSSL_SSL_LIBRARY} ${CURL_LIBRARIES})
install(TARGETS Santiago DESTINATION /usr/local/lib)
