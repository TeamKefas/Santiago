set(Fastcgi_SRCS Dummy.cpp)

if(WIN32)
  set(Fastcgi_SRCS ${fastcgi_devkit_SRCS}
    Acceptor.h
    ConnectionData.h
    PacketSocket.h
    RequestData.h
    Connection.h
    PairParseFunctions.h
    Request.h
    Common.h
    RecordSocket.h)
endif(WIN32)

add_library(Fastcgi STATIC ${Fastcgi_SRCS})
target_link_libraries(Fastcgi ${Boost_LIBRARIES})