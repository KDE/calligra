add_subdirectory( utils )
add_subdirectory( tableview )
add_subdirectory( relations )

include_directories( ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/widget/tableview ${CMAKE_SOURCE_DIR}/kexi/core ${KOMAIN_INCLUDES} ${CMAKE_SOURCE_DIR}/libs ${KDE4_INCLUDES}  )


########### next target ###############

set(kexiextendedwidgets_LIB_SRCS 
   kexidataawareview.cpp 
   kexidatatable.cpp 
   kexiquerydesignersqleditor.cpp 
   kexiqueryparameters.cpp
   kexisectionheader.cpp 
#   pixmapcollection.cpp 
   kexiscrollview.cpp 
   kexidbconnectionwidget.cpp 
   kexidbdrivercombobox.cpp 
   kexieditor.cpp 
   kexifieldlistview.cpp 
   kexifieldcombobox.cpp 
   kexidatasourcecombobox.cpp 
   kexipropertyeditorview.cpp 
   kexicustompropertyfactory.cpp 
   kexicustompropertyfactory_p.cpp 
   kexicharencodingcombobox.cpp
   kexiprjtypeselector.cpp
   kexibrowser.cpp 
   kexibrowseritem.cpp 
)


kde4_add_ui_files(kexiextendedwidgets_LIB_SRCS
  kexiprjtypeselector.ui
  kexidbconnectionwidget.ui
  kexidbconnectionwidgetdetails.ui
)

kde4_add_library(kexiextendedwidgets SHARED ${kexiextendedwidgets_LIB_SRCS})

target_link_libraries(
    kexiextendedwidgets
    kexiutils
    kexiguiutils
    kexidatatable
    kexicore
    ktexteditor
    kexidb
    ${KOPROPERTY_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KDE3SUPPORT_LIBS}
    ${QT_QT3SUPPORT_LIBRARY}
    )

set_target_properties(kexiextendedwidgets PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_VERSION} )
install(TARGETS kexiextendedwidgets  ${INSTALL_TARGETS_DEFAULT_ARGS})


########### install files ###############






#original Makefile.am contents follow:

#include $(top_srcdir)/kexi/Makefile.global
#
#lib_LTLIBRARIES = libkexiextendedwidgets.la
#libkexiextendedwidgets_la_SOURCES = kexidataawareview.cpp \
#	kexidatatable.cpp kexidatatableview.cpp \
#	kexiquerydesignersqleditor.cpp  \
#	kexisectionheader.cpp pixmapcollection.cpp \
#	kexiscrollview.cpp kexidbconnectionwidgetbase.ui kexidbconnectionwidget.cpp \
#	kexidbconnectionwidgetdetailsbase.ui kexidbdrivercombobox.cpp \
#	kexieditor.cpp \
#	kexifieldlistview.cpp kexifieldcombobox.cpp kexidatasourcecombobox.cpp \
#	kexipropertyeditorview.cpp kexismalltoolbutton.cpp \
#	kexicustompropertyfactory.cpp kexicustompropertyfactory_p.cpp \
#	kexicharencodingcombobox.cpp
#
#libkexiextendedwidgets_la_LDFLAGS = $(all_libraries) $(VER_INFO) -Wnounresolved
#libkexiextendedwidgets_la_LIBADD = $(LIB_KDEUI) ./utils/libkexiguiutils.la tableview/libkexidatatable.la ../core/libkexicore.la -lktexteditor
#
#SUBDIRS = utils tableview .  relations
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
## set the include path for X, qt and KDE - all_includes must remain last!
#INCLUDES = -I$(top_srcdir)/kexi -I$(top_srcdir)/kexi/widget/tableview \
#	$(LIB_KEXI_KMDI_INCLUDES) \
#	-I$(top_srcdir)/kexi/core \
#	-I$(top_srcdir)/lib/komain -I$(top_srcdir)/lib $(all_includes)
#
#METASOURCES = AUTO
#
