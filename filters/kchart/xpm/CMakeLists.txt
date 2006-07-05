

include_directories( ${CMAKE_SOURCE_DIR}/kchart ${CMAKE_SOURCE_DIR}/kchart/kdchart ${CMAKE_SOURCE_DIR}/interfaces ${CMAKE_SOURCE_DIR}/filters/libdialogfilter ${CMAKE_SOURCE_DIR}/filters/kchart/libimageexport ${KDE4_INCLUDES}  )


########### next target ###############

set(kchartxpmexport_PART_SRCS xpmexport.cpp ${libdialogfilter_SRCS})

kde4_automoc(${kchartxpmexport_PART_SRCS})

kde4_add_plugin(kchartxpmexport WITH_PREFIX ${kchartxpmexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kchartxpmexport )

target_link_libraries(kchartxpmexport  ${KDE4_KDECORE_LIBS} kchartcommon kchartimageexport  )

install(TARGETS kchartxpmexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kchart_xpm_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})





#original Makefile.am contents follow:

######## General stuff
#
#INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) \
#	-I$(top_srcdir)/kchart \
#	-I$(top_srcdir)/kchart/kdchart \
#	-I$(top_srcdir)/interfaces \
#	-I$(top_srcdir)/filters/libdialogfilter \
#	-I$(top_srcdir)/filters/kchart/libimageexport \
#	$(all_includes)
#
######## Files
#
#kde_module_LTLIBRARIES = libkchartxpmexport.la
#
#libkchartxpmexport_la_SOURCES = xpmexport.cpp
#libkchartxpmexport_la_LDFLAGS = -module $(KDE_PLUGIN) -no-undefined
#libkchartxpmexport_la_LIBADD = ../../../kchart/libkchartcommon.la ../libimageexport/libkchartimageexport.la ../../../filters/libdialogfilter/libdialogfilter.la $(KOFFICE_LIBS)
#noinst_HEADERS = \
#	xpmexport.h
#
#METASOURCES = AUTO
#
#service_DATA = kchart_xpm_export.desktop
#servicedir = $(kde_servicesdir)
#
