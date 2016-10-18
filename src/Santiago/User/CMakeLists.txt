add_subdirectory(SingleNode)

set(SantiagoUser_SRCS ControllerBase.cpp)

if(WIN32)
  set(SantiagoUser_SRCS ${SantiagoUser_SRCS}
      ControllerBase.h)
endif(WIN32)

add_library(SantiagoUser STATIC ${SantiagoUser_SRCS})
target_link_libraries(SantiagoUser SantiagoUserSingleNode SantiagoDBTables)