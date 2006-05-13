

include_directories( ${CMAKE_SOURCE_DIR}/kivio/kiviopart ${CMAKE_SOURCE_DIR}/kivio/kiviopart/kiviosdk ${CMAKE_SOURCE_DIR}/kivio/kiviopart/config ${KDE4_INCLUDES}  )


########### next target ###############

set(kivioimageexport_PART_SRCS kivio_imageexport.cpp kivio_imageexportdialog.cpp )

kde4_automoc(${kivioimageexport_PART_SRCS})

kde4_add_ui3_files(kivioimageexport_PART_SRCS kivio_imageexportwidget.ui )

kde4_add_plugin(kivioimageexport WITH_PREFIX ${kivioimageexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kivioimageexport )

target_link_libraries(kivioimageexport  ${KDE4_KDECORE_LIBS} kiviocommon )

install_targets(${PLUGIN_INSTALL_DIR} kivioimageexport )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  kivio_image_export.desktop )





#original Makefile.am contents follow:

#INCLUDES = -I$(srcdir) $(KOFFICE_INCLUDES) \
#	-I$(top_srcdir)/kivio/kiviopart \
#	-I$(top_srcdir)/kivio/kiviopart/kiviosdk \
#	-I$(top_srcdir)/kivio/kiviopart/config \
#        -I$(top_builddir)/kivio/kiviopart/config \
#	$(all_includes)
#
#METASOURCES = AUTO
#
#kde_module_LTLIBRARIES = libkivioimageexport.la
#
#libkivioimageexport_la_SOURCES = kivio_imageexport.cpp \
#	kivio_imageexportwidget.ui kivio_imageexportdialog.cpp
#libkivioimageexport_la_LDFLAGS = -module $(KDE_PLUGIN) -no-undefined
#libkivioimageexport_la_LIBADD = $(top_builddir)/kivio/kiviopart/libkiviocommon.la $(KOFFICE_LIBS)
#
#service_DATA = kivio_image_export.desktop
#servicedir = $(kde_servicesdir)
