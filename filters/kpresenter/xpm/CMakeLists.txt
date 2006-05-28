

include_directories( ${CMAKE_SOURCE_DIR}/kpresenter ${CMAKE_SOURCE_DIR}/lib/kotext ${CMAKE_SOURCE_DIR}/filters/libdialogfilter ${CMAKE_SOURCE_DIR}/filters/kpresenter/libimageexport ${KDE4_INCLUDES}  )


########### next target ###############

set(kpresenterxpmexport_PART_SRCS xpmexport.cpp  ${libdialogfilter_SRCS})

kde4_automoc(${kpresenterxpmexport_PART_SRCS})

kde4_add_plugin(kpresenterxpmexport WITH_PREFIX ${kpresenterxpmexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kpresenterxpmexport )

target_link_libraries(kpresenterxpmexport  ${KDE4_KDECORE_LIBS} kpresenterprivate kpresenterimageexport )

install_targets(${PLUGIN_INSTALL_DIR} kpresenterxpmexport )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  kpresenter_xpm_export.desktop )





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
#kde_module_LTLIBRARIES = libkpresenterxpmexport.la
#
#libkpresenterxpmexport_la_SOURCES = xpmexport.cpp
#libkpresenterxpmexport_la_LDFLAGS = -module $(KDE_PLUGIN) -no-undefined
#libkpresenterxpmexport_la_LIBADD = ../../../kpresenter/libkpresenterprivate.la ../../../filters/libdialogfilter/libdialogfilter.la ../libimageexport/libkpresenterimageexport.la $(KOFFICE_LIBS)
#noinst_HEADERS = \
#	xpmexport.h
#
#METASOURCES = AUTO
#
#service_DATA = kpresenter_xpm_export.desktop
#servicedir = $(kde_servicesdir)
#
