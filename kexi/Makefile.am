####### kdevelop will overwrite this part!!! (begin)##########
bin_PROGRAMS = kexi

## INCLUDES were found outside kdevelop specific part

kexi_SOURCES = kexiworkspace.cpp kexiinterfacetemplate.cpp kexiformbase.cpp kexiinterfacemanager.cpp kexiinterface.cpp kexicreateproject.cpp kexibrowser.cpp kexi.cpp main.cpp 
kexi_LDADD   =  $(LIB_KFILE) $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT) $(LIBSOCKET)

SUBDIRS = interfaces 

EXTRA_DIST = kexiui.rc kexi.desktop kexiinterfacetemplate.h kexiinterfacetemplate.cpp kexiglobal.h main.cpp kexi.cpp kexi.h kexiformbase.cpp kexiformbase.h kexiinterfacemanager.cpp kexiinterfacemanager.h kexiinterface.cpp kexiinterface.h kexibrowser.cpp kexibrowser.h kexicreateproject.cpp kexicreateproject.h 

install-data-local:
	$(mkinstalldirs) $(kde_appsdir)/Applications/
	$(INSTALL_DATA) $(srcdir)/kexi.desktop $(kde_appsdir)/Applications/kexi.desktop
	$(mkinstalldirs) $(kde_servicetypesdir)/
	$(INSTALL_DATA) $(srcdir)/kexiinterface.desktop $(kde_servicetypesdir)/kexiinterface.desktop
	$(mkinstalldirs) $(kde_servicedir)
	$(INSTALL_DATA) $(srcdir)/interfaces/mysqlinterface.desktop $(kde_servicesdir)/mysqlinterface.desktop
	
#	$(mkinstalldirs) $(kde_icondir)/locolor/32x32/apps/
#	$(INSTALL_DATA) $(srcdir)/lo32-app-kexi.png $(kde_icondir)/locolor/32x32/apps/kexi.png
#	$(mkinstalldirs) $(kde_icondir)/locolor/16x16/apps/
#	$(INSTALL_DATA) $(srcdir)/lo16-app-kexi.png $(kde_icondir)/locolor/16x16/apps/kexi.png
	$(mkinstalldirs) $(kde_icondir)/hicolor/16x16/actions/
	$(INSTALL_DATA)  $(srcdir)/hi16-action-table.png $(kde_icondir)/hicolor/16x16/actions/table.png
	$(INSTALL_DATA)  $(srcdir)/hi16-action-tables.png $(kde_icondir)/hicolor/16x16/actions/tables.png
	$(INSTALL_DATA)  $(srcdir)/hi16-action-db.png $(kde_icondir)/hicolor/16x16/actions/db.png
	$(INSTALL_DATA)  $(srcdir)/hi16-action-form.png $(kde_icondir)/hicolor/16x16/actions/form.png
	$(INSTALL_DATA)  $(srcdir)/hi16-action-forms.png $(kde_icondir)/hicolor/16x16/actions/forms.png
	$(INSTALL_DATA)  $(srcdir)/hi16-action-queries.png $(kde_icondir)/hicolor/16x16/actions/queries.png
	$(INSTALL_DATA)  $(srcdir)/hi16-action-query.png $(kde_icondir)/hicolor/16x16/actions/query.png
	$(INSTALL_DATA)  $(srcdir)/hi16-action-reports.png $(kde_icondir)/hicolor/16x16/actions/reports.png
	$(INSTALL_DATA)  $(srcdir)/hi16-action-report.png $(kde_icondir)/hicolor/16x16/actions/report.png
	$(INSTALL_DATA)  $(srcdir)/hi16-action-lineedit.png $(kde_icondir)/hicolor/22x22/actions/widget-lineedit.png
	$(INSTALL_DATA)  $(srcdir)/hi16-action-button.png $(kde_icondir)/hicolor/22x22/actions/widget-pushbutton.png
	$(mkinstalldirs) $(kde_icondir)/hicolor/16x16/apps/
	$(INSTALL_DATA)  $(srcdir)/hi16-app-kexi.png $(kde_icondir)/hicolor/16x16/apps/kexi.png
	$(mkinstalldirs) $(kde_icondir)/hicolor/32x32/apps/
	$(INSTALL_DATA)  $(srcdir)/hi32-app-kexi.png $(kde_icondir)/hicolor/32x32/apps/kexi.png
	$(mkinstalldirs) $(kde_icondir)/hicolor/48x48/apps/
	$(INSTALL_DATA)  $(srcdir)/hi48-app-kexi.png $(kde_icondir)/hicolor/48x48/apps/kexi.png
	$(mkinstalldirs) $(kde_datadir)/kexi
	$(INSTALL_DATA)  $(srcdir)/cp-wiz.png $(kde_datadir)/kexi/createproject.png
	
	
uninstall-local:
	-rm -f $(kde_appsdir)/Applications/kexi.desktop
	-rm -f $(kde_icondir)/locolor/32x32/apps/kexi.png
	-rm -f $(kde_icondir)/locolor/16x16/apps/kexi.png

####### kdevelop will overwrite this part!!! (end)############
# These paths are KDE specific. Use them:
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
INCLUDES= $(all_includes)

METASOURCES = AUTO

# the library search path. 
kexi_LDFLAGS = $(all_libraries) $(KDE_RPATH)

rcdir = $(kde_datadir)/kexi
rc_DATA = kexiui.rc

messages: rc.cpp
	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
	if test -n "$$LIST"; then \
	 $(XGETTEXT) $$LIST -o $(podir)/kexi.pot; \
	fi

