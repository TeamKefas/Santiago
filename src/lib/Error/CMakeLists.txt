set(Error_SRCS ErrorCategory.cpp)

if(WIN32)
  set(Error_SRCS ${Error_SRCS}
      ErrorCategory.h)
endif(WIN32)

add_library(Error STATIC ${Error_SRCS})
