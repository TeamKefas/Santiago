set(fastcgi_devkit_SRCS Dummy.cpp)

if(WIN32)
  set(fastcgi_devkit_SRCS ${fastcgi_devkit_SRCS} fastcgi_devkit.h)
endif(WIN32)

add_library(fastcgi_devkit STATIC ${fastcgi_devkit_SRCS})