

include_directories( ${CMAKE_SOURCE_DIR}/karbon ${CMAKE_SOURCE_DIR}/karbon/core ${CMAKE_SOURCE_DIR}/karbon/render ${CMAKE_SOURCE_DIR}/karbon/visitors ${KDE4_INCLUDES}  )


########### next target ###############

set(karbonxcfexport_PART_SRCS xcfexport.cc )


kde4_add_plugin(karbonxcfexport ${karbonxcfexport_PART_SRCS})



target_link_libraries(karbonxcfexport  ${KDE4_KDECORE_LIBS}  karboncommon )

install(TARGETS karbonxcfexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  karbon_xcf_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})





#original Makefile.am contents follow:

#kde_module_LTLIBRARIES = libkarbonxcfexport.la
#
#libkarbonxcfexport_la_LDFLAGS = $(KDE_PLUGIN)
#libkarbonxcfexport_la_LIBADD = \
#	 \
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
