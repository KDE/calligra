kde4_header()




########### next target ###############

set(ooimpressimport_PART_SRCS ooimpressimport.cc )

kde4_automoc(${ooimpressimport_PART_SRCS})

kde4_add_plugin(ooimpressimport WITH_PREFIX ${ooimpressimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} ooimpressimport )

target_link_libraries(ooimpressimport  ${KDE4_KDECORE_LIBS} oofilter )

install_targets(${PLUGIN_INSTALL_DIR} ooimpressimport )


########### next target ###############

set(ooimpressexport_PART_SRCS ooimpressexport.cc stylefactory.cc )

kde4_automoc(${ooimpressexport_PART_SRCS})

kde4_add_plugin(ooimpressexport WITH_PREFIX ${ooimpressexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} ooimpressexport )

target_link_libraries(ooimpressexport  ${KDE4_KDECORE_LIBS} oofilter )

install_targets(${PLUGIN_INSTALL_DIR} ooimpressexport )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  kpresenter_ooimpress_import.desktop kpresenter_ooimpress_export.desktop )

kde4_footer()



#original Makefile.am contents follow:

######## General stuff
#
#INCLUDES= -I$(srcdir)/../../liboofilter $(KOFFICE_INCLUDES) $(all_includes)
#
######## Files
#kde_module_LTLIBRARIES = libooimpressimport.la libooimpressexport.la
#
#
#libooimpressimport_la_SOURCES = ooimpressimport.cc
#libooimpressimport_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
#libooimpressimport_la_LIBADD  = ../../liboofilter/liboofilter.la $(KOFFICE_LIBS)
#
#libooimpressexport_la_SOURCES = ooimpressexport.cc stylefactory.cc
#libooimpressexport_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
#libooimpressexport_la_LIBADD  = ../../liboofilter/liboofilter.la $(KOFFICE_LIBS)
#
#METASOURCES = AUTO
#
#service_DATA = kpresenter_ooimpress_import.desktop kpresenter_ooimpress_export.desktop
#
#servicedir = $(kde_servicesdir)
