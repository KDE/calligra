kde4_header()

include_directories( ${CMAKE_SOURCE_DIR}/karbon ${CMAKE_SOURCE_DIR}/karbon/core ${CMAKE_SOURCE_DIR}/karbon/commands ${KDE4_INCLUDES}  )


########### next target ###############

set(wmfimport_PART_SRCS wmfimport.cc wmfimportparser.cc )

kde4_automoc(${wmfimport_PART_SRCS})

kde4_add_plugin(wmfimport WITH_PREFIX ${wmfimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} wmfimport )

target_link_libraries(wmfimport  ${KDE4_KDECORE_LIBS} kowmf karboncommon )

install_targets(${PLUGIN_INSTALL_DIR} wmfimport )


########### next target ###############

set(wmfexport_PART_SRCS wmfexport.cc )

kde4_automoc(${wmfexport_PART_SRCS})

kde4_add_plugin(wmfexport WITH_PREFIX ${wmfexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} wmfexport )

target_link_libraries(wmfexport  ${KDE4_KDECORE_LIBS} kowmf karboncommon )

install_targets(${PLUGIN_INSTALL_DIR} wmfexport )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  karbon_wmf_import.desktop karbon_wmf_export.desktop )

kde4_footer()



#original Makefile.am contents follow:

#
#INCLUDES= \
#	-I$(srcdir) \
#	$(KWMF_INCLUDES) \
#	$(KOFFICE_INCLUDES) \
#	$(KOPAINTER_INCLUDES) \
#	-I$(top_srcdir)/karbon \
#	-I$(top_srcdir)/karbon/core \
#	-I$(top_srcdir)/karbon/commands \
#	$(all_includes)
#
#kde_module_LTLIBRARIES = libwmfimport.la libwmfexport.la
#
#libwmfimport_la_LDFLAGS = -module $(KDE_PLUGIN)
#libwmfimport_la_LIBADD = $(KOFFICE_LIBS) $(LIB_KOWMF) ../../../karbon/libkarboncommon.la
#libwmfimport_la_SOURCES = wmfimport.cc wmfimportparser.cc
#
#libwmfexport_la_LDFLAGS = -module $(KDE_PLUGIN)
#libwmfexport_la_LIBADD = $(KOFFICE_LIBS) $(LIB_KOWMF) ../../../karbon/libkarboncommon.la
#libwmfexport_la_SOURCES = wmfexport.cc
#
#noinst_HEADERS = wmfimport.h wmfimportparser.h wmfexport.h 
#
#METASOURCES = AUTO
#
#service_DATA = karbon_wmf_import.desktop karbon_wmf_export.desktop
#servicedir = $(kde_servicesdir)
