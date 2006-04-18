kde4_header()

include_directories( ${CMAKE_SOURCE_DIR}/kpresenter ${CMAKE_SOURCE_DIR}/lib/kotext ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kpresentersvgexport_PART_SRCS svgexport.cc )

kde4_automoc(${kpresentersvgexport_PART_SRCS})

kde4_add_plugin(kpresentersvgexport WITH_PREFIX ${kpresentersvgexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kpresentersvgexport )

target_link_libraries(kpresentersvgexport  ${KDE4_KDECORE_LIBS} kpresenterprivate )

install_targets(${PLUGIN_INSTALL_DIR} kpresentersvgexport )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  kpresenter_svg_export.desktop )

kde4_footer()



#original Makefile.am contents follow:

######## General stuff
#
#INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) \
#        -I$(top_srcdir)/kpresenter \
#        -I$(top_srcdir)/lib/kotext \
#	$(all_includes)
#
######## Files
#kde_module_LTLIBRARIES = libkpresentersvgexport.la
#
#libkpresentersvgexport_la_SOURCES = svgexport.cc
#libkpresentersvgexport_la_LDFLAGS = -module $(KDE_PLUGIN) -no-undefined
#libkpresentersvgexport_la_LIBADD = ../../../kpresenter/libkpresenterprivate.la $(KOFFICE_LIBS)
#noinst_HEADERS = \
#	svgexport.h
#
#METASOURCES = AUTO
#
#service_DATA = kpresenter_svg_export.desktop
#servicedir = $(kde_servicesdir)
#
