include $(top_srcdir)/kexi/Makefile.global

kdeinit_LTLIBRARIES = kexi.la
bin_PROGRAMS =
kexi_la_SOURCES = main.cpp
kexi_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kexi_la_LIBADD = core/libkexicore.la

#SUBDIRS= 3rdparty kexidb core tableview . data widget plugins tests
SUBDIRS= 3rdparty kexidb core tableview widget plugins data . 

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
INCLUDES = $(LIB_KEXI_KMDI_INCLUDES) $(KOFFICE_INCLUDES) $(all_includes)

METASOURCES = AUTO

messages: rc.cpp
	$(EXTRACTRC) data/*.rc > rc.cpp
	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
	if test -n "$$LIST"; then \
	 $(XGETTEXT) $$LIST -o $(podir)/kexi.pot; \
	fi

#uncomment this if you like doxygen docs for Kexi
#include $(top_srcdir)/../admin/Doxyfile.am

