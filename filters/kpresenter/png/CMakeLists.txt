kde4_header()

include_directories( ${CMAKE_SOURCE_DIR}/kpresenter ${CMAKE_SOURCE_DIR}/lib/kotext ${CMAKE_SOURCE_DIR}/filters/libdialogfilter ${CMAKE_SOURCE_DIR}/filters/kpresenter/libimageexport ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kpresenterpngexport_PART_SRCS pngexport.cpp )

kde4_automoc(${kpresenterpngexport_PART_SRCS})

kde4_add_plugin(kpresenterpngexport WITH_PREFIX ${kpresenterpngexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kpresenterpngexport )

target_link_libraries(kpresenterpngexport  ${KDE4_KDECORE_LIBS} kpresenterprivate dialogfilter kpresenterimageexport )

install_targets(${PLUGIN_INSTALL_DIR} kpresenterpngexport )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  kpresenter_png_export.desktop )

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
#kde_module_LTLIBRARIES = libkpresenterpngexport.la
#
#libkpresenterpngexport_la_SOURCES = pngexport.cpp
#libkpresenterpngexport_la_LDFLAGS = -module $(KDE_PLUGIN) -no-undefined
#libkpresenterpngexport_la_LIBADD = ../../../kpresenter/libkpresenterprivate.la ../../../filters/libdialogfilter/libdialogfilter.la ../libimageexport/libkpresenterimageexport.la $(KOFFICE_LIBS)
#noinst_HEADERS = \
#	pngexport.h
#
#METASOURCES = AUTO
#
#service_DATA = kpresenter_png_export.desktop
#servicedir = $(kde_servicesdir)
#
