



########### next target ###############

set(abiwordimport_PART_SRCS 
   abiwordimport.cc 
   ImportHelpers.cc 
   ImportFormatting.cc 
   ImportStyle.cc 
   ImportField.cc )

kde4_automoc(${abiwordimport_PART_SRCS})

kde4_add_plugin(abiwordimport WITH_PREFIX ${abiwordimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} abiwordimport )

target_link_libraries(abiwordimport  ${KDE4_KDECORE_LIBS} kwordexportfilters)

install(TARGETS abiwordimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

set(abiwordexport_PART_SRCS abiwordexport.cc )

kde4_automoc(${abiwordexport_PART_SRCS})

kde4_add_plugin(abiwordexport WITH_PREFIX ${abiwordexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} abiwordexport )

target_link_libraries(abiwordexport  ${KDE4_KDECORE_LIBS} kwordexportfilters)

install(TARGETS abiwordexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kword_abiword_import.desktop kword_abiword_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})





#original Makefile.am contents follow:

######## General stuff
#
#INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) -I$(srcdir)/../libexport $(all_includes)
#
#libabiwordimport_la_LDFLAGS	= $(all_libraries) -module -avoid-version -no-undefined
#libabiwordimport_la_LIBADD = $(KOFFICE_LIBS)
#
#libabiwordexport_la_LDFLAGS	= $(all_libraries) -module -avoid-version -no-undefined
#libabiwordexport_la_LIBADD = $(KOFFICE_LIBS) ../libexport/libkwordexportfilters.la
#
######## Files
#
#kde_module_LTLIBRARIES = libabiwordimport.la libabiwordexport.la
#
#libabiwordimport_la_SOURCES = abiwordimport.cc ImportHelpers.cc\
#ImportFormatting.cc ImportStyle.cc ImportField.cc
#
#libabiwordexport_la_SOURCES = abiwordexport.cc
#
#noinst_HEADERS = abiwordimport.h ImportHelpers.h ImportFormatting.h\
#ImportStyle.h ImportField.h abiwordexport.h
#
#METASOURCES = AUTO
#
#service_DATA = kword_abiword_import.desktop kword_abiword_export.desktop
#servicedir = $(kde_servicesdir)
