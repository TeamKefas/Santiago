set(HelloWorld_SRCS  main.cpp RequestHandlerBase.cpp EchoHandler.cpp  )

if(WIN32)
  set(HelloWorld_SRCS 
    ${HelloWorld_SRCS}
    Server.h
    RequestHandlerBase.h
    EchoHandler.h
    ErrorURIHandler.h)
endif(WIN32)



add_executable(HelloWorldExec ${HelloWorld_SRCS}) 
target_link_libraries(HelloWorldExec Santiago ${Boost_LIBRARIES}) 
