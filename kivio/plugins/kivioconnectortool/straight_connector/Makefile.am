stencildir = $(kde_datadir)/kivio/autoloadStencils
stencil_DATA = straight_connector.xpm

kde_module_LTLIBRARIES =libstraight_connector.la

libstraight_connector_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

# Hack to make it work (dave m)
libstraight_connector_la_LIBADD = $(top_builddir)/kivio/kiviopart/libkiviopart.la

INCLUDES = -I$(top_srcdir)/kivio/kiviopart\
	-I$(top_srcdir)/kivio/kiviopart/kiviosdk\
	-I$(top_srcdir)/kivio/kiviopart/tklib\
	-I$(top_srcdir)/kivio/kiviopart/tools\
	$(KOTEXT_INCLUDES)\
	$(KOFFICE_INCLUDES) $(all_includes)
METASOURCES = AUTO

libstraight_connector_la_SOURCES = \
		straight_connector.cpp
EXTRA_DIST = \
		straight_connector.cpp\
		straight_connector.h\
		straight_connector.xpm

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(stencildir)
	$(RM) $(DESTDIR)$(stencildir)/libstraight_connector.so
	$(LN_S) $(kde_moduledir)/libstraight_connector.so $(DESTDIR)$(stencildir)/libstraight_connector.so

uninstall-local:
	$(RM) $(DESTDIR)$(stencildir)/libstraight_connector.so

