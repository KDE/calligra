kde4_header()

include_directories( ${CMAKE_SOURCE_DIR}/karbon ${CMAKE_SOURCE_DIR}/karbon/core ${CMAKE_SOURCE_DIR}/karbon/visitors ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(karbonsvgexport_PART_SRCS svgexport.cc )

kde4_automoc(${karbonsvgexport_PART_SRCS})

kde4_add_plugin(karbonsvgexport WITH_PREFIX ${karbonsvgexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} karbonsvgexport )

target_link_libraries(karbonsvgexport  ${KDE4_KDECORE_LIBS} kofficeui karboncommon )

install_targets(${PLUGIN_INSTALL_DIR} karbonsvgexport )


########### next target ###############

set(karbonsvgimport_PART_SRCS svgimport.cc )

kde4_automoc(${karbonsvgimport_PART_SRCS})

kde4_add_plugin(karbonsvgimport WITH_PREFIX ${karbonsvgimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} karbonsvgimport )

target_link_libraries(karbonsvgimport  ${KDE4_KDECORE_LIBS} kofficeui karboncommon )

install_targets(${PLUGIN_INSTALL_DIR} karbonsvgimport )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  karbon_svg_export.desktop karbon_svg_import.desktop )

kde4_footer()



#original Makefile.am contents follow:

#kde_module_LTLIBRARIES = libkarbonsvgexport.la libkarbonsvgimport.la
#
#libkarbonsvgexport_la_LDFLAGS = $(KDE_PLUGIN)
#libkarbonsvgexport_la_LIBADD = \
#	$(LIB_KOFFICEUI) \
#	../../../karbon/libkarboncommon.la
#
#libkarbonsvgimport_la_LDFLAGS = $(KDE_PLUGIN)
#libkarbonsvgimport_la_LIBADD = \
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
#service_DATA = karbon_svg_export.desktop karbon_svg_import.desktop
#servicedir = $(kde_servicesdir)
#
#noinst_HEADERS = \
#	svgexport.h
#
#libkarbonsvgexport_la_SOURCES = \
#	svgexport.cc
#
#METASOURCES = AUTO
#
#libkarbonsvgimport_la_SOURCES = \
#	svgimport.cc
#
