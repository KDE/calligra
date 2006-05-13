

include_directories( ${CMAKE_SOURCE_DIR}/karbon ${CMAKE_SOURCE_DIR}/karbon/core ${CMAKE_SOURCE_DIR}/karbon/visitors ${KDE4_INCLUDES}  )


########### next target ###############

set(karbonxamlexport_PART_SRCS xamlexport.cc )

kde4_automoc(${karbonxamlexport_PART_SRCS})

kde4_add_plugin(karbonxamlexport WITH_PREFIX ${karbonxamlexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} karbonxamlexport )

target_link_libraries(karbonxamlexport  ${KDE4_KDECORE_LIBS} kofficeui karboncommon )

install_targets(${PLUGIN_INSTALL_DIR} karbonxamlexport )


########### next target ###############

set(karbonxamlimport_PART_SRCS xamlimport.cc )

kde4_automoc(${karbonxamlimport_PART_SRCS})

kde4_add_plugin(karbonxamlimport WITH_PREFIX ${karbonxamlimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} karbonxamlimport )

target_link_libraries(karbonxamlimport  ${KDE4_KDECORE_LIBS} kofficeui karboncommon )

install_targets(${PLUGIN_INSTALL_DIR} karbonxamlimport )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  karbon_xaml_export.desktop karbon_xaml_import.desktop )





#original Makefile.am contents follow:

#kde_module_LTLIBRARIES = libkarbonxamlexport.la libkarbonxamlimport.la
#
#libkarbonxamlexport_la_LDFLAGS = $(KDE_PLUGIN)
#libkarbonxamlexport_la_LIBADD = \
#	$(LIB_KOFFICEUI) \
#	../../../karbon/libkarboncommon.la
#
#libkarbonxamlimport_la_LDFLAGS = $(KDE_PLUGIN)
#libkarbonxamlimport_la_LIBADD = \
#	$(LIB_KOFFICEUI) \
#	../../../karbon/libkarboncommon.la
#
#INCLUDES = \
#	$(KOFFICE_INCLUDES) \
#	$(KOPAINTER_INCLUDES) \
#	-I$(top_srcdir)/karbon \
#	-I$(top_srcdir)/karbon/core \
#	-I$(top_srcdir)/karbon/visitors \
#	$(all_includes)
#
#service_DATA = karbon_xaml_export.desktop karbon_xaml_import.desktop
#servicedir = $(kde_servicesdir)
#
#noinst_HEADERS = \
#	xamlexport.h \
#	xamlimport.h
#
#libkarbonxamlexport_la_SOURCES = \
#	xamlexport.cc
#
#libkarbonxamlimport_la_SOURCES = \
#	xamlimport.cc
#
#METASOURCES = AUTO
#
