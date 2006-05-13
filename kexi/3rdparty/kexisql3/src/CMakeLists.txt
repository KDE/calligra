




########### next target ###############

set(kexisql3_LIB_SRCS 
   attach.c 
   auth.c 
   btree.c 
   btree.h 
   build.c 
   date.c 
   delete.c 
   expr.c 
   func.c 
   hash.c 
   insert.c 
   legacy.c 
   main.c 
   opcodes.c 
   os_mac.c 
   os_unix.c 
   os_win.c 
   pager.c 
   parse.c 
   pragma.c 
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
   vdbemem.c 
   where.c )

kde4_automoc(${kexisql3_LIB_SRCS})

kde4_add_library(kexisql3 SHARED ${kexisql3_LIB_SRCS})

target_link_libraries(kexisql3  ${KDE4_KDECORE_LIBS} )

set_target_properties(kexisql3 PROPERTIES VERSION 3.0.0 SOVERSION 3 )
install_targets(${LIB_INSTALL_DIR} kexisql3 )


########### next target ###############

set(ksqlite_SRCS shell.c )

kde4_automoc(${ksqlite_SRCS})

kde4_add_executable(ksqlite ${ksqlite_SRCS})

target_link_libraries(ksqlite  ${KDE4_KDECORE_LIBS} kexisql3 )

install_targets(/bin ksqlite )


########### install files ###############

install_files(  FILES  ./ksqlite2to3 )





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
