stencildir = $(kde_datadir)/kivio/autoloadStencils

stencil_LTLIBRARIES =libstraight_connector.la
stencil_DATA = straight_connector.xpm

libstraight_connector_la_LDFLAGS = -version-info 1

# Hack to make it work (dave m)
libstraight_connector_la_LIBADD = \
    $(top_builddir)/kivio/kiviopart/libkiviopart.la

INCLUDES = -I$(top_srcdir)/kivio/kiviopart\
	-I$(top_srcdir)/kivio/kiviopart/kiviosdk\
	-I$(top_srcdir)/kivio/kiviopart/tklib\
	-I$(top_srcdir)/kivio/kiviopart/tools\
	$(KOFFICE_INCLUDES) $(all_includes)
libstraight_connector_la_METASOURCES = AUTO
libstraight_connector_la_SOURCES = \
		straight_connector.cpp
EXTRA_DIST = \
		straight_connector.cpp\
		straight_connector.h\
		straight_connector.xpm

## This is a very bad hack..... why is this needed ? (David)

#install-libLTLIBRARIES: $(lib_LTLIBRARIES)
	#@$(NORMAL_INSTALL)
	#$(mkinstalldirs) $(DESTDIR)$(libdir)
	#$(LIBTOOL)  --mode=install $(INSTALL) ./.libs/libstraight_connector.so $(DESTDIR)$(libdir)/straight_connector.ksp;
#
#uninstall-libLTLIBRARIES:
	#@$(NORMAL_UNINSTALL)
	#$(LIBTOOL)  --mode=uninstall rm -f $(DESTDIR)$(libdir)/straight_connector.ksp;

