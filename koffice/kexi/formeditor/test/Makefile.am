## Makefile.am for kformdesigner

include $(top_srcdir)/kexi/Makefile.global

# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = kformdesigner

# set the include path for X, qt and KDE
INCLUDES = -I$(top_srcdir)/kexi -I$(top_srcdir)/kexi/formeditor \
	-I$(top_srcdir)/kexi/widget -I$(top_srcdir)/kexi/core \
	-I$(top_srcdir)/lib -I$(top_srcdir)/lib/kofficecore $(all_includes)

kformdesigner_LDFLAGS = $(KDE_RPATH) $(all_libraries)

kformdesigner_LDADD = $(top_builddir)/kexi/formeditor/libkformdesigner.la

kformdesigner_SOURCES = main.cpp kfd_mainwindow.cpp

# client stuff

METASOURCES = AUTO

kdelnkdir = $(kde_appsdir)/Development
kdelnk_DATA = kformdesigner.desktop

rcdir = $(kde_datadir)/kformdesigner
rc_DATA = kfd_mainwindow.rc


# KFormDesigner KPart
kde_module_LTLIBRARIES = libkformdesigner_part.la

libkformdesigner_part_la_SOURCES = kfd_part.cpp
libkformdesigner_part_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries) $(VER_INFO)
libkformdesigner_part_la_LIBADD  = $(top_builddir)/kexi/formeditor/libkformdesigner.la $(LIB_KFILE)

# this is where the desktop file will go
partdesktopdir   = $(kde_servicesdir)
partdesktop_DATA = kformdesigner_part.desktop

# this is where the part's XML-GUI resource file goes
partrcdir   = $(kde_datadir)/kformdesigner_part
partrc_DATA = kformdesigner_part.rc kformdesigner_part_shell.rc

KDE_ICON = kformdesigner
