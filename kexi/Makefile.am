kde_module_LTLIBRARIES = libkexipart.la
libkexipart_la_SOURCES = kexi_factory.cpp kexiproject.cpp\
	kexiinputtableedit.cpp kexidatetableedit.cpp kmultitabbar.cpp kexitabbrowser.cpp kexiwidget.cpp \
	kexitableheader.cpp kexitablerm.cpp  kexitablelist.cpp kexitableitem.cpp kexitableview.cpp kexidatatable.cpp \
	kexidialogbase.cpp kexibrowseritem.cpp kexiworkspaceMDI.cpp kexicreateprojectpage.cpp \
	kexicreateproject.cpp kexibrowser.cpp kexiview.cpp kexiformmanager.cpp \
	kexicreateprojectpageengine.cpp kexicreateprojectpagelocation.cpp kexicreateprojectpagedb.cpp \
	kexicreateprojectpagefile.cpp kexicreateprojectpageauth.cpp kexicreateprojectpageauth.h \
	kexiprojectproperties.cpp kexiprojectpropertiesdb.cpp kexialtertable.cpp kexiquerydesigner.cpp \
	kexiquerydesignerguieditor.cpp kexirelation.cpp kexirelationdialog.cpp kexirelationview.cpp kexirelationviewtable.cpp kexiformbase.cpp \
	kexicomboboxtableedit.cpp kexitableedit.cpp KexiViewIface.skel KexiViewIface.cpp KexiProjectIface.skel \
	KexiProjectIface.cpp kexipartpopupmenu.cpp kexiprojectpart.cpp kexitablepart.cpp kexiquerypart.cpp \
	kexiprojectpartitem.cpp

libkexipart_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN)
libkexipart_la_LIBADD = $(LIB_KOFFICEUI) -lktexteditor  kexiDB/libkexidb.la formeditor/libkformeditor.la

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


SUBDIRS = kexiDB formeditor . interfaces pics

KDE_CXXFLAGS = $(USE_EXCEPTIONS)

install-data-local:
	$(mkinstalldirs) $(kde_appsdir)/Office/
	$(INSTALL_DATA) $(srcdir)/kexi.desktop $(kde_appsdir)/Office/kexi.desktop
#	$(mkinstalldirs) $(kde_servicetypesdir)/
#	$(INSTALL_DATA) $(srcdir)/kexiinterface.desktop $(kde_servicetypesdir)/kexiinterface.desktop
#	$(mkinstalldirs) $(kde_servicedir)
#	$(INSTALL_DATA) $(srcdir)/interfaces/mysqlinterface.desktop $(kde_servicesdir)/mysqlinterface.desktop
	$(mkinstalldirs) $(kde_mimedir)/application/
	$(INSTALL_DATA) $(srcdir)/x-kexi.desktop $(kde_mimedir)/application/x-kexi.desktop

uninstall-local:
	rm -f $(kde_appsdir)/Office/kexi.desktop
	rm -f $(kde_mimedir)/application/x-kexi.desktop

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


rcdir = $(kde_datadir)/kexi
rc_DATA = kexiui.rc kexiformeditorui.rc kexiquerydesignerui.rc

messages: rc.cpp
	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
	if test -n "$$LIST"; then \
	 $(XGETTEXT) $$LIST -o $(podir)/kexi.pot; \
	fi

