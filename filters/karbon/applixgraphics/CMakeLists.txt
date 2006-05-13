




########### next target ###############

set(applixgraphicimport_PART_SRCS applixgraphicimport.cc )

kde4_automoc(${applixgraphicimport_PART_SRCS})

kde4_add_plugin(applixgraphicimport WITH_PREFIX ${applixgraphicimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} applixgraphicimport )

target_link_libraries(applixgraphicimport  ${KDE4_KDECORE_LIBS} )

install_targets(${PLUGIN_INSTALL_DIR} applixgraphicimport )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  kontour_applixgraphic_import.desktop )





#original Makefile.am contents follow:

######## General stuff
#
#INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)
#libapplixgraphicimport_la_LDFLAGS	= -module -avoid-version -no-undefined $(all_libraries)
#
######## Files
#
#kde_module_LTLIBRARIES = libapplixgraphicimport.la
#
#libapplixgraphicimport_la_SOURCES = applixgraphicimport.cc
#libapplixgraphicimport_la_LIBADD = $(KOFFICE_LIBS)
#noinst_HEADERS = applixgraphicimport.h
#
#METASOURCES = AUTO
#
#service_DATA = kontour_applixgraphic_import.desktop
#servicedir = $(kde_servicesdir)
#
#messages: rc.cpp
#	$(XGETTEXT) *.cc *.cpp -o $(podir)/kontourapplixgraphicsfilter.pot
