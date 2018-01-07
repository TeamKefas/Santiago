set(Santiago_SRCS
  ErrorCategory.cpp
  HTTPDefinitions.cpp
  SantiagoDBTables/Definitions.cpp
  SantiagoDBTables/MariaDBConnection.cpp
  Authentication/AuthenticatorBaseV1.cpp
  Authentication/ControllerDataBase.cpp
  Authentication/SingleNode/AuthenticatorImpl.cpp
  Authentication/SingleNode/AuthenticatorV1.cpp
  Authentication/SingleNode/ControllerData.cpp
  Authentication/MultiNode/ConnectionMessage.cpp
  Authentication/MultiNode/ConnectionMessageSocket.cpp
  Authentication/MultiNode/ConnectionRequestsController.cpp
  Authentication/MultiNode/ConnectionRequestsControllerBase.cpp
  Authentication/MultiNode/ClientCache.cpp
  Authentication/MultiNode/AuthenticatorImpl.cpp
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
      Authentication/AuthenticatorImplBase.h
      Authentication/ControllerBase.h
      Authentication/ControllerDataBase.h
      Authentication/SingleNode/AuthenticatorImpl.h
      Authentication/SingleNode/AuthenticatorV1.h
      Authentication/SingleNode/Controller.h
      Authentication/SingleNode/ControllerData.h
      Authentication/MultiNode/TimeDefs.h
      Authentication/MutiNode/ConnectionMessage.h
      Authentication/MutiNode/ConnectionMessageSocket.h
      Authentication/MutiNode/ConnectionRequestsController.h
      Authentication/MutiNode/ConnectionRequestsControllerBase.h
      Authentication/MultiNode/ClientCache.h
      Authentication/MultiNode/AuthenticatorImpl.h
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
target_link_libraries(Santiago ${Boost_LIBRARIES} ${MARIADB_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY} ${OPENSSL_SSL_LIBRARY} ${CURL_LIBRARIES})
install(TARGETS Santiago DESTINATION /usr/local/lib)
