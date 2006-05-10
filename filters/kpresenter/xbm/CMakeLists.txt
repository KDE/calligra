kde4_header()

include_directories( ${CMAKE_SOURCE_DIR}/kpresenter ${CMAKE_SOURCE_DIR}/lib/kotext ${CMAKE_SOURCE_DIR}/filters/libdialogfilter ${CMAKE_SOURCE_DIR}/filters/kpresenter/libimageexport ${KDE4_INCLUDES}  )


########### next target ###############

set(kpresenterxbmexport_PART_SRCS xbmexport.cpp )

kde4_automoc(${kpresenterxbmexport_PART_SRCS})

kde4_add_plugin(kpresenterxbmexport WITH_PREFIX ${kpresenterxbmexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kpresenterxbmexport )

target_link_libraries(kpresenterxbmexport  ${KDE4_KDECORE_LIBS} kpresenterprivate dialogfilter kpresenterimageexport )

install_targets(${PLUGIN_INSTALL_DIR} kpresenterxbmexport )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  kpresenter_xbm_export.desktop )

kde4_footer()



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
#kde_module_LTLIBRARIES = libkpresenterxbmexport.la
#
#libkpresenterxbmexport_la_SOURCES = xbmexport.cpp
#libkpresenterxbmexport_la_LDFLAGS = -module $(KDE_PLUGIN) -no-undefined
#libkpresenterxbmexport_la_LIBADD = ../../../kpresenter/libkpresenterprivate.la ../../../filters/libdialogfilter/libdialogfilter.la ../libimageexport/libkpresenterimageexport.la $(KOFFICE_LIBS)
#noinst_HEADERS = \
#	xbmexport.h
#
#METASOURCES = AUTO
#
#service_DATA = kpresenter_xbm_export.desktop
#servicedir = $(kde_servicesdir)
#
