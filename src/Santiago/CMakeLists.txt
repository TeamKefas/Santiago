add_subdirectory(Fastcgi)
add_subdirectory(fastcgi_devkit)
add_subdirectory(User)
add_subdirectory(Utils)
add_subdirectory(SantiagoDBTables)
#add_subdirectory(Server)
#TODO move LocalEndpoint.h to somewhere else

set(Santiago_SRCS ErrorCategory.cpp HTTPDefinitions.cpp)

if(WIN32)
  set(Santiago_SRCS ${Santiago_SRCS}
      ErrorCategory.h
      HTTPDefinitions.h)
endif(WIN32)

add_library(Santiago STATIC ${Santiago_SRCS})
target_link_libraries(Santiago Fastcgi fastcgi_devkit SantiagoDBTables SantiagoUser Utils)
install(TARGETS Santiago DESTINATION /usr/local/lib)
