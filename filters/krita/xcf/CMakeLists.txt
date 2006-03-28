kde4_header()

add_subdirectory( xcf )

include_directories( ${CMAKE_SOURCE_DIR}/krita ${CMAKE_SOURCE_DIR}/krita/core ${CMAKE_SOURCE_DIR}/krita/sdk ${CMAKE_SOURCE_DIR}/krita/core/tiles ${CMAKE_SOURCE_DIR}/krita/kritacolor ${CMAKE_SOURCE_DIR}/krita/ui ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kritaxcfimport_PART_SRCS xcfimport.cpp )

kde4_automoc(${kritaxcfimport_PART_SRCS})

kde4_add_plugin(kritaxcfimport WITH_PREFIX ${kritaxcfimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kritaxcfimport )

target_link_libraries(kritaxcfimport  ${KDE4_KDECORE_LIBS} kritacommon )

install_targets(${LIB_INSTALL_DIR}/kde4 kritaxcfimport )


########### next target ###############

set(kritaxcfexport_PART_SRCS xcfexport.cpp )

kde4_automoc(${kritaxcfexport_PART_SRCS})

kde4_add_plugin(kritaxcfexport WITH_PREFIX ${kritaxcfexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kritaxcfexport )

target_link_libraries(kritaxcfexport  ${KDE4_KDECORE_LIBS} kritacommon )

install_targets(${LIB_INSTALL_DIR}/kde4 kritaxcfexport )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  krita_xcf_import.desktop krita_xcf_export.desktop )

kde4_footer()



#original Makefile.am contents follow:

#kde_module_LTLIBRARIES = libkritaxcfimport.la libkritaxcfexport.la
#
#libkritaxcfexport_la_LDFLAGS = $(KDE_PLUGIN) $(LIBMAGICK_LDFLAGS) $(KDE_RPATH) $(LIBMAGICK_RPATH) $(all_libraries) -module -avoid-version -no-undefined
#libkritaxcfexport_la_LIBADD = \
#	$(KOFFICE_LIBS) \
#	$(LIBMAGICK_LIBS) \
#	$(top_builddir)/krita/libkritacommon.la
#
#libkritaxcfimport_la_LDFLAGS = $(KDE_PLUGIN) $(LIBMAGICK_LDFLAGS) $(KDE_RPATH) $(LIBMAGICK_RPATH) $(all_libraries) -module -avoid-version -no-undefined
#libkritaxcfimport_la_LIBADD = \
#	$(KOFFICE_LIBS) \
#	$(LIBMAGICK_LIBS) \
#	$(top_builddir)/krita/libkritacommon.la
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
#        $(KOPAINTER_INCLUDES) $(LIBMAGICK_CPPFLAGS) \
#	$(all_includes) 
#
#service_DATA = krita_xcf_import.desktop krita_xcf_export.desktop
#servicedir = $(kde_servicesdir)
#
#libkritaxcfimport_la_SOURCES = xcfimport.cpp 
#libkritaxcfexport_la_SOURCES = xcfexport.cpp
#
#METASOURCES = AUTO
#
#SUBDIRS=xcf
#
#KDE_CXXFLAGS = $(USE_EXCEPTIONS)
