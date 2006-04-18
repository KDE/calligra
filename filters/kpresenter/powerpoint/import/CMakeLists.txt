kde4_header()




########### next target ###############

set(powerpointimport_PART_SRCS powerpointimport.cc )

kde4_automoc(${powerpointimport_PART_SRCS})

kde4_add_plugin(powerpointimport WITH_PREFIX ${powerpointimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} powerpointimport )

target_link_libraries(powerpointimport  ${KDE4_KDECORE_LIBS} ppt )

install_targets(${PLUGIN_INSTALL_DIR} powerpointimport )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  kpresenter_powerpoint_import.desktop )

kde4_footer()



#original Makefile.am contents follow:

#INCLUDES = -I$(srcdir) -I$(srcdir)/../libppt $(KOFFICE_INCLUDES) $(all_includes)
#
#kde_module_LTLIBRARIES = libpowerpointimport.la
#
#libpowerpointimport_la_SOURCES = powerpointimport.cc
#libpowerpointimport_la_LIBADD  = $(KOFFICE_LIBS) ../libppt/libppt.la
#libpowerpointimport_la_LDFLAGS = -module $(KDE_PLUGIN)
#noinst_HEADERS = powerpointimport.h
#
#METASOURCES = AUTO
#
#service_DATA = kpresenter_powerpoint_import.desktop
#servicedir = $(kde_servicesdir)
#
