cmake_minimum_required(VERSION 2.8)
project (Santiago)

#install(DIRECTORY build/src/Santiago DESTINATION /usr/local/lib
#	FILES_MATCHING PATTERN "*.a")

install(DIRECTORY src/Santiago DESTINATION /usr/local/include
        FILES_MATCHING PATTERN "*.h")


set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules")

if (WIN32)
add_definitions (-D__WIN__ -D_CRT_SECURE_NO_WARNINGS)
set(BOOST_ROOT "C:/Program Files/boost/boost_1_52_0")
include_directories(${BOOST_ROOT})
set(BOOST_LIBRARYDIR "C:/Program Files/boost/boost_1_52_0/stage/lib")
set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
endif (WIN32)

#commands specific to unix
if (UNIX)
add_definitions (-D__UNIX__)
add_definitions("-std=c++0x -std=gnu++1y -Wall -pedantic")
endif (UNIX)

#Make tie src/lib directory in the include path for the whole project
include_directories (${CMAKE_SOURCE_DIR})

#find the boost library
find_package(Boost COMPONENTS thread system date_time coroutine REQUIRED)
include_directories( ${Boost_INCLUDE_DIR}/../ )
set(Boost_LIBRARIES ${Boost_LIBRARIES} pthread)

#TODO:Add MariaDB find_package(), include_directories() here
#     May need to add a cmakemodule
find_package(MariaDB REQUIRED)
if(MARIADB_FOUND)
include_directories (${MARIADB_INCLUDE_DIR})
else()
message(FATAL_ERROR "maria db not found")
endif()

find_package(OpenSSL REQUIRED)
include_directories(${OpenSSL_INCLUDE_DIR})

find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})

include_directories (${CMAKE_SOURCE_DIR}/src)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
set(RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_subdirectory (src)
add_subdirectory (examples)
