

include_directories( ${CMAKE_SOURCE_DIR}/kchart ${CMAKE_SOURCE_DIR}/kchart/kdchart ${CMAKE_SOURCE_DIR}/interfaces ${CMAKE_SOURCE_DIR}/filters/libdialogfilter ${CMAKE_SOURCE_DIR}/filters/kchart/libimageexport ${KDE4_INCLUDES}  )


########### next target ###############

set(kchartjpegexport_PART_SRCS jpegexport.cpp ${libdialogfilter_SRCS})

kde4_automoc(${kchartjpegexport_PART_SRCS})

kde4_add_plugin(kchartjpegexport WITH_PREFIX ${kchartjpegexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kchartjpegexport )

target_link_libraries(kchartjpegexport  ${KDE4_KDECORE_LIBS} kchartcommon kchartimageexport  )

install(TARGETS kchartjpegexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kchart_jpeg_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})





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
#kde_module_LTLIBRARIES = libkchartjpegexport.la
#
#libkchartjpegexport_la_SOURCES = jpegexport.cpp
#libkchartjpegexport_la_LDFLAGS = -module $(KDE_PLUGIN) -no-undefined
#libkchartjpegexport_la_LIBADD = ../../../kchart/libkchartcommon.la ../libimageexport/libkchartimageexport.la ../../../filters/libdialogfilter/libdialogfilter.la $(KOFFICE_LIBS)
# 
#noinst_HEADERS = \
#	jpegexport.h
#
#METASOURCES = AUTO
#
#service_DATA = kchart_jpeg_export.desktop
#servicedir = $(kde_servicesdir)
#
