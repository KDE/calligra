

include_directories( 
		${CMAKE_SOURCE_DIR}/filters/liboofilter
)

########### next target ###############

set(oodrawimport_PART_SRCS oodrawimport.cc ${liboofilter_SRCS})

kde4_automoc(${oodrawimport_PART_SRCS})

kde4_add_plugin(oodrawimport WITH_PREFIX ${oodrawimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} oodrawimport )

target_link_libraries(oodrawimport  ${KDE4_KDECORE_LIBS}  karboncommon )

install(TARGETS oodrawimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  karbon_oodraw_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})





#original Makefile.am contents follow:

######## General stuff
#
#INCLUDES= -I$(srcdir)/../../liboofilter \
#	$(KOFFICE_INCLUDES) \
#	$(KOPAINTER_INCLUDES) \
#	-I$(top_srcdir)/karbon \
#	-I$(top_srcdir)/karbon/core \
#	$(all_includes)
#
######## Files
#kde_module_LTLIBRARIES = liboodrawimport.la
#
#
#liboodrawimport_la_SOURCES = oodrawimport.cc
#liboodrawimport_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
#liboodrawimport_la_LIBADD  = ../../liboofilter/liboofilter.la \
#							../../../karbon/libkarboncommon.la \
#							$(KOFFICE_LIBS)
#
#METASOURCES = AUTO
#
#service_DATA = karbon_oodraw_import.desktop
#
#servicedir = $(kde_servicesdir)
