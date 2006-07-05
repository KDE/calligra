

include_directories( ${CMAKE_SOURCE_DIR}/kpresenter ${CMAKE_SOURCE_DIR}/lib/kotext ${KDE4_INCLUDES}  )


########### next target ###############

set(kpresentersvgexport_PART_SRCS svgexport.cc )

kde4_automoc(${kpresentersvgexport_PART_SRCS})

kde4_add_plugin(kpresentersvgexport WITH_PREFIX ${kpresentersvgexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kpresentersvgexport )

target_link_libraries(kpresentersvgexport  ${KDE4_KDECORE_LIBS} kpresenterprivate )

install(TARGETS kpresentersvgexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kpresenter_svg_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})





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
