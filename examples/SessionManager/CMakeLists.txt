#define the variable echo-external_SRCS

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/testbin)
set(RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/testbin)

set(SessionManager_SRCS 
Main.cpp
SimplePageHandler.cpp
RequestHandlerBase.cpp
LoginPageHandler.cpp
SignupPageHandler.cpp)

if(WIN32)
  set(SessionManager_SRCS 
    ${SessionManager_SRCS}   
    LoginPageHandler.h
    Server.h
    RequestHandlerBase.h
    SimplePageHandler.h
    ErrorPageHandler.h
    SignupPageHandler.h)
endif(WIN32)

add_executable(SessionManager ${SessionManager_SRCS}) 
target_link_libraries(SessionManager Santiago ${Boost_LIBRARIES})
