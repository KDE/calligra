#message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(KDE34COMPAT)")

########### install files ###############

install( FILES  kexihandler.desktop  DESTINATION ${SERVICETYPES_INSTALL_DIR})
install( FILES  kexirc  DESTINATION ${CONFIG_INSTALL_DIR})
install( FILES  kexiui.rc  DESTINATION ${DATA_INSTALL_DIR}/kexi)
# already installed by kdelibs/mimetypes/:
# install( FILES  kexi.magic   DESTINATION ${CONFIG_INSTALL_DIR}/magic)
# install( FILES
#	x-kexiproject-sqlite.desktop
#	x-kexi-connectiondata.desktop
#	x-kexiproject-sqlite2.desktop
#	x-kexiproject-sqlite3.desktop
#	x-kexiproject-shortcut.desktop
#	DESTINATION ${MIME_INSTALL_DIR}/application)


#original Makefile.am contents follow:

#include $(top_srcdir)/kexi/Makefile.global
#
## kde_appsdir         Where your application's menu entry (.desktop) should go to.
## kde_icondir         Where your icon should go to - better use KDE_ICON.
## kde_sounddir        Where your sounds should go to.
## kde_htmldir         Where your docs should go to. (contains lang subdirs)
## kde_datadir         Where you install application data. (Use a subdir)
## kde_locale          Where translation files should go to. (contains lang subdirs)
## kde_cgidir          Where cgi-bin executables should go to.
## kde_confdir         Where config files should go to (system-wide ones with default values).
## kde_mimedir         Where mimetypes .desktop files should go to.
## kde_servicesdir     Where services .desktop files should go to.
## kde_servicetypesdir Where servicetypes .desktop files should go to.
## kde_toolbardir      Where general toolbar icons should go to (deprecated, use KDE_ICON).
## kde_wallpaperdir    Where general wallpapers should go to.
## kde_templatesdir    Where templates for the "New" menu (Konqueror/KDesktop) should go to.
## kde_bindir          Where executables should go to. Use bin_PROGRAMS or bin_SCRIPTS.
## kde_libdir          Where shared libraries should go to. Use lib_LTLIBRARIES.
## kde_moduledir       Where modules (e.g. parts) should go to. Use kde_module_LTLIBRARIES.
## kde_styledir        Where Qt/KDE widget styles should go to (new in KDE 3).
## kde_designerdir     Where Qt Designer plugins should go to (new in KDE 3).
#
#rcdir = $(kde_datadir)/kexi
#rc_DATA = kexiui.rc
#
#kde_conf_DATA = kexirc
#
#servicetypesdir = $(kde_servicetypesdir)
#servicetypes_DATA=kexihandler.desktop
#
##servicesdir=$(kde_servicesdir)/kexi
##services_DATA=
#
#magicdir = $(kde_confdir)/magic
#magic_DATA = kexi.magic 
#
#
#mimetypedir = $(kde_mimedir)/application
#mimetype_DATA = x-kexiproject-sqlite.desktop \
#x-kexi-connectiondata.desktop \
#x-kexiproject-sqlite2.desktop \
#x-kexiproject-sqlite3.desktop \
#x-kexiproject-shortcut.desktop
#
##TODO: add x-kexi-shortcut-table etc.
#
##kde <=3.4
#if need_kde34_compatibility
#  KDE34COMPAT = kde34compat
#endif
#
#SUBDIRS = $(KDE34COMPAT)
