




########### next target ###############

set(applixspreadimport_PART_SRCS applixspreadimport.cc )

kde4_automoc(${applixspreadimport_PART_SRCS})

kde4_add_plugin(applixspreadimport WITH_PREFIX ${applixspreadimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} applixspreadimport )

target_link_libraries(applixspreadimport  ${KDE4_KDECORE_LIBS} kofficecore )

install(TARGETS applixspreadimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kspread_applixspread_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})





#original Makefile.am contents follow:

######## General stuff
#
#INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)
#
######## Files
#
#kde_module_LTLIBRARIES = libapplixspreadimport.la
#
#libapplixspreadimport_la_SOURCES = applixspreadimport.cc
#libapplixspreadimport_la_LIBADD = $(KOFFICE_LIBS)
#libapplixspreadimport_la_LDFLAGS = -module $(KDE_PLUGIN)
#noinst_HEADERS = applixspreadimport.h
#
#METASOURCES = AUTO
#
#service_DATA = kspread_applixspread_import.desktop
#servicedir = $(kde_servicesdir)
