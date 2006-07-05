




########### next target ###############

set(wmlimport_PART_SRCS wmlimport.cc wmlparser.cpp )

kde4_automoc(${wmlimport_PART_SRCS})

kde4_add_plugin(wmlimport WITH_PREFIX ${wmlimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} wmlimport )

target_link_libraries(wmlimport  ${KDE4_KDECORE_LIBS} kofficecore kwordexportfilters)

install(TARGETS wmlimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

set(wmlexport_PART_SRCS wmlexport.cc )

kde4_automoc(${wmlexport_PART_SRCS})

kde4_add_plugin(wmlexport WITH_PREFIX ${wmlexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} wmlexport )

target_link_libraries(wmlexport  ${KDE4_KDECORE_LIBS} kwordexportfilters kofficecore )

install(TARGETS wmlexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kword_wml_import.desktop kword_wml_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})





#original Makefile.am contents follow:

######## General stuff
#
#INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) -I$(srcdir)/../libexport $(all_includes)
#libwmlimport_la_LDFLAGS	= $(all_libraries) -module -avoid-version -no-undefined
#libwmlimport_la_LIBADD = $(KOFFICE_LIBS)
#libwmlexport_la_LDFLAGS	= $(all_libraries) -module -avoid-version -no-undefined
#libwmlexport_la_LIBADD = $(KOFFICE_LIBS) ../libexport/libkwordexportfilters.la
#
######## Files
#
#kde_module_LTLIBRARIES = libwmlimport.la libwmlexport.la
#
#libwmlimport_la_SOURCES = wmlimport.cc wmlparser.cpp
#libwmlexport_la_SOURCES = wmlexport.cc
#
#noinst_HEADERS = wmlparser.h
#
#METASOURCES = AUTO
#
#service_DATA = kword_wml_import.desktop kword_wml_export.desktop
#servicedir = $(kde_servicesdir)
