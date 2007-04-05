include $(top_srcdir)/kexi/Makefile.global

noinst_LTLIBRARIES = libkeximainstartup.la

libkeximainstartup_la_SOURCES = KexiConnSelectorBase.ui KexiProjectSelectorBase.ui \
	KexiOpenExistingFile.ui \
	KexiNewPrjTypeSelector.ui KexiDBTitlePageBase.ui \
	KexiServerDBNamePage.ui \
	KexiDBTitlePage.cpp \
	KexiConnSelector.cpp KexiProjectSelector.cpp \
	KexiStartupDialog.cpp \
	KexiStartupFileDialog.cpp KexiNewProjectWizard.cpp \
	KexiStartup.cpp KexiStartup_p.cpp KexiStartupDialogTemplatesPage.cpp

noinst_HEADERS = KexiStartup_p.h

libkeximainstartup_la_LDFLAGS = $(all_libraries) -Wnounresolved
libkeximainstartup_la_LIBADD = \
	../../widget/libkexiextendedwidgets.la

libkeximainstartup_la_METASOURCES = AUTO

SUBDIRS = .

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
INCLUDES= -I$(top_srcdir)/kexi -I$(top_srcdir)/kexi/main/startup -I$(top_srcdir)/kexi/core -I$(top_srcdir)/kexi/widget -I$(top_builddir)/kexi/widget $(all_includes)

