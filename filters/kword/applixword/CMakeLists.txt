kde4_header()




########### next target ###############

set(applixwordimport_PART_SRCS applixwordimport.cc )

kde4_automoc(${applixwordimport_PART_SRCS})

kde4_add_plugin(applixwordimport WITH_PREFIX ${applixwordimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} applixwordimport )

target_link_libraries(applixwordimport  ${KDE4_KDECORE_LIBS} kstore )

install_targets(${PLUGIN_INSTALL_DIR} applixwordimport )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  kword_applixword_import.desktop )

kde4_footer()



#original Makefile.am contents follow:

######## General stuff
#
#INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)
#libapplixwordimport_la_LDFLAGS	= $(all_libraries) -module -avoid-version -no-undefined 
#
######## Files
#
#kde_module_LTLIBRARIES = libapplixwordimport.la
#
#libapplixwordimport_la_SOURCES = applixwordimport.cc
#libapplixwordimport_la_LIBADD = $(KOFFICE_LIBS)
#noinst_HEADERS = applixwordimport.h
#
#METASOURCES = AUTO
#
#service_DATA = kword_applixword_import.desktop
#servicedir = $(kde_servicesdir)
