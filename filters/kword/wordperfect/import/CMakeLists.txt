

include_directories(${WPD_INCLUDE_DIR})


########### next target ###############

set(wpimport_PART_SRCS wpimport.cc )

kde4_automoc(${wpimport_PART_SRCS})

kde4_add_plugin(wpimport WITH_PREFIX ${wpimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} wpimport )

target_link_libraries(wpimport  ${KDE4_KDECORE_LIBS} kofficecore ${WPD_LIBRARIES} )

install_targets(${PLUGIN_INSTALL_DIR} wpimport )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  kword_wp_import.desktop  )





#original Makefile.am contents follow:

#INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(LIBWPD_CFLAGS) $(all_includes)
#
#kde_module_LTLIBRARIES = libwpimport.la 
#
#libwpimport_la_SOURCES = wpimport.cc 
#libwpimport_la_LDFLAGS	= $(all_libraries) -module -avoid-version -no-undefined
#libwpimport_la_LIBADD = $(KOFFICE_LIBS) $(LIBWPD_LIBS)
#
#noinst_HEADERS = wpimport.h 
#
#METASOURCES = AUTO
#
#service_DATA = kword_wp_import.desktop 
#servicedir = $(kde_servicesdir)
