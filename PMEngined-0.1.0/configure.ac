#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([PMEngined], [0.1.0], [])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([main.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([beanstalkclient], [bsc_new], , [AC_MSG_ERROR([Couldn't find beanstalkclient lib...try downloading the source from \
https://github.com/bergundy/libbeanstalkclient/] )])
AC_CHECK_LIB([curl], [curl_global_init])
AC_CHECK_LIB([ssl], [SSL_library_init])

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h float.h limits.h netdb.h netinet/in.h stdlib.h string.h sys/socket.h syslog.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([bzero floor gethostbyname inet_ntoa memset pow select socket strchr strtol])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
