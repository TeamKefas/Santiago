set(Server_SRCS main.cpp)

if(WIN32)
  set(Server_SRCS 
    ${Server_SRCS})
endif(WIN32)

add_executable(ServerExec ${Server_SRCS}) 
target_link_libraries(ServerExec Santiago mysqlclient ${Boost_LIBRARIES})
