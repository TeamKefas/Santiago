set(Client_SRCS Client.cpp main.cpp )

if(WIN32)
  set(Client_SRCS 
    ${Client_SRCS}
    Client.h)
endif(WIN32)

add_executable(ClientExec ${Client_SRCS}) 
target_link_libraries(ClientExec Santiago mysqlclient ${Boost_LIBRARIES}) 
