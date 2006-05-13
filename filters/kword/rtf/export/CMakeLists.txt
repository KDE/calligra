




########### next target ###############

set(rtfexport_PART_SRCS rtfexport.cc ExportFilter.cc )

kde4_automoc(${rtfexport_PART_SRCS})

kde4_add_plugin(rtfexport WITH_PREFIX ${rtfexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} rtfexport )

target_link_libraries(rtfexport  ${KDE4_KDECORE_LIBS} kwordexportfilters )

install_targets(${PLUGIN_INSTALL_DIR} rtfexport )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  kword_rtf_export.desktop )





#original Makefile.am contents follow:

######## General stuff
#
#INCLUDES = -I$(srcdir) $(KOFFICE_INCLUDES) -I$(srcdir)/../../libexport $(all_includes)
#
#librtfexport_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
#
#librtfexport_la_LIBADD = $(KOFFICE_LIBS) ../../libexport/libkwordexportfilters.la
#
######## Files
#
#kde_module_LTLIBRARIES = librtfexport.la
#
#librtfexport_la_SOURCES = rtfexport.cc ExportFilter.cc
#
#noinst_HEADERS = rtfexport.h ExportFilter.h
#
#METASOURCES = AUTO
#
#service_DATA = kword_rtf_export.desktop
#servicedir = $(kde_servicesdir)
