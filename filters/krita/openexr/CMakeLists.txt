kde4_header()

include_directories(
	${OPENEXR_INCLUDE_DIR}
	${CMAKE_SOURCE_DIR}/krita
	${CMAKE_SOURCE_DIR}/krita/core/
	${CMAKE_SOURCE_DIR}/krita/sdk/
	${CMAKE_SOURCE_DIR}/krita/core/tiles/
	${CMAKE_SOURCE_DIR}/krita/kritacolor/
	${CMAKE_SOURCE_DIR}/krita/ui/
	${CMAKE_SOURCE_DIR}/krita/colorspaces/rgb_f32/
	${CMAKE_SOURCE_DIR}/krita/colorspaces/rgb_f16half/
	)


########### next target ###############

set(krita_openexr_import_PART_SRCS kis_openexr_import.cpp )

kde4_automoc(${krita_openexr_import_PART_SRCS})

kde4_add_plugin(krita_openexr_import WITH_PREFIX ${krita_openexr_import_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} krita_openexr_import )

target_link_libraries(krita_openexr_import  ${KDE4_KDECORE_LIBS} kritacommon krita_rgb_f32 krita_rgb_f16half ${OPENEXR_LIBRARIES} )

install_targets(${LIB_INSTALL_DIR}/kde4 krita_openexr_import )


########### next target ###############

set(krita_openexr_export_PART_SRCS kis_openexr_export.cpp )

kde4_automoc(${krita_openexr_export_PART_SRCS})

kde4_add_plugin(krita_openexr_export WITH_PREFIX ${krita_openexr_export_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} krita_openexr_export )

target_link_libraries(krita_openexr_export  ${KDE4_KDECORE_LIBS} kritacommon krita_rgb_f32 krita_rgb_f16half ${OPENEXR_LIBRARIES})

install_targets(${LIB_INSTALL_DIR}/kde4 krita_openexr_export )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  krita_openexr_import.desktop krita_openexr_export.desktop )
install_files( ${APPLNK_INSTALL_DIR}/Office FILES  krita_openexr.desktop )

kde4_footer()



#original Makefile.am contents follow:

#AM_CPPFLAGS= \
#	-I$(srcdir) \
#	-I$(top_srcdir)/krita \
#	-I$(top_srcdir)/krita/core \
#	-I$(top_srcdir)/krita/sdk \
#	-I$(top_srcdir)/krita/core/tiles \
#	-I$(top_srcdir)/krita/kritacolor \
#	-I$(top_srcdir)/krita/ui \
#	-I$(top_srcdir)/krita/colorspaces/rgb_f32 \
#	-I$(top_srcdir)/krita/colorspaces/rgb_f16half \
#        $(KOFFICE_INCLUDES) \
#	-I$(interfacedir) \
#        $(OPENEXR_CFLAGS) \
#	$(all_includes) 
#
#kde_module_LTLIBRARIES = libkrita_openexr_import.la libkrita_openexr_export.la
#
#libkrita_openexr_export_la_LDFLAGS = $(KDE_PLUGIN) $(KDE_RPATH) $(all_libraries) -module -avoid-version -no-undefined
#libkrita_openexr_export_la_LIBADD = \
#	$(KOFFICE_LIBS) \
#	$(OPENEXR_LIBS) \
#	$(top_builddir)/krita/libkritacommon.la \
#	$(top_builddir)/krita/colorspaces/rgb_f32/libkrita_rgb_f32.la \
#	$(top_builddir)/krita/colorspaces/rgb_f16half/libkrita_rgb_f16half.la
#
#
#libkrita_openexr_import_la_LDFLAGS = $(KDE_PLUGIN) $(KDE_RPATH) $(all_libraries) -module -avoid-version -no-undefined
#libkrita_openexr_import_la_LIBADD = \
#	$(KOFFICE_LIBS) \
#	$(OPENEXR_LIBS) \
#	$(top_builddir)/krita/libkritacommon.la \
#	$(top_builddir)/krita/colorspaces/rgb_f32/libkrita_rgb_f32.la \
#	$(top_builddir)/krita/colorspaces/rgb_f16half/libkrita_rgb_f16half.la
#
#
#service_DATA = krita_openexr_import.desktop krita_openexr_export.desktop
#servicedir = $(kde_servicesdir)
#
#kdelnk_DATA = krita_openexr.desktop
#kdelnkdir = $(kde_appsdir)/Office
#
#libkrita_openexr_import_la_SOURCES = kis_openexr_import.cpp
#libkrita_openexr_export_la_SOURCES = kis_openexr_export.cpp
#
#METASOURCES = AUTO
#
#KDE_CXXFLAGS = $(USE_EXCEPTIONS)
#
