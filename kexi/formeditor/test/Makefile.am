## Makefile.am for kformdesigner

# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = kformdesigner

# set the include path for X, qt and KDE
INCLUDES = -I$(top_srcdir)/kexi/formeditor $(all_includes)

kformdesigner_LDFLAGS = $(KDE_RPATH) $(all_libraries)

kformdesigner_LDADD = $(top_builddir)/kexi/formeditor/libkformeditor.la

kformdesigner_SOURCES = main.cpp kfmview.cpp

# client stuff

METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kformdesigner.pot

KDE_ICON = kformdesigner

kdelnkdir = $(kde_appsdir)/Utilities
kdelnk_DATA = kformdesigner.desktop

rcdir = $(kde_datadir)/kformdesigner
rc_DATA = kfmui.rc
