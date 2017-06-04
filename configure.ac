# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([Santiago], [1.0], [info@kefassoftwares.com])
#AM_INIT_AUTOMAKE(SANTIAGO, 1.0)
#AM_INIT_AUTOMAKE
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
AM_PROG_AR

AC_CONFIG_HEADERS([config.h])


CFLAGS="-g -O3" # -fno-omit-frame-pointer
CPPFLAGS="-g -O3" # -fno-omit-frame-pointer
CXXFLAGS="-g -O3 -std=c++0x -std=gnu++1y -Wall -pedantic -L/usr/lib/mysql -I/usr/include/mysql -lmysqlclient -lboost_system -lboost_thread -lboost_coroutine -L/usr/lib -lssl -lcrypto -lpthread -lcurl" # -fno-omit-frame-pointer

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_RANLIB
AC_PROG_MAKE_SET

#CFLAGS="$CFLAGS -I/opt/local/include"
#CXXFLAGS="$CXXFLAGS -I/opt/local/include"
#LDFLAGS="$LDFLAGS -L/usr/local/lib -I/usr/include/mysql"

# Checks for libraries.
AC_CHECK_LIB([boost_thread], [main], [], [echo "boost library not found.  Please install boost library before proceeding"; exit -1])
AC_CHECK_LIB([crypto], [main], [], [echo "openssl crypto library not found.  Please install openssl library before proceeding"; exit -1])
AC_CHECK_LIB([pthread], [pthread_mutex_init], [], [echo "pthreads not found.  Please install pthread library before proceeding"; exit -1])
AC_CHECK_LIB([mysqlclient], [mysql_init], [], [echo "libmysqlclient is needed for MySQL support"; exit -1])
AC_CHECK_LIB([curl], [], [echo "curl library is needed"; exit -1])
 	         
# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h fcntl.h netdb.h netinet/in.h stdlib.h string.h sys/ioctl.h sys/param.h sys/socket.h sys/time.h unistd.h])


# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_OFF_T
AC_TYPE_SSIZE_T
AC_HEADER_TIME
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T
AC_C_VOLATILE


# Checks for library functions.
AC_CHECK_FUNCS([bzero strrchr])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
