kde4_header()
project(karbon)

add_subdirectory( commands )
add_subdirectory( visitors )
add_subdirectory( core )
add_subdirectory( shapes )
add_subdirectory( widgets )
add_subdirectory( dialogs )
add_subdirectory( dockers )
add_subdirectory( render )
add_subdirectory( tools )
add_subdirectory( plugins )
add_subdirectory( data )
add_subdirectory( pics )
add_subdirectory( templates )




########### next target ###############

set(karboncommon_LIB_SRCS 
   karbon_factory.cc 
   karbon_resourceserver.cc 
   karbon_part.cc 
   karbon_part_iface.cc 
   karbon_view.cc 
   karbon_view_iface.cc 
   karbon_drag.cpp 
   karbon_grid_data.cpp 
   karbon_tool_registry.cc 
   karbon_tool_factory.cc 
   vtoolcontroller.cc 
   vtool.cc )

kde4_automoc(${karboncommon_LIB_SRCS})

kde4_add_dcop_skels(karboncommon_LIB_SRCS karbon_part_iface.h karbon_view_iface.h )

kde4_add_library(karboncommon SHARED ${karboncommon_LIB_SRCS})

target_link_libraries(karboncommon  ${KDE4_KDECORE_LIBS} kofficecore kofficeui kopalette kopainter kotext karbondockers karbonwidgets vpainter karbonxrgbrender karboncore karbonshapes karboncommands karbonvisitors karbondialogs )

set_target_properties(karboncommon PROPERTIES VERSION 1.0.0 SOVERSION 1 )
install_targets(${LIB_INSTALL_DIR} karboncommon )


########### next target ###############

set(karbonpart_PART_SRCS karbon_factory_init.cc )

kde4_automoc(${karbonpart_PART_SRCS})

kde4_add_plugin(karbonpart WITH_PREFIX ${karbonpart_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} karbonpart )

target_link_libraries(karbonpart  ${KDE4_KDECORE_LIBS} karboncommon )

install_targets(${PLUGIN_INSTALL_DIR} karbonpart )


########### next target ###############

set(karbon_KDEINIT_SRCS main.cc )

kde4_automoc(${karbon_KDEINIT_SRCS})

kde4_add_kdeinit_executable( karbon ${karbon_KDEINIT_SRCS})

target_link_libraries(kdeinit_karbon  ${KDE4_KDECORE_LIBS} karboncommon )

install_targets(${LIB_INSTALL_DIR} kdeinit_karbon )

target_link_libraries( karbon kdeinit_karbon )
install_targets(/bin karbon )

########### install files ###############




kde4_footer()



#original Makefile.am contents follow:

#INCLUDES = $(KOFFICE_INCLUDES) $(KOTEXT_INCLUDES) $(KOPALETTE_INCLUDES) $(KOPAINTER_INCLUDES) \
#	-I$(srcdir) \
#	-I$(srcdir)/commands \
#	-I$(srcdir)/core \
#	-I$(srcdir)/dialogs \
#	-I$(srcdir)/dockers \
#	-I$(srcdir)/render \
#	-I$(srcdir)/visitors \
#	-I$(srcdir)/widgets \
#	$(all_includes)
#
#SUBDIRS = commands visitors core shapes widgets dialogs dockers render . tools plugins data pics templates
#
#lib_LTLIBRARIES = libkarboncommon.la
#kde_module_LTLIBRARIES = libkarbonpart.la
#
#libkarboncommon_la_SOURCES = karbon_factory.cc 	karbon_resourceserver.cc 	karbon_part.cc 	karbon_part_iface.cc 	karbon_part_iface.skel 	karbon_view.cc 	karbon_view_iface.cc 	karbon_view_iface.skel karbon_drag.cpp karbon_grid_data.cpp karbon_tool_registry.cc karbon_tool_factory.cc	vtoolcontroller.cc vtool.cc
#
#libkarboncommon_la_LDFLAGS = $(KDE_RPATH) $(LIBFREETYPE_RPATH) -no-undefined
#libkarboncommon_la_LIBADD = $(LIB_KOFFICECORE) $(LIB_KOFFICEUI) $(LIB_KOPALETTE) $(LIB_KOPAINTER) $(LIB_KOTEXT) \
#	dockers/libkarbondockers.la \
#	widgets/libkarbonwidgets.la \
#	render/libvpainter.la \
#	render/xrgbrender/libkarbonxrgbrender.la \
#	core/libkarboncore.la \
#	shapes/libkarbonshapes.la \
#	commands/libkarboncommands.la \
#	visitors/libkarbonvisitors.la \
#	dialogs/libkarbondialogs.la \
#	$(LIBART_LIBS) \
#	$(LIBFONTCONFIG_LIBS) \
#	$(LIBMAGICK_LIBS) \
#	$(LIBFREETYPE_LIBS)
#
#libkarbonpart_la_SOURCES = karbon_factory_init.cc
#libkarbonpart_la_LDFLAGS = $(KDE_PLUGIN) $(LIBFREETYPE_RPATH)
#libkarbonpart_la_LIBADD = libkarboncommon.la
#
#bin_PROGRAMS =
#kdeinit_LTLIBRARIES = karbon.la
#karbon_la_SOURCES = main.cc
#karbon_la_LDFLAGS = $(all_libraries) -module -avoid-version
#karbon_la_LIBADD = libkarboncommon.la
#
#noinst_HEADERS = \
#	karbon_factory.h \
#	karbon_part.h \
#	karbon_part_iface.h \
#	karbon_view.h \
#	karbon_drag.h \
#	vtoolcontroller.h \
#	vtool.h
#
#METASOURCES = AUTO
#
#messages: rc.cpp
#	$(EXTRACTRC) data/*.rc >> rc.cpp
#	$(XGETTEXT) rc.cpp *.cpp *.cc */*.cc tools/*.h plugins/*/*.cc -o $(podir)/karbon.pot
#
#include $(top_srcdir)/admin/Doxyfile.am
#
