lib_LTLIBRARIES =libsml_connector.la

INCLUDES = -I$(top_srcdir)/kivio/kiviopart\
	-I$(top_srcdir)/kivio/kiviopart/kiviosdk\
	-I$(top_srcdir)/kivio/kiviopart/tklib\
	-I$(top_srcdir)/kivio/kiviopart/tools\
	$(KOFFICE_INCLUDES) $(KOTEXT_INCLUDES) $(all_includes)

METASOURCES = AUTO

libsml_connector_la_SOURCES = sml_connector.cpp
libsml_connector_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)
libsml_connector_la_LIBADD = $(top_builddir)/kivio/kiviopart/libkiviopart.la

stencildir = $(kde_datadir)/kivio/autoloadStencils
stencil_DATA = sml_connector.xpm

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(stencildir)
	$(RM) $(DESTDIR)$(stencildir)/libsml_connector.so
	$(LN_S) $(kde_moduledir)/libsml_connector.so $(DESTDIR)$(stencildir)/libsml_connector.so

uninstall-local:
	$(RM) $(DESTDIR)$(stencildir)/libsml_connector.so