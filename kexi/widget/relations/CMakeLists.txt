

include_directories( ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/core ${KDE4_INCLUDES}  )


########### next target ###############

set(kexirelationsview_LIB_SRCS 
   kexirelationview.cpp 
   kexirelationviewconnection.cpp 
   kexirelationviewtable.cpp 
   kexirelationwidget.cpp )

kde4_automoc(${kexirelationsview_LIB_SRCS})

kde4_add_library(kexirelationsview SHARED ${kexirelationsview_LIB_SRCS})

target_link_libraries(kexirelationsview  ${KDE4_KDECORE_LIBS} kexicore )

set_target_properties(kexirelationsview PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )
install(TARGETS kexirelationsview  DESTINATION ${LIB_INSTALL_DIR})


########### install files ###############






#original Makefile.am contents follow:

#include $(top_srcdir)/kexi/Makefile.global
#
#lib_LTLIBRARIES = libkexirelationsview.la
#
#libkexirelationsview_la_SOURCES = kexirelationview.cpp kexirelationviewconnection.cpp \
#				kexirelationviewtable.cpp kexirelationwidget.cpp
#
#libkexirelationsview_la_LDFLAGS = $(all_libraries) $(VER_INFO) -Wnounresolved
#libkexirelationsview_la_LIBADD = ../../core/libkexicore.la
#
#libkexirelationsview_la_METASOURCES = AUTO
#
#SUBDIRS = .
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
## set the include path for X, qt and KDE
#INCLUDES= -I$(top_srcdir)/kexi $(LIB_KEXI_KMDI_INCLUDES) \
#	-I$(top_srcdir)/kexi/core $(all_includes)
#
