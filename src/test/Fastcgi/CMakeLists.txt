#define the variable echo-external_SRCS
set (FastcgiEcho_SRCS Echo.cpp)

add_executable(FastcgiEcho ${FastcgiEcho_SRCS}) 
target_link_libraries(FastcgiEcho Fastcgi ${Boost_LIBRARIES})