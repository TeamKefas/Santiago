#define the variable echo-external_SRCS
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/examplesbin)
set(RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/examplesbin)


set(SessionServer_SRCS 
Main.cpp
RequestHandlerBase.cpp
LoginHandler.cpp
LogoutHandler.cpp
SignupHandler.cpp)

if(WIN32)
  set(SessionServer_SRCS 
    ${SessionServer_SRCS}   
    LoginHandler.h
    LogoutHandler.h
    Server.h
    RequestHandlerBase.h
    ErrorPageHandler.h
    SignupHandler.h)
endif(WIN32)

add_executable(SessionServer ${SessionServer_SRCS}) 
target_link_libraries(SessionServer Santiago ${Boost_LIBRARIES})
