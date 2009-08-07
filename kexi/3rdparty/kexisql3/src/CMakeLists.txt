project(kexisql3)

configure_file(sqliteconfig.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/sqliteconfig.h )

########### next target ###############

set(kexisql3_LIB_SRCS 
   alter.c 
   analyze.c 
   attach.c 
   auth.c 
   btree.c 
   btree.h 
   build.c 
   callback.c 
   complete.c 
   date.c 
   delete.c 
   expr.c 
   func.c 
   hash.c 
   insert.c 
   legacy.c 
   main.c 
   opcodes.c 
   pager.c 
   parse.c 
   pragma.c 
   prepare.c 
   printf.c 
   random.c 
   select.c 
   table.c 
   tokenize.c 
   trigger.c 
   utf.c 
   update.c 
   util.c 
   vacuum.c 
   vdbe.c 
   vdbeapi.c 
   vdbeaux.c 
   vdbefifo.c
   vdbemem.c 
   where.c )

IF(WIN32)
   set(kexisql3_LIB_SRCS ${kexisql3_LIB_SRCS} ../sqlite3.def os_win.c)
ELSE() #unix (including Mac OSX)
   set(kexisql3_LIB_SRCS ${kexisql3_LIB_SRCS} os_unix.c)
ENDIF()

add_library(kexisql3 SHARED ${kexisql3_LIB_SRCS})

set_target_properties(kexisql3 PROPERTIES VERSION 3.2.8 SOVERSION 3 )
install(TARGETS kexisql3  ${INSTALL_TARGETS_DEFAULT_ARGS})


########### next target ###############

set(ksqlite_SRCS shell.c )


add_executable(ksqlite ${ksqlite_SRCS})

target_link_libraries(ksqlite kexisql3)

install(TARGETS ksqlite  DESTINATION ${BIN_INSTALL_DIR})


########### install files ###############


#original Makefile.am contents follow:

#include $(top_srcdir)/kexi/Makefile.global
#
#lib_LTLIBRARIES = libkexisql3.la
#
#bin_PROGRAMS = ksqlite
#
#bin_SCRIPTS = ./ksqlite2to3
#
#INCLUDES = $(all_includes)
#
#libkexisql3_la_SOURCES = \
#  attach.c \
#  auth.c \
#  btree.c \
#  btree.h \
#  build.c \
#  date.c \
#  delete.c \
#  expr.c \
#  func.c \
#  hash.c \
#  insert.c \
#  legacy.c \
#  main.c \
#  opcodes.c \
#  os_mac.c \
#  os_unix.c \
#  os_win.c \
#  pager.c \
#  parse.c \
#  pragma.c \
#  printf.c \
#  random.c \
#  select.c \
#  table.c \
#  tokenize.c \
#  trigger.c \
#  utf.c \
#  update.c \
#  util.c \
#  vacuum.c \
#  vdbe.c \
#  vdbeapi.c \
#  vdbeaux.c \
#  vdbemem.c \
#  where.c
#
##tclsqlite.c
#
#AM_CFLAGS = -DNO_TCL=1
#
#ksqlite_SOURCES = shell.c
#ksqlite_LDADD = libkexisql3.la $(LIBREADLINE)
#
#libkexisql3_la_LIBADD =
#libkexisql3_la_LDFLAGS = -version-info 3:0 $(all_libraries) --no-undefined
#
