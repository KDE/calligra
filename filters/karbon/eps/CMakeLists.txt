

########### next target ###############

set(karbonepsexport_PART_SRCS epsexport.cc epsexportdlg.cc )

kde4_automoc(${karbonepsexport_PART_SRCS})

kde4_add_plugin(karbonepsexport WITH_PREFIX ${karbonepsexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} karbonepsexport )

target_link_libraries(karbonepsexport  ${KDE4_KDECORE_LIBS} kofficeui karboncommon )

install_targets(${PLUGIN_INSTALL_DIR} karbonepsexport )


########### next target ###############

set(karbonepsimport_PART_SRCS epsimport.cc pscommentlexer.cc )

kde4_automoc(${karbonepsimport_PART_SRCS})

kde4_add_plugin(karbonepsimport WITH_PREFIX ${karbonepsimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} karbonepsimport )

target_link_libraries(karbonepsimport  ${KDE4_KDECORE_LIBS} kofficeui )

install_targets(${PLUGIN_INSTALL_DIR} karbonepsimport )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  karbon_eps_export.desktop  
	karbon_eps_import.desktop
  	karbon_ps_import.desktop )





#original Makefile.am contents follow:

#kde_module_LTLIBRARIES = \
#	libkarbonepsexport.la \
#	libkarbonepsimport.la
#
#libkarbonepsexport_la_SOURCES = \
#	epsexport.cc \
#	epsexportdlg.cc
#
#libkarbonepsexport_la_LDFLAGS = $(KDE_PLUGIN)
#libkarbonepsexport_la_LIBADD  = $(LIB_KOFFICEUI) ../../../karbon/libkarboncommon.la
#
#libkarbonepsimport_la_SOURCES = epsimport.cc pscommentlexer.cc
#libkarbonepsimport_la_LDFLAGS = $(KDE_PLUGIN)
#libkarbonepsimport_la_LIBADD  = $(LIB_KOFFICEUI)
#
#INCLUDES = \
#	$(KOFFICE_INCLUDES) \
#	$(KOPAINTER_INCLUDES) \
#	-I$(top_srcdir)/karbon \
#	-I$(top_srcdir)/karbon/core \
#	-I$(top_srcdir)/karbon/visitors \
#	$(all_includes)
#
#service_DATA = \
#	karbon_eps_export.desktop \
#	karbon_eps_import.desktop \
#	karbon_ps_import.desktop
#
#servicedir = $(kde_servicesdir)
#
#noinst_HEADERS = \
#	epsexport.h \
#	epsexportdlg.h \
#	epsimport.h \
#	pscommentlexer.h
#
#METASOURCES = AUTO
