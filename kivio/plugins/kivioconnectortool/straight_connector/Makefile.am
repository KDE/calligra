#1########################################################!@#$
####   Makefile.am generated automatically by KDEStudio   ####
#### WARNING! All changes made in this part will be lost! ####
##############################################################

libdir = $(kde_datadir)/kivio/autoloadStencils
lib_LTLIBRARIES =libstraight_connector.la


libstraight_connector_la_LDFLAGS = -version-info 1

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
##############################################################
####                  END KDEStudio part                  ####
#2########################################################!@#$

## This is a very bad hack..... why is this needed ? (David)

install-libLTLIBRARIES: $(lib_LTLIBRARIES)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libdir)
	$(LIBTOOL)  --mode=install $(INSTALL) ./.libs/libstraight_connector.so $(DESTDIR)$(libdir)/libstraight_connector.ksp;

uninstall-libLTLIBRARIES:
	@$(NORMAL_UNINSTALL)
	$(LIBTOOL)  --mode=uninstall rm -f $(DESTDIR)$(libdir)/libstraight_connector.ksp;

stencil_DATA = straight_connector.xpm
stencildir = $(kde_datadir)/kivio/autoloadStencils

