include_directories (${CMAKE_SOURCE_DIR}/src)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/testbin)
set(RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/testbin)

#add_subdirectory(Fastcgi)
add_subdirectory(SimpleAppServer)
#add_subdirectory(ServerV1)
#add_subdirectory(Client)
#add_subdirectory(Authentication)
#add_subdirectory(SantiagoDBTables)
add_subdirectory(HelloWorldApp)
