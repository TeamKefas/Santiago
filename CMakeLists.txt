cmake_minimum_required(VERSION 2.8)
project (Santiago)

install(DIRECTORY src/Santiago DESTINATION /usr/local/include
        FILES_MATCHING PATTERN "*.h")


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules")

if(UNIX)
  add_definitions(-D__UNIX__)
  add_definitions(-DBOOST_COROUTINES_NO_DEPRECATION_WARNING)
  add_compile_options(-std=c++17)
  add_compile_options(-Wall)
endif(UNIX)

if(WIN32)
  add_definitions (-D__WIN__ -D_CRT_SECURE_NO_WARNINGS)
endif(WIN32)

#find the boost library
find_package(Boost COMPONENTS thread system date_time coroutine REQUIRED)
include_directories( ${Boost_INCLUDE_DIR}/../ )
set(Boost_LIBRARIES ${Boost_LIBRARIES} pthread)

find_package(MariaDB REQUIRED)
include_directories (${MARIADB_INCLUDE_DIR})

find_package(OpenSSL REQUIRED)
include_directories(${OpenSSL_INCLUDE_DIR})

find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})

#Make tie src/lib directory in the include path for the whole project
include_directories (${CMAKE_SOURCE_DIR}/src)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
set(RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_subdirectory (src)
add_subdirectory (examples)
