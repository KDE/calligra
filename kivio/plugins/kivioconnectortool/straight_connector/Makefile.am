#1########################################################!@#$
####   Makefile.am generated automatically by KDEStudio   ####
#### WARNING! All changes made in this part will be lost! ####
##############################################################

libdir = $(kde_datadir)/kivio/autoloadStencils
lib_LTLIBRARIES =libstraight_connector.la


libstraight_connector_la_LDFLAGS = -version-info 1

INCLUDES = $(all_includes)\
		-I$(top_srcdir)/kiviopart\
		-I$(top_srcdir)/kiviopart/kiviosdk\
		-I$(top_srcdir)/kiviopart/tklib\
		-I$(top_srcdir)/kiviopart/tools
CXXFLAGS = -O2 -Wall 
DEFS = -I. -I$(srcdir) -I$(top_srcdir)
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
install-libLTLIBRARIES: $(lib_LTLIBRARIES)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libdir)
	$(LIBTOOL)  --mode=install $(INSTALL) ./.libs/libstraight_connector.so $(DESTDIR)$(libdir)/libstraight_connector.ksp;

uninstall-libLTLIBRARIES:
	@$(NORMAL_UNINSTALL)
	$(LIBTOOL)  --mode=uninstall rm -f $(DESTDIR)$(libdir)/libstraight_connector.ksp;

stencil_DATA = straight_connector.xpm
stencildir = $(kde_datadir)/kivio/autoloadStencils
