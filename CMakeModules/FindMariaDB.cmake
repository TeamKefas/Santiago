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
    # remove the preceding -I
    STRING(SUBSTRING ${MARIADB_INCLUDE_PATH} 2 -1 MARIADB_INCLUDE_DIR)

    EXEC_PROGRAM(${MARIADB_BIN_DIR}/mariadb_config
      ARGS "--libs"
      OUTPUT_VARIABLE MARIADB_LIBRARY_PATH)
    
    # get the string from -l to end
    STRING(FIND ${MARIADB_LIBRARY_PATH} " -l" MY_LENGTH)
    MATH(EXPR MY_LENGTH "${MY_LENGTH} + 3")
    STRING(SUBSTRING ${MARIADB_LIBRARY_PATH} ${MY_LENGTH} -1 MARIADB_LIBRARY) 
    
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
