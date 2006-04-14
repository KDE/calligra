kde4_header()

set(kritapngimport_PART_SRCS
    kis_png_import.cc 
    kis_png_converter.cc
    )

kde4_automoc(${kritapngimport_PART_SRCS})

kde4_add_plugin(kritapngimport WITH_PREFIX ${kritapngimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kritapngimport )

include_directories(${PNG_INCLUDE_DIR})

add_definitions(${PNG_DEFINITIONS} ${KDE4_ENABLE_EXCEPTIONS})

target_link_libraries(kritapngimport  ${KDE4_KDECORE_LIBS} kritacommon ${PNG_LIBRARIES})

install_targets(${LIB_INSTALL_DIR}/kde4 kritapngimport )

set(kritapngexport_PART_SRCS 
    kis_png_export.cc 
    kis_png_converter.cc
    )

kde4_automoc(${kritapngexport_PART_SRCS})

kde4_add_ui3_files(kritapngexport_PART_SRCS kis_wdg_options_png.ui )

kde4_add_plugin(kritapngexport WITH_PREFIX ${kritapngexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kritapngexport )

target_link_libraries(kritapngexport  ${KDE4_KDECORE_LIBS} kritacommon ${PNG_LIBRARIES})

install_targets(${LIB_INSTALL_DIR}/kde4 kritapngexport )

install_files( ${SERVICES_INSTALL_DIR} FILES  krita_png_import.desktop krita_png_export.desktop )
install_files( ${APPLNK_INSTALL_DIR}/Office FILES  krita_png.desktop )

kde4_footer()

#original Makefile.am contents follow:

#kde_module_LTLIBRARIES = libkritapngimport.la libkritapngexport.la
#
#libkritapngexport_la_LDFLAGS = $(KDE_PLUGIN) $(KDE_RPATH) $(all_libraries) -module -avoid-version -no-undefined
#libkritapngexport_la_LIBADD = \
#	$(KOFFICE_LIBS) \
#	$(top_builddir)/krita/libkritacommon.la \
#	-lpng
#
#libkritapngimport_la_LDFLAGS = $(KDE_PLUGIN) $(KDE_RPATH) $(all_libraries) -module -avoid-version -no-undefined
#libkritapngimport_la_LIBADD = \
#	$(KOFFICE_LIBS) \
#	$(top_builddir)/krita/libkritacommon.la \
#	-lpng
#
#INCLUDES= \
#	-I$(srcdir) \
#	 $(KOFFICE_INCLUDES) \
#	-I$(top_srcdir)/krita \
#	-I$(top_srcdir)/krita/core \
#	-I$(top_srcdir)/krita/sdk \
#	-I$(top_srcdir)/krita/core/tiles \
#	-I$(top_srcdir)/krita/kritacolor \
#	-I$(top_srcdir)/krita/ui \
#        $(KOFFICE_INCLUDES) -I$(interfacedir) \
#        $(KOPAINTER_INCLUDES) \
#	$(all_includes) 
#
#service_DATA = krita_png_import.desktop krita_png_export.desktop
#servicedir = $(kde_servicesdir)
#
#kdelnk_DATA = krita_png.desktop
#kdelnkdir = $(kde_appsdir)/Office
#
#libkritapngimport_la_SOURCES = kis_png_import.cc kis_png_converter.cc
#libkritapngexport_la_SOURCES = kis_wdg_options_png.ui kis_png_export.cc kis_png_converter.cc
#
#METASOURCES = AUTO
#
#
#KDE_CXXFLAGS = $(USE_EXCEPTIONS)
