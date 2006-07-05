

include_directories( ${CMAKE_SOURCE_DIR}/karbon ${CMAKE_SOURCE_DIR}/karbon/core ${CMAKE_SOURCE_DIR}/karbon/render ${CMAKE_SOURCE_DIR}/karbon/visitors ${KDE4_INCLUDES}  )


########### next target ###############

set(karbonpngexport_PART_SRCS pngexport.cc )

kde4_automoc(${karbonpngexport_PART_SRCS})

kde4_add_plugin(karbonpngexport WITH_PREFIX ${karbonpngexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} karbonpngexport )

target_link_libraries(karbonpngexport  ${KDE4_KDECORE_LIBS} kofficeui karboncommon )

install(TARGETS karbonpngexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  karbon_png_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})





#original Makefile.am contents follow:

#kde_module_LTLIBRARIES = libkarbonpngexport.la
#
#libkarbonpngexport_la_LDFLAGS = $(KDE_PLUGIN)
#libkarbonpngexport_la_LIBADD = \
#	$(LIB_KOFFICEUI) \
#	../../../karbon/libkarboncommon.la
#
#INCLUDES = \
#	$(KOFFICE_INCLUDES) $(KOPAINTER_INCLUDES) \
#	-I$(top_srcdir)/karbon \
#	-I$(top_srcdir)/karbon/core \
#	-I$(top_srcdir)/karbon/render \
#	-I$(top_srcdir)/karbon/visitors \
#	$(all_includes)
#
#service_DATA = karbon_png_export.desktop
#servicedir = $(kde_servicesdir)
#
#noinst_HEADERS = \
#	pngexport.h
#
#libkarbonpngexport_la_SOURCES = \
#	pngexport.cc
#
#libkarbonpngexport_la_METASOURCES = \
#	AUTO
#
