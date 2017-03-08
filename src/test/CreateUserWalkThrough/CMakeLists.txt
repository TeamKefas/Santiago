set(CreateUserWalkThrough_SRCS  main.cpp RequestHandlerBase.cpp CreateUserHandler.cpp  )

if(WIN32)
  set(CreateUserWalkThrough_SRCS 
    ${CreateUserWalkThroug_SRCS}
    Server.h
    RequestHandlerBase.h
    CreateUserHandler.h
    ErrorURIHandler.h)
endif(WIN32)



add_executable(CreateUserWalkThroughExec ${CreateUserWalkThrough_SRCS}) 
target_link_libraries(CreateUserWalkThroughExec Santiago ${MARIADB_LIBRARIES} ${Boost_LIBRARIES}) 
