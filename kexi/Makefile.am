kde_module_LTLIBRARIES = libkexipart.la
libkexipart_la_SOURCES = kexipart.cpp

libkexipart_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN)
libkexipart_la_LIBADD = $(LIB_KOFFICEUI) core/libkexicore.la

## The kdeinit loadable module
lib_LTLIBRARIES = kexi.la
kexi_la_SOURCES = main.cpp
kexi_la_LDFLAGS = $(all_libraries) -module -avoid-version
kexi_la_LIBADD = $(LIB_KOFFICECORE)

## The executable
bin_PROGRAMS = kexi
kexi_SOURCES = dummy.cc
kexi_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kexi_LDADD = kexi.la

dummy.cc:
	echo > dummy.cc


SUBDIRS = 3rdparty kexidb formeditor core tableview widget . plugins pics data interfaces filters

kdelnkdir = $(kde_appsdir)/Office
kdelnk_DATA = kexi.desktop

install-data-local:
#	$(mkinstalldirs) $(kde_servicetypesdir)/
#	$(INSTALL_DATA) $(srcdir)/kexiinterface.desktop $(kde_servicetypesdir)/kexiinterface.desktop
#	$(mkinstalldirs) $(kde_servicedir)
#	$(INSTALL_DATA) $(srcdir)/interfaces/mysqlinterface.desktop $(kde_servicesdir)/mysqlinterface.desktop

# kde_appsdir         Where your application's menu entry (.desktop) should go to.
# kde_icondir         Where your icon should go to - better use KDE_ICON.
# kde_sounddir        Where your sounds should go to.
# kde_htmldir         Where your docs should go to. (contains lang subdirs)
# kde_datadir         Where you install application data. (Use a subdir)
# kde_locale          Where translation files should go to. (contains lang subdirs)
# kde_cgidir          Where cgi-bin executables should go to.
# kde_confdir         Where config files should go to (system-wide ones with default values).
# kde_mimedir         Where mimetypes .desktop files should go to.
# kde_servicesdir     Where services .desktop files should go to.
# kde_servicetypesdir Where servicetypes .desktop files should go to.
# kde_toolbardir      Where general toolbar icons should go to (deprecated, use KDE_ICON).
# kde_wallpaperdir    Where general wallpapers should go to.
# kde_templatesdir    Where templates for the "New" menu (Konqueror/KDesktop) should go to.
# kde_bindir          Where executables should go to. Use bin_PROGRAMS or bin_SCRIPTS.
# kde_libdir          Where shared libraries should go to. Use lib_LTLIBRARIES.
# kde_moduledir       Where modules (e.g. parts) should go to. Use kde_module_LTLIBRARIES.
# kde_styledir        Where Qt/KDE widget styles should go to (new in KDE 3).
# kde_designerdir     Where Qt Designer plugins should go to (new in KDE 3).

# set the include path for X, qt and KDE
INCLUDES= $(KOFFICE_INCLUDES) $(all_includes)

METASOURCES = AUTO

messages: rc.cpp
	$(EXTRACTRC) data/*.rc > rc.cpp
	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
	if test -n "$$LIST"; then \
	 $(XGETTEXT) $$LIST -o $(podir)/kexi.pot; \
	fi

#uncomment this if you like doxygen docs for Kexi
#include $(top_srcdir)/../admin/Doxyfile.am

include $(top_srcdir)/kexi/Makefile.global
