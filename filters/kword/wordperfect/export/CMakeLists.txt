




########### next target ###############

set(wpexport_PART_SRCS wpexport.cc wp5.cc wp6.cc )

kde4_automoc(${wpexport_PART_SRCS})

kde4_add_plugin(wpexport WITH_PREFIX ${wpexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} wpexport )

target_link_libraries(wpexport  ${KDE4_KDECORE_LIBS} kwordexportfilters )

install(TARGETS wpexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kword_wp_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})





#original Makefile.am contents follow:

#
#INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) -I$(srcdir)/../../libexport $(all_includes)
#kde_module_LTLIBRARIES = libwpexport.la
#
#libwpexport_la_SOURCES = wpexport.cc wp5.cc wp6.cc
#libwpexport_la_LDFLAGS	= $(all_libraries) -module -avoid-version -no-undefined
#libwpexport_la_LIBADD = $(KOFFICE_LIBS) ../../libexport/libkwordexportfilters.la
#
#noinst_HEADERS = wpexport.h wp5.h wp6.h
#
#METASOURCES = AUTO
#
#service_DATA = kword_wp_export.desktop
#servicedir = $(kde_servicesdir)
