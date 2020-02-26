#
# FindMariaDB.cmake
#
# Try to find the include directory


  
IF(WIN32)
  
  IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
    SET(PFILES $ENV{ProgramW6432})
  ELSE()
    SET(PFILES $ENV{ProgramFiles})
  ENDIF()

  FIND_PATH(MARIADB_INCLUDE_DIR mysql.h
    ${PFILES}/MariaDB/*/include)

  # Try to find mariadb client library path
  FIND_PATH(MARIADB_LIBRARY_DIR mariadbclient.lib
    ${PFILES}/MariaDB/*/lib)
  
  IF(MARIADB_LIBRARY_DIR)
    STRING(CONCAT MARIADB_LIBRARY "${MARIADB_LIBRARY_DIR}/" "mariadbclient.lib")
  ENDIF()

ELSE()
  FIND_PATH(MARIADB_BIN_DIR mariadb_config
    /bin)
  IF(MARIADB_BIN_DIR)
    EXEC_PROGRAM(${MARIADB_BIN_DIR}/mariadb_config
      ARGS "--include"
      OUTPUT_VARIABLE MARIADB_INCLUDE_PATH)

    #Removing -I and splitting multiple paths into list
    STRING(REPLACE "-I" ""  MARIADB_INCLUDE_PATH "${MARIADB_INCLUDE_PATH}")
#    MESSAGE(STATUS "mariadb_config returned:${MARIADB_INCLUDE_PATH} \n")
    #Splitting the multiple paths into a list. This is needed for cmake.
    STRING(REGEX MATCHALL "[^ ]+" MARIADB_INCLUDE_DIR "${MARIADB_INCLUDE_PATH}")
#    MESSAGE(STATUS "path after after regex: ${MARIADB_INCLUDE_PATHS}")
   
   
    EXEC_PROGRAM(${MARIADB_BIN_DIR}/mariadb_config
      ARGS "--libs"
      OUTPUT_VARIABLE MARIADB_LIBRARY_PATH)
    
    # get the string from first -l to end and removing -l and split the multiple libraries
    # into a list
    STRING(FIND ${MARIADB_LIBRARY_PATH} " -l" MY_LENGTH)
    STRING(SUBSTRING ${MARIADB_LIBRARY_PATH} ${MY_LENGTH} -1 MARIADB_LIBRARY_PATH)
    STRING(REPLACE "-l" ""  MARIADB_LIBRARY_PATH "${MARIADB_LIBRARY_PATH}")
    STRING(REGEX MATCHALL "[^ ]+" MARIADB_LIBRARY "${MARIADB_LIBRARY_PATH}")    
    
  ELSE()
    
    FIND_PATH(MARIADB_INCLUDE_DIR mysql.h
      /usr/include
      /usr/include/mariadb
      /usr/include/mysql)
    
    FIND_PATH(MARIADB_LIBRARY_DIR libmariadbclient.a
      /usr/lib/mariadb
      /usr/lib
      /usr/libmariadb)
    
    IF(MARIADB_LIBRARY_DIR)
      STRING(CONCAT MARIADB_LIBRARY "${MARIADB_LIBRARY_DIR}/" "libmariadbclient.a")
    ELSE()
      FIND_PATH(MARIADB_LIBRARY_DIR libmariadb.a
        /usr/lib/mariadb
        /usr/lib
        /usr/libmariadb)
      
      IF(MARIADB_LIBRARY_DIR)
        STRING(CONCAT MARIADB_LIBRARY "${MARIADB_LIBRARY_DIR}/" "libmariadb.a")
      ENDIF()
      
    ENDIF()
  ENDIF()      
ENDIF()

IF(MARIADB_LIBRARY AND MARIADB_INCLUDE_DIR)
  MESSAGE(STATUS "Found MariaDB includes: ${MARIADB_INCLUDE_DIR}")
  MESSAGE(STATUS "Found MariaDB library: ${MARIADB_LIBRARY}")
  SET(MARIADB_FOUND TRUE)
ELSE()
  MESSAGE(STATUS "MariaDB not found. Includes: ${MARIADB_INCLUDE_DIR}, Libs: ${MARIADB_LIBRARY_DIR}")
  SET(MARIADB_FOUND FALSE)
ENDIF()
