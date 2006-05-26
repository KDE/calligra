
include_directories( ${KWMF_INCLUDES} )



########### next target ###############

set(karbonmsodimport_PART_SRCS msodimport.cc msod.cc )

kde4_automoc(${karbonmsodimport_PART_SRCS})

kde4_add_plugin(karbonmsodimport WITH_PREFIX ${karbonmsodimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} karbonmsodimport )

target_link_libraries(karbonmsodimport  ${KDE4_KDECORE_LIBS} kwmf kofficecore)

install_targets(${PLUGIN_INSTALL_DIR} karbonmsodimport )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  karbon_msod_import.desktop )





#original Makefile.am contents follow:

######## General stuff
#
#INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)
#libkarbonmsodimport_la_LDFLAGS	= -module -avoid-version -no-undefined $(all_libraries)
#libkarbonmsodimport_la_LIBADD = $(KOFFICE_LIBS) $(LIBZ)
#
######## Files
#
#kde_module_LTLIBRARIES = libkarbonmsodimport.la
#
#libkarbonmsodimport_la_SOURCES = msodimport.cc msod.cc
#
#noinst_HEADERS = msodimport.h msod.h
#
#METASOURCES = AUTO
#
#service_DATA = karbon_msod_import.desktop
#servicedir = $(kde_servicesdir)
