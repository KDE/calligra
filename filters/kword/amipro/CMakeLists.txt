kde4_header()




########### next target ###############

set(amiproimport_PART_SRCS amiproimport.cc amiproparser.cpp )

kde4_automoc(${amiproimport_PART_SRCS})

kde4_add_plugin(amiproimport WITH_PREFIX ${amiproimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} amiproimport )

target_link_libraries(amiproimport  ${KDE4_KDECORE_LIBS} )

install_targets(${PLUGIN_INSTALL_DIR} amiproimport )


########### next target ###############

set(amiproexport_PART_SRCS amiproexport.cc )

kde4_automoc(${amiproexport_PART_SRCS})

kde4_add_plugin(amiproexport WITH_PREFIX ${amiproexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} amiproexport )

target_link_libraries(amiproexport  ${KDE4_KDECORE_LIBS} )

install_targets(${PLUGIN_INSTALL_DIR} amiproexport )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  kword_amipro_import.desktop kword_amipro_export.desktop )

kde4_footer()



#original Makefile.am contents follow:

######## General stuff
#
#INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) -I$(srcdir)/../libexport $(all_includes)
#libamiproimport_la_LDFLAGS	= $(all_libraries) -module -avoid-version -no-undefined
#libamiproimport_la_LIBADD = $(KOFFICE_LIBS)
#
#libamiproexport_la_LDFLAGS      = $(all_libraries) -module -avoid-version -no-undefined
#libamiproexport_la_LIBADD = $(KOFFICE_LIBS) ../libexport/libkwordexportfilters.la
#
######## Files
#
#kde_module_LTLIBRARIES = libamiproimport.la libamiproexport.la
#
#libamiproimport_la_SOURCES = amiproimport.cc amiproparser.cpp
#libamiproexport_la_SOURCES = amiproexport.cc
#
#noinst_HEADERS = amiproparser.h
#
#METASOURCES = AUTO
#
#service_DATA = kword_amipro_import.desktop kword_amipro_export.desktop
#servicedir = $(kde_servicesdir)
