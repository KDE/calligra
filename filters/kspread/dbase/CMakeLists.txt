kde4_header()




########### next target ###############

set(dbaseimport_PART_SRCS dbaseimport.cc dbase.cpp )

kde4_automoc(${dbaseimport_PART_SRCS})

kde4_add_plugin(dbaseimport WITH_PREFIX ${dbaseimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} dbaseimport )

target_link_libraries(dbaseimport kofficecore)

install_targets(${PLUGIN_INSTALL_DIR} dbaseimport )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  kspread_dbase_import.desktop )

kde4_footer()



#original Makefile.am contents follow:

######## General stuff
#
#INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)
#
######## Files
#
#kde_module_LTLIBRARIES = libdbaseimport.la
#
#libdbaseimport_la_SOURCES = dbaseimport.cc dbase.cpp
#libdbaseimport_la_LIBADD = $(KOFFICE_LIBS)
#libdbaseimport_la_LDFLAGS = -module $(KDE_PLUGIN)
#noinst_HEADERS = dbaseimport.h dbase.h
#
#METASOURCES = AUTO
#
#service_DATA = kspread_dbase_import.desktop
#servicedir = $(kde_servicesdir)
