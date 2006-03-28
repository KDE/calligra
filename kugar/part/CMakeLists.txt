kde4_header()




########### next target ###############

set(kugarpart_PART_SRCS kugar_part.cpp kugar_factory.cpp kugar_view.cpp )

kde4_automoc(${kugarpart_PART_SRCS})

kde4_add_plugin(kugarpart WITH_PREFIX ${kugarpart_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kugarpart )

target_link_libraries(kugarpart  ${KDE4_KDECORE_LIBS} kugarlib ${KDE4_KPARTS_LIBS} ${KDE4_KDEPRINT_LIBS} kofficecore )

install_targets(${LIB_INSTALL_DIR}/kde4 kugarpart )


########### next target ###############

set(kugar_KDEINIT_SRCS main.cpp )

kde4_automoc(${kugar_KDEINIT_SRCS})

kde4_add_kdeinit_executable( kugar ${kugar_KDEINIT_SRCS})

target_link_libraries(kdeinit_kugar  ${KDE4_KDECORE_LIBS} kofficecore )

install_targets(${LIB_INSTALL_DIR} kdeinit_kugar )

target_link_libraries( kugar kdeinit_kugar )
install_targets(/bin kugar )

########### install files ###############

install_files( ${APPLNK_INSTALL_DIR}/Office FILES  kugar.desktop )
install_files( ${SERVICES_INSTALL_DIR} FILES  kugarpart.desktop )
install_files( ${DATA_INSTALL_DIR}/kugar FILES  kugarpart.rc )
install_files( ${DATA_INSTALL_DIR}/kugar/dtd FILES  kugartemplate.dtd )

kde4_footer()



#original Makefile.am contents follow:

#INCLUDES = -I$(srcdir)/../lib $(KOFFICE_INCLUDES) $(all_includes)
#
#lib_LTLIBRARIES = 
#kde_module_LTLIBRARIES = libkugarpart.la
#libkugarpart_la_SOURCES = kugar_part.cpp kugar_factory.cpp kugar_view.cpp
#libkugarpart_la_LDFLAGS = $(all_libraries) -module -avoid-version
#libkugarpart_la_LIBADD = ../lib/libkugarlib.la $(LIB_KPARTS) $(LIB_KDEPRINT) $(LIB_KOFFICECORE)
#
#libkugarpart_la_METASOURCES = AUTO
#
#appsdir = $(kde_appsdir)/Office
#apps_DATA = kugar.desktop
#
#rcdir = $(kde_datadir)/kugar
#rc_DATA = kugarpart.rc
#
#dtddir = $(kde_datadir)/kugar/dtd
#dtd_DATA = kugartemplate.dtd
#
#EXTRA_DIST = \
#	kugar_part.h \
#	$(apps_DATA) \
#	$(rc_DATA) \
#	$(dtd_DATA)
#
#CLEANFILES = kugarpart_meta_unload.cpp
#
#
### The kdeinit loadable module and executable
#kdeinit_LTLIBRARIES = kugar.la
#bin_PROGRAMS =
#kugar_la_SOURCES = main.cpp
#kugar_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#kugar_la_LIBADD = $(LIB_KOFFICECORE)
#
#kde_services_DATA = kugarpart.desktop
