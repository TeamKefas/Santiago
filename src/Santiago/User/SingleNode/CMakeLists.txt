set(SantiagoUserSingleNode_SRCS Controller.cpp)

if(WIN32)
  set(SantiagoUserSingleNode_SRCS ${SantiagoUserSingleNode_SRCS}
      Controller.h)
endif(WIN32)

add_library(SantiagoUserSingleNode STATIC ${SantiagoUserSingleNode_SRCS})
target_link_libraries(SantiagoUserSingleNode SantiagoDBTables)