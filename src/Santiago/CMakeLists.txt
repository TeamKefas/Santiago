set(Santiago_SRCS
  ErrorCategory.cpp
  HTTPDefinitions.cpp
  SantiagoDBTables/Definitions.cpp
  SantiagoDBTables/MariaDBConnection.cpp
  User/SingleNode/Controller.cpp
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
  User/ServerV1/LogoutUserForCookieRequestHandler.cpp
  User/ServerV1/LogoutUserForAllCookiesRequestHandler.cpp
  User/ServerV1/DeleteUserRequestHandler.cpp
  Utils/PTimeUtils.cpp
  Utils/STLog.cpp
  )

if(WIN32)
  set(Santiago_SRCS ${Santiago_SRCS}
      ErrorCategory.h
      HTTPDefinitions.h
      SantiagoDBTables/Definitions.h
      SantiagoDBTables/MariaDBConnection.h
      User/SingleNode/Controller.h
      User/ServerV1/ConnectionMessage.ch
      User/ServerV1/ServerMessage.h
      User/ServerV1/Server.h
      User/ServerV1/ConnectionServer.h
      User/ServerV1/ConnectionRequestsController.h
      User/ServerV1/ConnectionMessageSocket.h
      User/ServerV1/CreateUserRequestHandler.h
      User/ServerV1/ChangeUserEmailAddressRequestHandler.h
      User/ServerV1/ChangeUserPasswordRequestHandler.h
      User/ServerV1/LoginUserRequestHandler.h
      User/ServerV1/VerifyUserForCookieRequestHandler.h
      User/ServerV1/LogoutUserForCookieRequestHandler.h
      User/ServerV1/LogoutUserForAllCookiesRequestHandler.h
      User/ServerV1/DeleteUserRequestHandler.h
      Utils/PTimeUtils.h
      Utils/STLog.h)
endif(WIN32)

add_library(Santiago STATIC ${Santiago_SRCS})
target_link_libraries(Santiago ${Boost_LIBRARIES} ${MARIADB_LIBRARIES} ${OPENSSL_CRYPTO_LIBRARY} ${OPENSSL_SSL_LIBRARY})
install(TARGETS Santiago DESTINATION /usr/local/lib)
