####
# Makefile.am for KIllustrator

MIMEDIR = $(kde_mimedir)/application
DATADIR = $(kde_datadir)/killustrator

INCLUDES= -I$(srcdir)/../share -I$(srcdir)/.. $(all_includes)

EXTRA_DIST = killustrator.xpm \
	killustrator-doc.xpm killustrator-doc-mini.xpm \
	killustrator.kdelnk x-killustrator.kdelnk \
	default.pal

bin_PROGRAMS = 	killustrator

# Which sources should be compiled for killustrator.
killustrator_SOURCES	= KIllustrator.cc main.cc

# the library search path
killustrator_LDFLAGS = $(all_libraries) $(KDE_RPATH)

# the libraries to link against. Be aware of the order. First the libraries,
# that depend on the following ones.
FILTER_LIBS = ../filter/libfilter.la
XML_LIBS = ../xmlutils/libxml.la
KSHARE_LIBS = ../share/libkillu.la
killustrator_LDADD   =  $(FILTER_LIBS) $(KSHARE_LIBS) $(XML_LIBS) \
	                $(LIB_KFILE) $(LIB_KIMGIO) $(LIBJPEG) $(LIBTIFF)

# this option you can leave out. Just, if you use "make dist", you need it
noinst_HEADERS = KIllustrator.h

# just to make sure, automake makes them
METASOURCES =	KIllustrator.moc

## Disabled - this is not KDE-1.x anymore
#appsdir = $(kde_appsdir)/Graphics
#apps_DATA = killustrator.kdelnk

#mimedir = $(kde_mimedir)/application
#mime_DATA = x-killustrator.kdelnk

#kde_icon_DATA = killustrator.xpm killustrator-doc.xpm killustrator-doc-mini.xpm

datadir = $(kde_datadir)/killustrator
data_DATA = default.pal

