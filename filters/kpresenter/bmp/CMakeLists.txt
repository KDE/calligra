

include_directories( ${CMAKE_SOURCE_DIR}/kpresenter ${CMAKE_SOURCE_DIR}/lib/kotext ${CMAKE_SOURCE_DIR}/filters/libdialogfilter ${CMAKE_SOURCE_DIR}/filters/kpresenter/libimageexport ${KDE4_INCLUDES}  )


########### next target ###############

set(kpresenterbmpexport_PART_SRCS bmpexport.cpp ${libdialogfilter_SRCS})

kde4_automoc(${kpresenterbmpexport_PART_SRCS})

kde4_add_plugin(kpresenterbmpexport WITH_PREFIX ${kpresenterbmpexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kpresenterbmpexport )

target_link_libraries(kpresenterbmpexport  ${KDE4_KDECORE_LIBS} kpresenterprivate kpresenterimageexport )

install(TARGETS kpresenterbmpexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kpresenter_bmp_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})





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
#kde_module_LTLIBRARIES = libkpresenterbmpexport.la
#
#libkpresenterbmpexport_la_SOURCES = bmpexport.cpp
#libkpresenterbmpexport_la_LDFLAGS = -module $(KDE_PLUGIN) -no-undefined
#libkpresenterbmpexport_la_LIBADD = ../../../kpresenter/libkpresenterprivate.la ../../../filters/libdialogfilter/libdialogfilter.la ../libimageexport/libkpresenterimageexport.la $(KOFFICE_LIBS)
#noinst_HEADERS = \
#	bmpexport.h
#
#METASOURCES = AUTO
#
#service_DATA = kpresenter_bmp_export.desktop
#servicedir = $(kde_servicesdir)
#
