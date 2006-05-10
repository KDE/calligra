kde4_header()

include_directories( ${CMAKE_SOURCE_DIR}/kchart ${CMAKE_SOURCE_DIR}/kchart/kdchart ${CMAKE_SOURCE_DIR}/interfaces ${CMAKE_SOURCE_DIR}/filters/libdialogfilter ${CMAKE_SOURCE_DIR}/filters/kchart/libimageexport ${KDE4_INCLUDES}  )


########### next target ###############

set(kchartpngexport_PART_SRCS pngexport.cpp ${libdialogfilter_SRCS})

kde4_automoc(${kchartpngexport_PART_SRCS})

kde4_add_plugin(kchartpngexport WITH_PREFIX ${kchartpngexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kchartpngexport )

target_link_libraries(kchartpngexport  ${KDE4_KDECORE_LIBS} kchartcommon kchartimageexport  )

install_targets(${PLUGIN_INSTALL_DIR} kchartpngexport )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  kchart_png_export.desktop )

kde4_footer()



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
#kde_module_LTLIBRARIES = libkchartpngexport.la
#
#libkchartpngexport_la_SOURCES = pngexport.cpp
#libkchartpngexport_la_LDFLAGS = -module $(KDE_PLUGIN) -no-undefined
#libkchartpngexport_la_LIBADD = ../../../kchart/libkchartcommon.la ../libimageexport/libkchartimageexport.la ../../../filters/libdialogfilter/libdialogfilter.la $(KOFFICE_LIBS)
#noinst_HEADERS = \
#	pngexport.h 
#
#METASOURCES = AUTO
#
#service_DATA = kchart_png_export.desktop
#servicedir = $(kde_servicesdir)
#
## Note: If your filter exports or exports some special file
## which KDE doesn't have a mimetype for, yet, you'll have to
## create a mimetype and install it using those two lines.
## In case of doubt please ask koffice@kde.org or
## koffice-devel@kde.org. Thanks.
## data_DATA = x-png.desktop
## datadir = $(kde_mimedir)/text
