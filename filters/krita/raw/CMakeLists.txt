




########### next target ###############

set(krita_raw_import_PART_SRCS kis_raw_import.cpp )

kde4_automoc(${krita_raw_import_PART_SRCS})

kde4_add_ui_files(krita_raw_import_PART_SRCS wdgrawimport.ui )

kde4_add_plugin(krita_raw_import WITH_PREFIX ${krita_raw_import_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} krita_raw_import )

target_link_libraries(krita_raw_import kritaui)

install_targets(${PLUGIN_INSTALL_DIR} krita_raw_import )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  krita_raw_import.desktop )
install_files( ${XDG_APPS_DIR} FILES  krita_raw.desktop )





#original Makefile.am contents follow:

#AM_CPPFLAGS= \
#	-I$(srcdir) \
#	-I$(top_srcdir)/krita \
#	-I$(top_srcdir)/krita/core \
#	-I$(top_srcdir)/krita/sdk \
#	-I$(top_srcdir)/krita/kritacolor \
#	-I$(top_srcdir)/krita/ui \
#	-I$(top_srcdir)/filters/krita/magick \
#	$(KOFFICE_INCLUDES) \
#	$(all_includes)
#
#kde_module_LTLIBRARIES = libkrita_raw_import.la
#
#libkrita_raw_import_la_LDFLAGS = $(KDE_PLUGIN) $(KDE_RPATH) $(all_libraries) -module -avoid-version -no-undefined
#libkrita_raw_import_la_LIBADD = \
#	$(KOFFICE_LIBS) \
#	$(raw_LIBS) \
#	$(top_builddir)/krita/libkritacommon.la
#
#
#service_DATA = krita_raw_import.desktop
#servicedir = $(kde_servicesdir)
#
#kdelnk_DATA = krita_raw.desktop
#kdelnkdir = $(kde_appsdir)/Office
#
#libkrita_raw_import_la_SOURCES = kis_raw_import.cpp wdgrawimport.ui
#
#METASOURCES = AUTO
#
#KDE_CXXFLAGS = $(USE_EXCEPTIONS)
#
#
