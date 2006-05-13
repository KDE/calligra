

include_directories( ${CMAKE_SOURCE_DIR}/kivio/kiviopart ${CMAKE_SOURCE_DIR}/kivio/kiviopart/kiviosdk ${CMAKE_SOURCE_DIR}/kivio/kiviopart/tools ${KDE4_INCLUDES}  )


########### next target ###############

set(sml_connector_PART_SRCS sml_connector.cpp )

kde4_automoc(${sml_connector_PART_SRCS})

kde4_add_plugin(sml_connector ${sml_connector_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} sml_connector )

target_link_libraries(sml_connector  ${KDE4_KDECORE_LIBS} kopainter kiviocommon )

install_targets(${PLUGIN_INSTALL_DIR} sml_connector )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kivio/autoloadStencils FILES  sml_connector.xpm )





#original Makefile.am contents follow:

#kde_module_LTLIBRARIES =sml_connector.la
#
#INCLUDES = -I$(top_srcdir)/kivio/kiviopart\
#	-I$(top_srcdir)/kivio/kiviopart/kiviosdk\
#	-I$(top_srcdir)/kivio/kiviopart/tools\
#	$(KOFFICE_INCLUDES) $(KOTEXT_INCLUDES) $(all_includes)
#
#METASOURCES = AUTO
#
#sml_connector_la_SOURCES = sml_connector.cpp
#sml_connector_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)
#sml_connector_la_LIBADD = $(LIB_KOPAINTER) $(top_builddir)/kivio/kiviopart/libkiviocommon.la
#
#stencildir = $(kde_datadir)/kivio/autoloadStencils
#stencil_DATA = sml_connector.xpm
#
#install-data-local:
#	$(mkinstalldirs) $(DESTDIR)$(stencildir)
#	rm -f $(DESTDIR)$(stencildir)/sml_connector.so
#	$(LN_S) $(kde_moduledir)/sml_connector.so $(DESTDIR)$(stencildir)/sml_connector.so
#
#uninstall-local:
#	rm -f $(DESTDIR)$(stencildir)/sml_connector.so
