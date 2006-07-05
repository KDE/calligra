

include_directories( ${CMAKE_SOURCE_DIR}/kchart ${CMAKE_SOURCE_DIR}/kchart/kdchart ${CMAKE_SOURCE_DIR}/interfaces ${CMAKE_SOURCE_DIR}/filters/libdialogfilter ${CMAKE_SOURCE_DIR}/filters/kchart/libimageexport ${KDE4_INCLUDES}  )


########### next target ###############

set(kchartbmpexport_PART_SRCS bmpexport.cpp ${libdialogfilter_SRCS})

kde4_automoc(${kchartbmpexport_PART_SRCS})

kde4_add_plugin(kchartbmpexport WITH_PREFIX ${kchartbmpexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kchartbmpexport )

target_link_libraries(kchartbmpexport  ${KDE4_KDECORE_LIBS} kchartcommon kchartimageexport  )

install(TARGETS kchartbmpexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kchart_bmp_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})





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
######## Files
#
#kde_module_LTLIBRARIES = libkchartbmpexport.la
#
#libkchartbmpexport_la_SOURCES = bmpexport.cpp
#libkchartbmpexport_la_LDFLAGS = -module $(KDE_PLUGIN) -no-undefined
#libkchartbmpexport_la_LIBADD = ../../../kchart/libkchartcommon.la ../libimageexport/libkchartimageexport.la ../../../filters/libdialogfilter/libdialogfilter.la $(KOFFICE_LIBS)
#noinst_HEADERS = \
#	bmpexport.h
#
#METASOURCES = AUTO
#
#service_DATA = kchart_bmp_export.desktop
#servicedir = $(kde_servicesdir)
#
