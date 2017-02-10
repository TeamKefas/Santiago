set(Santiago_SRCS
  ErrorCategory.cpp
  HTTPDefinitions.cpp
  SantiagoDBTables/Definitions.cpp
  SantiagoDBTables/MariaDBConnection.cpp
  #User/SingleNode/Controller.cpp
  User/ServerV1/ConnectionMessage.cpp
  User/ServerV1/ServerMessage.cpp
  User/ServerV1/Server.cpp
  User/ServerV1/ConnectionServer.cpp
  User/ServerV1/ConnectionRequestsController.cpp
  User/ServerV1/ConnectionMessageSocket.cpp
  User/ServerV1/CreateUserRequestHandler.cpp
  User/ServerV1/ChangeUserEmailAddressRequestHandler.cpp
  User/ServerV1/ChangeUserPasswordRequestHandler.cpp
  User/ServerV1/LoginUserRequestHandler.cpp
  User/ServerV1/VerifyUserForCookieRequestHandler.cpp
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
