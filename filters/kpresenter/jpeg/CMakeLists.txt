kde4_header()

include_directories( ${CMAKE_SOURCE_DIR}/kpresenter ${CMAKE_SOURCE_DIR}/lib/kotext ${CMAKE_SOURCE_DIR}/filters/libdialogfilter ${CMAKE_SOURCE_DIR}/filters/kpresenter/libimageexport ${KDE4_INCLUDES}  )


########### next target ###############

set(kpresenterjpegexport_PART_SRCS jpegexport.cpp )

kde4_automoc(${kpresenterjpegexport_PART_SRCS})

kde4_add_plugin(kpresenterjpegexport WITH_PREFIX ${kpresenterjpegexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kpresenterjpegexport )

target_link_libraries(kpresenterjpegexport  ${KDE4_KDECORE_LIBS} kpresenterprivate dialogfilter kpresenterimageexport )

install_targets(${PLUGIN_INSTALL_DIR} kpresenterjpegexport )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  kpresenter_jpeg_export.desktop )

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
#kde_module_LTLIBRARIES = libkpresenterjpegexport.la
#
#libkpresenterjpegexport_la_SOURCES = jpegexport.cpp
#libkpresenterjpegexport_la_LDFLAGS = -module $(KDE_PLUGIN) -no-undefined
#libkpresenterjpegexport_la_LIBADD = ../../../kpresenter/libkpresenterprivate.la ../../../filters/libdialogfilter/libdialogfilter.la ../libimageexport/libkpresenterimageexport.la $(KOFFICE_LIBS)
#noinst_HEADERS = \
#	jpegexport.h
#
#METASOURCES = AUTO
#
#service_DATA = kpresenter_jpeg_export.desktop
#servicedir = $(kde_servicesdir)
#
