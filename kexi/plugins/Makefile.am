if compile_qsa_plugin
scripting_dir = scripting
endif

kde_module_LTLIBRARIES = kexihandler_table.la kexihandler_query.la kexihandler_relation.la #kexihandler_form.la

kexihandler_table_la_SOURCES = kexitablepart.cpp kexitablepartproxy.cpp kexialtertable.cpp
kexihandler_table_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
kexihandler_table_la_LIBADD = ../core/libkexicore.la ../tableview/libkexidatatable.la \
		../widget/libkexiextendedwidgets.la ../kexiDB/libkexidb.la ../formeditor/libkformeditor.la

kexihandler_query_la_SOURCES = kexiquerypart.cpp kexiquerydesigner.cpp kexiquerypartitem.cpp kexiquerypartproxy.cpp \
				kexiquerydesignerguieditor.cpp kexiquerydesignersql.cpp kexiquerydesignersqleditor.cpp \
				kexiquerydesignersqlhistory.cpp kexiparameterlisteditor.ui kexiaddparamwidget.ui \
				kexiaddparamdialog.cpp kexidynamicqueryparameterdialog.cpp

kexihandler_query_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
kexihandler_query_la_LIBADD = ../core/libkexicore.la ../tableview/libkexidatatable.la ../widget/libkexiextendedwidgets.la

kexihandler_relation_la_SOURCES = kexirelationhandler.cpp kexirelationhandlerproxy.cpp kexirelationview.cpp \
				kexirelationviewtable.cpp kexirelationdialog.cpp \
				kexirelationviewconnection.cpp
kexihandler_relation_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
kexihandler_relation_la_LIBADD = ../core/libkexicore.la

#kexihandler_form_la_SOURCES = kexiformbase.cpp kexiformhandler.cpp kexiformhandlerproxy.cpp kexiformhandleritem.cpp kexidbwidgetcontainer.cpp kexidbwidgets.cpp kexiwidgetprovider.cpp
#kexihandler_form_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
#kexihandler_form_la_LIBADD = ../core/libkexicore.la ../formeditor/libkformeditor.la \
#		../widget/libkexiextendedwidgets.la ../kexiDB/libkexidb.la

SUBDIRS = . $(scripting_dir) kugar
#SUBDIRS = . forms

KDE_CXXFLAGS = $(USE_EXCEPTIONS) -DKEXI_HAND_TBL_EXPORT= -DKEXICORE_EXPORT= -DKEXIDB_EXPORT= \
-DKEXIFILTER_EXPORT= -DKFORMEDITOR_EXPORT= -DKEXI_HAND_TBL_EXPORT= -DKEXI_HAND_QUERY_EXPORT= \
-DKEXI_HAND_RELAT_EXPORT= -DKEXIEXTWIDGETS_EXPORT= -DKEXI_HAND_FORM_EXPORT= -DKEXIDATATABLE_EXPORT=


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

# set the include path for X, qt and KDE - all_includes must remain last!
INCLUDES= $(KOFFICE_INCLUDES) -I$(top_srcdir)/kexi/core -I$(top_srcdir)/kexi \
		-I$(top_srcdir)/kexi/widget -I$(top_srcdir)/kexi/tableview \
		-I$(top_srcdir)/kexi/kexiDB $(all_includes)

METASOURCES = AUTO

