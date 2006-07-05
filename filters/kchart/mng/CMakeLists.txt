

include_directories( ${CMAKE_SOURCE_DIR}/kchart ${CMAKE_SOURCE_DIR}/kchart/kdchart ${CMAKE_SOURCE_DIR}/interfaces ${CMAKE_SOURCE_DIR}/filters/libdialogfilter ${CMAKE_SOURCE_DIR}/filters/kchart/libimageexport ${KDE4_INCLUDES}  )


########### next target ###############

set(kchartmngexport_PART_SRCS mngexport.cpp ${libdialogfilter_SRCS} )

kde4_automoc(${kchartmngexport_PART_SRCS})

kde4_add_plugin(kchartmngexport WITH_PREFIX ${kchartmngexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kchartmngexport )

target_link_libraries(kchartmngexport  ${KDE4_KDECORE_LIBS} kchartcommon kchartimageexport  )

install(TARGETS kchartmngexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kchart_mng_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})





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
#kde_module_LTLIBRARIES = libkchartmngexport.la
#
#libkchartmngexport_la_SOURCES = mngexport.cpp
#libkchartmngexport_la_LDFLAGS = -module $(KDE_PLUGIN) -no-undefined
#libkchartmngexport_la_LIBADD = ../../../kchart/libkchartcommon.la ../libimageexport/libkchartimageexport.la ../../../filters/libdialogfilter/libdialogfilter.la $(KOFFICE_LIBS)
#noinst_HEADERS = \
#	mngexport.h
#
#METASOURCES = AUTO
#
#service_DATA = kchart_mng_export.desktop
#servicedir = $(kde_servicesdir)
#
