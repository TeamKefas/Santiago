set(UserServer_SRCS
  ConnectionMessage.cpp
  ServerMessage.cpp
  ServerV1.cpp
  ConnectionServer.cpp
  ConnectionRequestsController.cpp
  ConnectionMessageSocket.cpp
  DatabaseInterface.cpp
  CreateUserRequestHandler.cpp
  LoginUserRequestHandler.cpp
  VerifyUserForCookieRequestHandler.cpp
  ChangeUserPasswordRequestHandler.cpp
  LogoutUserForCookieRequestHandler.cpp
  LogoutUserForAllCookiesRequestHandler.cpp
  AddResourceRequestHandler.cpp
  RemovedCookieFromAppserverRequestHandler.cpp)

if(WIN32)
  set(UserServer_SRCS ${UserServer_SRCS}   
  ConnectionMessage.h
  ServerMessage.h
  ServerV1.h
  ConnectionServer.h
  ConnectionRequestsController.h
  ConnectionMessageSocket.h
  DatabaseInterface.h
  RequestHandlerBase.h
  CreateUserRequestHandler.h
  LoginUserRequestHandler.h
  VerifyUserForCookieRequestHandler.h
  ChangeUserPasswordRequestHandler.h
  LogoutUserForCookieRequestHandler.h
  LogoutUserForAllCookiesRequestHandler.h
  AddResourceRequestHandler.h
  RemovedCookieFromAppserverRequestHandler.h)
endif(WIN32)

add_library(SantiagoUserServer STATIC ${UserServer_SRCS})
target_link_libraries(SantiagoUserServer Fastcgi ${Boost_LIBRARIES})
install(TARGETS SantiagoUserServer DESTINATION /usr/local/lib)
