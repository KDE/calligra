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
APPSDIR = $(kde_appsdir)/share/apps

# set the include path for X, qt and KDE
INCLUDES= -I../share -I.. $(all_includes)
# claim, which subdirectories you want to install

EXTRA_DIST = killustrator.xpm killustrator.kdelnk

####### This part is project specific
# you can add here more. This one gets installed 
bin_PROGRAMS = 	killustrator

# Which sources should be compiled for killustrator.
killustrator_SOURCES	= KIllustrator.cc \
			  main.cc

# the library search path
killustrator_LDFLAGS = $(all_libraries) $(KDE_RPATH)

# the libraries to link against. Be aware of the order. First the libraries,
# that depend on the following ones.
FILTER_LIBS = ../filter/libfilter.a
XML_LIBS = ../xmlutils/libxml.a
KSHARE_LIBS = ../share/libkillu.a
killustrator_LDADD   =  $(FILTER_LIBS) $(KSHARE_LIBS) $(XML_LIBS) \
	-lkfile -lkfm -lkdeui -lkdecore \
	-lqt $(LIBQIMGIO) \
	-lXext  $(LIB_X11)

# this option you can leave out. Just, if you use "make dist", you need it
noinst_HEADERS = KIllustrator.h

# just to make sure, automake makes them 
BUILTSOURCES =	KIllustrator.moc 

TARGET	=	killustrator 

# if you "make distclean", this files get removed. If you want to remove
# them while "make clean", use CLEANFILES
DISTCLEANFILES = $(BUILTSOURCES)

# make messages.po. Move this one to ../po/ and "make merge" in po
messages:
	$(XGETTEXT) -C -ktranslate -ki18n $(killustrator_SOURCES) && mv messages.po ../po/killustrator.pot

# just install datas here. Use install-exec-data for scripts and etc.
# the binary itself is already installed from automake
# use mkinstalldirs, not "install -d"
# don't install a list of file. Just one file per install.
# if you have more of them, create a subdirectory with an extra Makefile 
install-data-local: 
#	$(mkinstalldirs) $(APPSDIR)
#	$(INSTALL_DATA) killustrator.kdelnk $(APPSDIR)
	$(mkinstalldirs) $(kde_icondir)
	$(INSTALL_DATA) killustrator.xpm $(kde_icondir)
#	$(mkinstalldirs) $(kde_minidir)
#	$(INSTALL_DATA) mini-killustrator.xpm $(kde_minidir)/killustrator.xpm

# remove ALL you have installed in install-data-local or install-exec-local
uninstall-local:
#	-rm -f $(APPSDIR)/killustrator.kdelnk
	-rm -f $(kde_icondir)/killustrator.xpm
#	-rm -f $(kde_minidir)/killustrator.xpm

# add a dependency for every moc file to be full portable
# I've added a key binding to emacs for this. 
KIllustrator.cc: KIllustrator.moc
KIllustrator.moc: KIllustrator.h
	$(MOC) KIllustrator.h -o KIllustrator.moc


