kde_module_LTLIBRARIES =sml_connector.la

INCLUDES = -I$(top_srcdir)/kivio/kiviopart\
	-I$(top_srcdir)/kivio/kiviopart/kiviosdk\
	-I$(top_srcdir)/kivio/kiviopart/tools\
	$(KOFFICE_INCLUDES) $(KOTEXT_INCLUDES) $(all_includes)

METASOURCES = AUTO

sml_connector_la_SOURCES = sml_connector.cpp
sml_connector_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)
sml_connector_la_LIBADD = $(LIB_KOPAINTER) $(top_builddir)/kivio/kiviopart/libkiviocommon.la

stencildir = $(kde_datadir)/kivio/autoloadStencils
stencil_DATA = sml_connector.xpm

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(stencildir)
	rm -f $(DESTDIR)$(stencildir)/sml_connector.so
	$(LN_S) $(kde_moduledir)/sml_connector.so $(DESTDIR)$(stencildir)/sml_connector.so

uninstall-local:
	rm -f $(DESTDIR)$(stencildir)/sml_connector.so
