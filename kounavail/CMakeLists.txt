kde4_header()

include_directories( ${KOFFICECORE_INCLUDES} )


########### next target ###############

set(kounavailpart_PART_SRCS kounavail.cc )

kde4_automoc(${kounavailpart_PART_SRCS})

kde4_add_plugin(kounavailpart WITH_PREFIX ${kounavailpart_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kounavailpart )

target_link_libraries(kounavailpart  ${KDE4_KDECORE_LIBS} kofficecore )

install_targets(${PLUGIN_INSTALL_DIR} kounavailpart )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  kounavail.desktop )

kde4_footer()



#original Makefile.am contents follow:

#INCLUDES  = $(KOFFICE_INCLUDES) $(all_includes)
#
### The part
#kde_module_LTLIBRARIES = libkounavailpart.la
#libkounavailpart_la_SOURCES = kounavail.cc
#libkounavailpart_la_LDFLAGS = $(KDE_PLUGIN)
#libkounavailpart_la_LIBADD = $(LIB_KOFFICECORE)
#
#METASOURCES = AUTO
#noinst_HEADERS =  kounavail.h
#
#kdelnk_DATA = kounavail.desktop
#kdelnkdir = $(kde_servicesdir)
#
#messages:
#	$(XGETTEXT) *.cc -o $(podir)/kounavail.pot
