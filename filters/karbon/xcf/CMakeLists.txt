kde4_header()

include_directories( ${CMAKE_SOURCE_DIR}/karbon ${CMAKE_SOURCE_DIR}/karbon/core ${CMAKE_SOURCE_DIR}/karbon/render ${CMAKE_SOURCE_DIR}/karbon/visitors ${KDE4_INCLUDES}  )


########### next target ###############

set(karbonxcfexport_PART_SRCS xcfexport.cc )

kde4_automoc(${karbonxcfexport_PART_SRCS})

kde4_add_plugin(karbonxcfexport WITH_PREFIX ${karbonxcfexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} karbonxcfexport )

target_link_libraries(karbonxcfexport  ${KDE4_KDECORE_LIBS} kofficeui karboncommon )

install_targets(${PLUGIN_INSTALL_DIR} karbonxcfexport )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  karbon_xcf_export.desktop )

kde4_footer()



#original Makefile.am contents follow:

#kde_module_LTLIBRARIES = libkarbonxcfexport.la
#
#libkarbonxcfexport_la_LDFLAGS = $(KDE_PLUGIN)
#libkarbonxcfexport_la_LIBADD = \
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
#service_DATA = karbon_xcf_export.desktop
#servicedir = $(kde_servicesdir)
#
#noinst_HEADERS = \
#	xcfexport.h
#
#libkarbonxcfexport_la_SOURCES = \
#	xcfexport.cc
#
#libkarbonxcfexport_la_METASOURCES = \
#	AUTO
#
