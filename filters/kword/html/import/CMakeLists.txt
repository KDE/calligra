kde4_header()




########### next target ###############

set(htmlimport_PART_SRCS htmlimport.cpp khtmlreader.cpp kwdwriter.cpp )

kde4_automoc(${htmlimport_PART_SRCS})

kde4_add_plugin(htmlimport WITH_PREFIX ${htmlimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} htmlimport )

target_link_libraries(htmlimport  ${KDE4_KDECORE_LIBS} )

install_targets(${PLUGIN_INSTALL_DIR} htmlimport )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  kword_html_import.desktop )

kde4_footer()



#original Makefile.am contents follow:

######## General stuff
#
#INCLUDES = -I$(srcdir) $(KOFFICE_INCLUDES) $(KOTEXT_INCLUDES) $(all_includes)
#
#libhtmlimport_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
#
#libhtmlimport_la_LIBADD = $(LIB_KHTML) $(KOFFICE_LIBS) $(LIB_KOTEXT)
#
######## Files
#
#kde_module_LTLIBRARIES = libhtmlimport.la
#
#libhtmlimport_la_SOURCES = htmlimport.cpp khtmlreader.cpp kwdwriter.cpp
#
#noinst_HEADERS = htmlimport.h khtmlreader.h kwdwriter.h
#
#METASOURCES = AUTO
#
#service_DATA = kword_html_import.desktop
#servicedir = $(kde_servicesdir)
