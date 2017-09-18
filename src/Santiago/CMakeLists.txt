set(Santiago_SRCS
  ErrorCategory.cpp
  HTTPDefinitions.cpp
  SantiagoDBTables/Definitions.cpp
  SantiagoDBTables/MariaDBConnection.cpp
  Authentication/AuthenticatorBaseV1.cpp
  Authentication/AuthenticatorBase.cpp
  Authentication/ControllerBase.cpp
  Authentication/ControllerDataBase.cpp
  Authentication/SingleNode/Authenticator.cpp
  Authentication/MultiNode/Authenticator.cpp
  Authentication/MultiNode/ClientCache.cpp
  Authentication/MultiNode/AuthenticatorMessageSocket.cpp
  Authentication/MultiNode/ConnectionMessage.cpp
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
      Authentication/AuthenticatorBaseV1.h
      Authentication/AuthenticatorBase.h
      Authentication/ControllerBase.h
      Authentication/ControllerDataBase.h
      Authentication/SingleNode/Authenticator.h
      Authentication/MultiNode/Authenticator.h
      Authentication/MultiNode/ClientCache.h
      Authentication/MultiNode/AuthenticatorMessageSocket.h
      Authentication/MultiNode/ConnectionMessage.h
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
