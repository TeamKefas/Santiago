cmake_minimum_required(VERSION 2.8)
project (Santiago)

install(DIRECTORY src/Santiago DESTINATION /usr/local/include
        FILES_MATCHING PATTERN "*.h")


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules")

#commands specific to unix
if (UNIX)
add_definitions (-D__UNIX__)
add_definitions("-std=c++14 -Wall -pedantic")
endif (UNIX)


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

#Make tie src/lib directory in the include path for the whole project
include_directories (${CMAKE_SOURCE_DIR}/src)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
set(RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_subdirectory (src)
add_subdirectory (examples)
