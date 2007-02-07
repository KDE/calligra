include $(top_srcdir)/kexi/Makefile.global

lib_LTLIBRARIES = libkexidatatable.la
libkexidatatable_la_SOURCES = kexidataawareobjectiface.cpp \
	kexitableview.cpp kexitableview_p.cpp kexidatatableview.cpp \
	kexicelleditorfactory.cpp kexitableedit.cpp \
	kexitableviewheader.cpp kexitableitem.cpp kexitableviewdata.cpp \
	kexidatetableedit.cpp kexitimetableedit.cpp kexidatetimetableedit.cpp \
	kexiinputtableedit.cpp kexiblobtableedit.cpp kexibooltableedit.cpp \
	kexicomboboxbase.cpp kexicomboboxtableedit.cpp kexicomboboxpopup.cpp \
	kexidataawarepropertyset.cpp kexitextformatter.cpp

noinst_HEADERS = kexitableview_p.h

libkexidatatable_la_LDFLAGS = $(all_libraries) $(VER_INFO) -Wnounresolved
libkexidatatable_la_LIBADD =  $(top_builddir)/kexi/core/libkexicore.la $(top_builddir)/kexi/widget/utils/libkexiguiutils.la \
	$(top_builddir)/lib/koproperty/libkoproperty.la $(LIB_KDEUI)

#TODO: remove libkexicore link when kexiutils arrive

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
INCLUDES= -I$(top_srcdir)/kexi -I$(top_srcdir)/kexi/core -I$(top_srcdir)/kexi/kexidb \
	-I$(top_srcdir)/lib -I$(top_srcdir)/lib/kofficecore -I$(top_srcdir)/lib/koproperty/ $(all_includes)

METASOURCES = AUTO

