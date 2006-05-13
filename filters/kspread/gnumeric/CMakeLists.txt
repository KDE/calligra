

include_directories( ${CMAKE_SOURCE_DIR}/kspread ${KDE4_INCLUDES}  )


########### next target ###############

set(gnumericexport_PART_SRCS gnumericexport.cc )

kde4_automoc(${gnumericexport_PART_SRCS})

kde4_add_plugin(gnumericexport WITH_PREFIX ${gnumericexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} gnumericexport )

target_link_libraries(gnumericexport  ${KDE4_KDECORE_LIBS} kspreadcommon )

install_targets(${PLUGIN_INSTALL_DIR} gnumericexport )


########### next target ###############

set(gnumericimport_PART_SRCS gnumericimport.cc )

kde4_automoc(${gnumericimport_PART_SRCS})

kde4_add_plugin(gnumericimport WITH_PREFIX ${gnumericimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} gnumericimport )

target_link_libraries(gnumericimport  ${KDE4_KDECORE_LIBS} kspreadcommon )

install_targets(${PLUGIN_INSTALL_DIR} gnumericimport )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  kspread_gnumeric_export.desktop kspread_gnumeric_import.desktop )





#original Makefile.am contents follow:

######## General stuff
#
#INCLUDES= -I$(srcdir) -I$(top_srcdir)/kspread $(KOFFICE_INCLUDES) \
#	$(KOTEXT_INCLUDES) $(all_includes)
#
######## Files
#kde_module_LTLIBRARIES = libgnumericexport.la libgnumericimport.la
#
#libgnumericexport_la_SOURCES = gnumericexport.cc
#libgnumericexport_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
#libgnumericexport_la_LIBADD  = $(KOFFICE_LIBS) ../../../kspread/libkspreadcommon.la
#
#libgnumericimport_la_SOURCES = gnumericimport.cc
#libgnumericimport_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
#libgnumericimport_la_LIBADD  = $(KOFFICE_LIBS) ../../../kspread/libkspreadcommon.la
#
#METASOURCES = AUTO
#
#service_DATA = kspread_gnumeric_export.desktop kspread_gnumeric_import.desktop
#
#servicedir = $(kde_servicesdir)
