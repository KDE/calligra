kde4_header()

include_directories(${CMAKE_SOURCE_DIR}/filters/liboofilter/)

set(libcommon_SRCS conversion.cc)



########### next target ###############

set(oowriterimport_PART_SRCS oowriterimport.cc ${libcommon_SRCS} ${liboofilter_SRCS})

kde4_automoc(${oowriterimport_PART_SRCS})

kde4_add_plugin(oowriterimport WITH_PREFIX ${oowriterimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} oowriterimport )

target_link_libraries(oowriterimport  ${KDE4_KDECORE_LIBS}   )

install_targets(${PLUGIN_INSTALL_DIR} oowriterimport )


########### next target ###############

set(oowriterexport_PART_SRCS oowriterexport.cc ExportFilter.cc ${libcommon_SRCS} ${liboofilter_SRCS})

kde4_automoc(${oowriterexport_PART_SRCS})

kde4_add_plugin(oowriterexport WITH_PREFIX ${oowriterexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} oowriterexport )

target_link_libraries(oowriterexport  ${KDE4_KDECORE_LIBS} kwordexportfilters  )

install_targets(${PLUGIN_INSTALL_DIR} oowriterexport )


########### next target ###############


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  kword_oowriter_import.desktop kword_oowriter_export.desktop )

kde4_footer()



#original Makefile.am contents follow:

######## General stuff
#
#INCLUDES= -I$(srcdir)/../../liboofilter -I$(srcdir)/../libexport \
#$(KOFFICE_INCLUDES) $(all_includes)
#
######## Files
#kde_module_LTLIBRARIES = liboowriterimport.la liboowriterexport.la
#
#noinst_LTLIBRARIES = libcommon.la
#libcommon_la_SOURCES = conversion.cc
#
#liboowriterimport_la_SOURCES = oowriterimport.cc
#liboowriterimport_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
#liboowriterimport_la_LIBADD  = ../../liboofilter/liboofilter.la libcommon.la $(KOFFICE_LIBS)
#
#liboowriterexport_la_SOURCES = oowriterexport.cc ExportFilter.cc
#liboowriterexport_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
#liboowriterexport_la_LIBADD  = $(KOFFICE_LIBS) ../libexport/libkwordexportfilters.la libcommon.la
#
#noinst_HEADERS = ExportFilter.h
#
#METASOURCES = AUTO
#
#service_DATA = kword_oowriter_import.desktop kword_oowriter_export.desktop
#
#servicedir = $(kde_servicesdir)
