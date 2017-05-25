#define the variable echo-external_SRCS
set (SimpleAppServerEcho_SRCS main.cpp
		SimplePageHandler.cpp
		SignupHandler.cpp
		RequestHandlerBase.cpp
		LoginPageHandler.cpp
		PasswordRecoveryInitiateRecoveryHandler.cpp)

if(WIN32)
  set(SimpleAppServerEcho_SRCS ${SimpleAppServerEcho_SRCS}   
  LoginPageHandler.h
  Server.h
  RequestHandlerBase.h
  SimplePageHandler.h
  PasswordRecoveryInitiateRecoveryHandler.h
  signupHandler.h)
endif(WIN32)

add_executable(SimpleAppServerEcho ${SimpleAppServerEcho_SRCS}) 
target_link_libraries(SimpleAppServerEcho Santiago ${MARIADB_LIBRARIES} ${Boost_LIBRARIES})
