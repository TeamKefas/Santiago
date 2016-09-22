#add the server echo test
set (ServerEcho_SRCS Echo.cpp)

add_executable(ServerEcho ${ServerEcho_SRCS}) 
target_link_libraries(ServerEcho ${Boost_LIBRARIES})

#add the server asynchronous service test
set (ServerAsyncService_SRCS AsyncService.cpp)

add_executable(ServerAsyncService ${ServerAsyncService_SRCS}) 
target_link_libraries(ServerAsyncService ${Boost_LIBRARIES})