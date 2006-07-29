stencildir = $(kde_datadir)/kivio/autoloadStencils
stencil_DATA = straight_connector.xpm

kde_module_LTLIBRARIES =straight_connector.la

straight_connector_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

# Hack to make it work (dave m)
straight_connector_la_LIBADD = $(LIB_KOPAINTER) $(top_builddir)/kivio/kiviopart/libkiviocommon.la

INCLUDES = -I$(top_srcdir)/kivio/kiviopart\
	-I$(top_srcdir)/kivio/kiviopart/kiviosdk\
	-I$(top_srcdir)/kivio/kiviopart/tools\
	$(KOTEXT_INCLUDES)\
	$(KOFFICE_INCLUDES) $(all_includes)
METASOURCES = AUTO

straight_connector_la_SOURCES = \
		straight_connector.cpp
EXTRA_DIST = \
		straight_connector.cpp\
		straight_connector.h\
		straight_connector.xpm

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(stencildir)
	-rm -f $(DESTDIR)$(stencildir)/straight_connector.so
	$(LN_S) $(kde_moduledir)/straight_connector.so $(DESTDIR)$(stencildir)/straight_connector.so

uninstall-local:
	-rm -f $(DESTDIR)$(stencildir)/straight_connector.so

