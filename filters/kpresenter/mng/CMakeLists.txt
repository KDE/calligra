

include_directories( ${CMAKE_SOURCE_DIR}/kpresenter ${CMAKE_SOURCE_DIR}/lib/kotext ${CMAKE_SOURCE_DIR}/filters/libdialogfilter ${CMAKE_SOURCE_DIR}/filters/kpresenter/libimageexport ${KDE4_INCLUDES}  )


########### next target ###############

set(kpresentermngexport_PART_SRCS mngexport.cpp )

kde4_automoc(${kpresentermngexport_PART_SRCS})

kde4_add_plugin(kpresentermngexport WITH_PREFIX ${kpresentermngexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kpresentermngexport )

target_link_libraries(kpresentermngexport  ${KDE4_KDECORE_LIBS} kpresenterprivate dialogfilter kpresenterimageexport )

install_targets(${PLUGIN_INSTALL_DIR} kpresentermngexport )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  kpresenter_mng_export.desktop )





#original Makefile.am contents follow:

######## General stuff
#
#INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) \
#	-I$(top_srcdir)/kpresenter \
#        -I$(top_srcdir)/lib/kotext \
#	-I$(top_srcdir)/filters/libdialogfilter \
#	-I$(top_srcdir)/filters/kpresenter/libimageexport \
#	$(all_includes)
#
######## Files
#
#kde_module_LTLIBRARIES = libkpresentermngexport.la
#
#libkpresentermngexport_la_SOURCES = mngexport.cpp
#libkpresentermngexport_la_LDFLAGS = -module $(KDE_PLUGIN) -no-undefined
#libkpresentermngexport_la_LIBADD = ../../../kpresenter/libkpresenterprivate.la ../../../filters/libdialogfilter/libdialogfilter.la ../libimageexport/libkpresenterimageexport.la $(KOFFICE_LIBS)
#noinst_HEADERS = \
#	mngexport.h
#
#METASOURCES = AUTO
#
#service_DATA = kpresenter_mng_export.desktop
#servicedir = $(kde_servicesdir)
#
