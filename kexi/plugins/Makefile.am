kde_module_LTLIBRARIES = kexihandler_table.la kexihandler_query.la kexihandler_relation.la

kexihandler_table_la_SOURCES = kexitablepart.cpp kexitablepartproxy.cpp kexialtertable.cpp
kexihandler_table_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
kexihandler_table_la_LIBADD = ../core/libkexicore.la ../datatable/libkexidatatable.la ../widget/libkexiextendedwidgets.la

kexihandler_query_la_SOURCES = kexiquerypart.cpp kexiquerydesigner.cpp kexiquerypartproxy.cpp \
				kexiquerydesignerguieditor.cpp
kexihandler_query_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
kexihandler_query_la_LIBADD = ../core/libkexicore.la ../datatable/libkexidatatable.la ../widget/libkexiextendedwidgets.la

kexihandler_relation_la_SOURCES = kexirelationhandler.cpp kexirelationhandlerproxy.cpp kexirelationview.cpp \
				kexirelationviewtable.cpp
kexihandler_relation_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
kexihandler_relation_la_LIBADD = ../core/libkexicore.la

SUBDIRS = .

KDE_CXXFLAGS = $(USE_EXCEPTIONS)

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
INCLUDES= $(KOFFICE_INCLUDES) $(all_includes) -I$(top_srcdir)/kexi/core -I$(top_srcdir)/kexi \
		-I$(top_srcdir)/kexi/widget -I$(top_srcdir)/kexi/datatable

METASOURCES = AUTO

