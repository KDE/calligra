####
# Makefile.am for KIllustrator
#
# Most of this file is copied from kexample
#
###
# this 10 paths are KDE specific. Use them:
# kde_htmldir       Where your docs should go to. (contains lang subdirs)
# kde_appsdir       Where your application file (.kdelnk) should go to. 
# kde_icondir       Where your icon should go to.
# kde_minidir       Where your mini icon should go to.
# kde_datadir       Where you install application data. (Use a subdir)
# kde_locale        Where translation files should go to.(contains lang subdirs)
# kde_cgidir        Where cgi-bin executables should go to.
# kde_confdir       Where config files should go to.
# kde_mimedir       Where mimetypes should go to.
# kde_toolbardir    Where general toolbar icons should go to.
# kde_wallpaperdir  Where general wallpapers should go to.

# just set the variable

MIMEDIR = $(kde_mimedir)/application
DATADIR = $(kde_datadir)/killustrator

# set the include path for X, qt and KDE
INCLUDES= -I$(srcdir)/../share -I$(srcdir)/.. $(all_includes)
# claim, which subdirectories you want to install

EXTRA_DIST = killustrator.xpm \
	killustrator.kdelnk x-killustrator.kdelnk \
	default.pal

####### This part is project specific
# you can add here more. This one gets installed 
bin_PROGRAMS = 	killustrator

# Which sources should be compiled for killustrator.
killustrator_SOURCES	= KIllustrator.cc main.cc

# the library search path
killustrator_LDFLAGS = $(all_libraries) $(KDE_RPATH)

# the libraries to link against. Be aware of the order. First the libraries,
# that depend on the following ones.
FILTER_LIBS = ../filter/libfilter.a
XML_LIBS = ../xmlutils/libxml.a
KSHARE_LIBS = ../share/libkillu.a
killustrator_LDADD   =  $(FILTER_LIBS) $(KSHARE_LIBS) $(XML_LIBS) \
	   $(LIBQIMGIO) $(LIB_KFILE)

# this option you can leave out. Just, if you use "make dist", you need it
noinst_HEADERS = KIllustrator.h

# just to make sure, automake makes them 
METASOURCES =	KIllustrator.moc 

# if you "make distclean", this files get removed. If you want to remove
# them while "make clean", use CLEANFILES
DISTCLEANFILES = $(METASOURCES)

# make messages.po. Move this one to ../po/ and "make merge" in po
messages:
	$(XGETTEXT) -C -ktranslate -ki18n $(killustrator_SOURCES) && mv messages.po ../po/killustrator.pot

appsdir = $(kde_appsdir)/Graphics
apps_DATA = killustrator.kdelnk

mimedir = $(kde_mimedir)/application
mime_DATA = x-killustrator.kdelnk

kde_icon_DATA = killustrator.xpm

datadir = $(kde_datadir)/killustrator
data_DATA = default.pal

