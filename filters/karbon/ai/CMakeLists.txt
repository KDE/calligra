

########### next target ###############

set(karbonaiimport_PART_SRCS 
   aicolor.cc 
   aielement.cc 
   aiimport.cc 
   ailexer.cc 
   aiparserbase.cc 
   ai88handler.cc 
   ai3handler.cc 
   karbonaiparserbase.cc )

kde4_automoc(${karbonaiimport_PART_SRCS})

kde4_add_plugin(karbonaiimport WITH_PREFIX ${karbonaiimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} karbonaiimport )

target_link_libraries(karbonaiimport  ${KDE4_KDECORE_LIBS} kofficeui karboncommon )

install_targets(${PLUGIN_INSTALL_DIR} karbonaiimport )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  karbon_ai_import.desktop )





#original Makefile.am contents follow:

#kde_module_LTLIBRARIES = libkarbonaiimport.la
#
#libkarbonaiimport_la_LIBADD = \
#	$(LIB_KOFFICEUI) \
#	../../../karbon/libkarboncommon.la
#
#libkarbonaiimport_la_LDFLAGS = \
#	$(all_libraries) \
#	$(KDE_RPATH) \
#	-module -avoid-version -no-undefined
#
#INCLUDES = \
#	$(KOFFICE_INCLUDES) \
#	$(KOPAINTER_INCLUDES) \
#	-I$(top_srcdir)/karbon \
#	-I$(top_srcdir)/karbon/core \
#	$(all_includes)
#
#service_DATA = karbon_ai_import.desktop
#servicedir = $(kde_servicesdir)
#
#noinst_HEADERS = \
#	aicolor.h \
#	aielement.h \
#	aiimport.h \
#	ailexer.h \
#	aiparserbase.h \
#	ai88handler.h \
#	ai3handler.h \
#	karbonaiparserbase.h
#
#libkarbonaiimport_la_SOURCES = \
#	aicolor.cc \
#	aielement.cc \
#	aiimport.cc \
#	ailexer.cc \
#	aiparserbase.cc \
#	ai88handler.cc \
#	ai3handler.cc \
#	karbonaiparserbase.cc
#
#libkarbonaiimport_la_METASOURCES = \
#	AUTO
#
