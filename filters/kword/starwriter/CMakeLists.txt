




########### next target ###############

set(starwriterimport_PART_SRCS pole.cpp starwriterimport.cc )

kde4_automoc(${starwriterimport_PART_SRCS})

kde4_add_plugin(starwriterimport WITH_PREFIX ${starwriterimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} starwriterimport )

target_link_libraries(starwriterimport  ${KDE4_KDECORE_LIBS} )

install(TARGETS starwriterimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kword_starwriter_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})





#original Makefile.am contents follow:

######## General stuff
#
#INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)
#
######## Files
#
#kde_module_LTLIBRARIES = libstarwriterimport.la
#
#libstarwriterimport_la_SOURCES = pole.cpp starwriterimport.cc
#libstarwriterimport_la_LDFLAGS	= -module $(KDE_PLUGIN)
#libstarwriterimport_la_LIBADD = $(KOFFICE_LIBS)
#
#METASOURCES = AUTO
#
#service_DATA = kword_starwriter_import.desktop
#servicedir = $(kde_servicesdir)
