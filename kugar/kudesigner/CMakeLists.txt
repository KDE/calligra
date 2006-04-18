kde4_header()

add_subdirectory( toolbar )
add_subdirectory( pics )
add_subdirectory( templates )




########### next target ###############

set(kudesignercore_LIB_SRCS ${libkudesignercommon_SRCS} kudesigner_view.cpp kudesigner_factory.cpp kudesigner_doc.cpp )

kde4_automoc(${kudesignercore_LIB_SRCS})

kde4_add_library(kudesignercore SHARED ${kudesignercore_LIB_SRCS})

target_link_libraries(kudesignercore  ${KDE4_KDECORE_LIBS} kofficeui ${KDE4_KPARTS_LIBS} kofficecore koproperty ${KDE4_KIO_LIBS} ${KDE4_KDEPRINT_LIBS} )

set_target_properties(kudesignercore PROPERTIES VERSION 1.0.0 SOVERSION 1 )
install_targets(${LIB_INSTALL_DIR} kudesignercore )


########### next target ###############

set(kudesignerpart_PART_SRCS kudesigner_part.cpp )

kde4_automoc(${kudesignerpart_PART_SRCS})

kde4_add_plugin(kudesignerpart WITH_PREFIX ${kudesignerpart_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kudesignerpart )

target_link_libraries(kudesignerpart  ${KDE4_KDECORE_LIBS} kudesignercore kofficeui ${KDE4_KPARTS_LIBS} kofficecore )

install_targets(${PLUGIN_INSTALL_DIR} kudesignerpart )


########### next target ###############

set(kudesigner_KDEINIT_SRCS main.cpp )

kde4_automoc(${kudesigner_KDEINIT_SRCS})

kde4_add_kdeinit_executable( kudesigner ${kudesigner_KDEINIT_SRCS})

target_link_libraries(kdeinit_kudesigner  ${KDE4_KDECORE_LIBS} kofficecore )

install_targets(${LIB_INSTALL_DIR} kdeinit_kudesigner )

target_link_libraries( kudesigner kdeinit_kudesigner )
install_targets(/bin kudesigner )

########### install files ###############

install_files( ${APPLNK_INSTALL_DIR}/Office FILES  kudesigner.desktop )
install_files( ${DATA_INSTALL_DIR}/kudesigner FILES  kudesignerui.rc kudesigner_readonly.rc )

kde4_footer()



#original Makefile.am contents follow:

#SUBDIRS = toolbar pics templates
#
#INCLUDES = -I$(srcdir)/../kudesigner_lib -I$(srcdir)/../lib \
#	$(KOPROPERTY_INCLUDES) $(KOFFICE_INCLUDES) $(all_includes)
#
#bin_PROGRAMS =
#
#lib_LTLIBRARIES = libkudesignercore.la
#libkudesignercore_la_SOURCES = kudesigner_view.cpp kudesigner_factory.cpp \
#	kudesigner_doc.cpp
#libkudesignercore_la_LDFLAGS = -avoid-version $(all_libraries) $(KDE_RPATH) -no-undefined
#libkudesignercore_la_LIBADD = ../kudesigner_lib/libkudesignercommon.la $(LIB_KOFFICEUI) $(LIB_KPARTS) $(LIB_KOFFICECORE) $(LIB_KOPROPERTY) $(LIB_KIO) $(LIB_KDEPRINT)
#
### The part
#kde_module_LTLIBRARIES = libkudesignerpart.la
#libkudesignerpart_la_SOURCES = kudesigner_part.cpp
#libkudesignerpart_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) $(KDE_RPATH)
#libkudesignerpart_la_LIBADD = libkudesignercore.la $(LIB_KOFFICEUI) $(LIB_KPARTS) $(LIB_KOFFICECORE)
#
### The kdeinit loadable module
#kdeinit_LTLIBRARIES = kudesigner.la
#kudesigner_la_SOURCES = main.cpp
#kudesigner_la_LDFLAGS = $(all_libraries) -module -avoid-version
#kudesigner_la_LIBADD = $(LIB_KOFFICECORE)
#
#appsdir = $(kde_appsdir)/Office
#apps_DATA = kudesigner.desktop
#
#METASOURCES = AUTO
#
#rcdir = $(kde_datadir)/kudesigner
#rc_DATA = kudesignerui.rc kudesigner_readonly.rc
#
#EXTRA_DIST = \
#	kudesigner_aboutdata.h \
#	kudesigner_factory.h \
#	kudesigner_part.h \
#	kudesigner_view.h \
#	$(rc_DATA) \
#	$(apps_DATA)
#
#KDE_CXXFLAGS = $(USE_EXCEPTIONS) -DKPROPERTY_EXPORT=
#
