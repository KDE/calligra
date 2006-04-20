kde4_header()

include_directories( ${CMAKE_SOURCE_DIR}/kexi ${KDE4_INCLUDES}  )


########### next target ###############

set(kexiutils_LIB_SRCS identifier.cpp validator.cpp utils.cpp )

kde4_automoc(${kexiutils_LIB_SRCS})

kde4_add_library(kexiutils SHARED ${kexiutils_LIB_SRCS})

target_link_libraries(kexiutils  ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS} )

set_target_properties(kexiutils PROPERTIES VERSION 1.0.0 SOVERSION 1 )
install_targets(${LIB_INSTALL_DIR} kexiutils )


########### install files ###############

install_files( /include/kexiutils FILES  tristate.h identifier.h validator.h utils.h kexiutils_export.h )

kde4_footer()



#original Makefile.am contents follow:

#include $(top_srcdir)/kexi/Makefile.global
#
#lib_LTLIBRARIES = libkexiutils.la
#libkexiutils_la_SOURCES = identifier.cpp validator.cpp utils.cpp
#
#libkexiutilsincludedir=$(includedir)/kexiutils
#libkexiutilsinclude_HEADERS = tristate.h identifier.h validator.h utils.h kexiutils_export.h
#
#noinst_HEADERS = utils_p.h
#
#libkexiutils_la_LDFLAGS = \
#  $(KDE_RPATH) $(all_libraries) \
#  $(VER_INFO) -Wnounresolved -no-undefined
#
#libkexiutils_la_LIBADD = $(LIB_QT) $(LIB_KDEUI) $(LIB_KIO) 
#
#INCLUDES = -I$(top_srcdir)/kexi $(all_includes)
#
#METASOURCES = AUTO
