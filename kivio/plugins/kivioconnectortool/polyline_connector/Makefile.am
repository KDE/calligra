INCLUDES = -I$(top_srcdir)/kivio/kiviopart\
	-I$(top_srcdir)/kivio/kiviopart/kiviosdk\
	-I$(top_srcdir)/kivio/kiviopart/tools\
	$(KOTEXT_INCLUDES)\
	$(KOFFICE_INCLUDES) $(all_includes)

METASOURCES = AUTO

kde_module_LTLIBRARIES =  libpolyline_connector.la
# Hack to make it work (dave m)
libpolyline_connector_la_LIBADD = $(LIB_KOPAINTER) $(top_builddir)/kivio/kiviopart/libkiviocommon.la

libpolyline_connector_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)
libpolyline_connector_la_SOURCES =  kiviopolylineconnectorfactory.cpp kiviopolylineconnector.cpp

noinst_HEADERS =  kiviopolylineconnectorfactory.h kiviopolylineconnector.h

stencildir = $(kde_datadir)/kivio/autoloadStencils

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(stencildir)
	-rm -f $(DESTDIR)$(stencildir)/libpolyline_connector.so
	$(LN_S) $(kde_moduledir)/libpolyline_connector.so $(DESTDIR)$(stencildir)/libpolyline_connector.so

uninstall-local:
	-rm -f $(DESTDIR)$(stencildir)/libpolyline_connector.so
