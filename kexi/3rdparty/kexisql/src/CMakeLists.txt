




########### next target ###############

set(kexisql2_LIB_SRCS 
   attach.c 
   auth.c 
   btree.c 
   btree_rb.c 
   build.c 
   date.c 
   expr.c 
   func.c 
   hash.c 
   insert.c 
   os.c 
   pager.c 
   parse.c 
   pragma.c 
   printf.c 
   random.c 
   select.c 
   copy.c 
   delete.c 
   encode.c 
   table.c 
   tokenize.c 
   trigger.c 
   update.c 
   util.c 
   vacuum.c 
   vdbe.c 
   vdbeaux.c 
   where.c 
   opcodes.c 
   main.c )

kde4_automoc(${kexisql2_LIB_SRCS})

kde4_add_library(kexisql2 SHARED ${kexisql2_LIB_SRCS})

target_link_libraries(kexisql2  ${KDE4_KDECORE_LIBS} )

set_target_properties(kexisql2 PROPERTIES VERSION 2.8.15 SOVERSION 2 )
install(TARGETS kexisql2  DESTINATION ${LIB_INSTALL_DIR})


########### next target ###############

set(ksqlite2_SRCS shell.c )

kde4_automoc(${ksqlite2_SRCS})

kde4_add_executable(ksqlite2 ${ksqlite2_SRCS})

target_link_libraries(ksqlite2  ${KDE4_KDECORE_LIBS} kexisql2 )

install(TARGETS ksqlite2  DESTINATION ${BIN_INSTALL_DIR})


########### install files ###############






#original Makefile.am contents follow:

#include $(top_srcdir)/kexi/Makefile.global
#
#lib_LTLIBRARIES = libkexisql2.la
#
#bin_PROGRAMS = ksqlite2
#
#INCLUDES = $(all_includes)
#
#libkexisql2_la_SOURCES = attach.c auth.c btree.c btree_rb.c build.c date.c expr.c func.c hash.c \
#insert.c os.c pager.c parse.c pragma.c printf.c random.c select.c copy.c delete.c encode.c \
#table.c tokenize.c trigger.c update.c util.c vacuum.c vdbe.c vdbeaux.c where.c opcodes.c main.c
#
#AM_CFLAGS = -DNO_TCL=1
#
#ksqlite2_SOURCES = shell.c
#ksqlite2_LDADD = libkexisql2.la $(LIBREADLINE)
#
#libkexisql2_la_LIBADD =
#libkexisql2_la_LDFLAGS = -version-info 2:8 $(all_libraries) --no-undefined
#
