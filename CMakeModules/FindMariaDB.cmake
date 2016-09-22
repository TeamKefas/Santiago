# MariaDB
# -----
# Find the path to MariaDB header files and libraries
#
# DEFINES
# ------
# MARIADB_INCLUDE_DIR - MariaDB include directory
# MARIADB_LIBRARIES - Libraries required to link MariaDB
# MARIADB_FOUND - Confirmation

find_path(MARIADB_INCLUDE_DIR mysql.h
          /usr/include/mysql
          /usr/local/include/mysql
          )

find_library(MARIADB_LIBRARIES NAMES mysqlclient
            PATHS
            /usr/lib/mysql
            /usr/local/lib/mysql
            /opt/mysql/lib/mysql
            )

if(MARIADB_INCLUDE_DIR AND MARIADB_LIBRARIES)
    set(MARIADB_FOUND TRUE)
    message(STATUS "Found MariaDB: ${MARIADB_INCLUDE_DIR}, ${MARIADB_LIBRARIES}")
    else(MARIADB_INCLUDE_DIR AND MARIADB_LIBRARIES)
set(MARIADB_FOUND FALSE)
    message(STATUS "MariaDB not found.")
endif(MARIADB_INCLUDE_DIR AND MARIADB_LIBRARIES)

mark_as_advanced(MARIADB_INCLUDE_DIR MARIADB_LIBRARIES)
