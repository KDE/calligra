

include_directories( ${CMAKE_SOURCE_DIR}/kexi ${KDE4_INCLUDES}  )


########### next target ###############

set(kexiutils_LIB_SRCS identifier.cpp validator.cpp utils.cpp debuggui.cpp styleproxy.cpp)

kde4_automoc(${kexiutils_LIB_SRCS})

add_definitions( -DMAKE_KEXIUTILS_LIB )
kde4_add_library(kexiutils SHARED ${kexiutils_LIB_SRCS})
target_link_libraries(kexiutils  ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS} )

set_target_properties(kexiutils PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )
install(TARGETS kexiutils  DESTINATION ${LIB_INSTALL_DIR})


########### install files ###############

install( FILES  tristate.h identifier.h validator.h utils.h kexiutils_export.h  DESTINATION ${INCLUDE_INSTALL_DIR}/kexiutils)





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
