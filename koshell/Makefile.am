
INCLUDES  = $(KOFFICE_INCLUDES) $(all_includes)

## The kdeinit loadable module
lib_LTLIBRARIES = koshell.la
koshell_la_SOURCES = koshell_main.cc koshell_shell.cc
koshell_la_LDFLAGS = $(all_libraries) -avoid-version -module
koshell_la_LIBADD = $(LIB_KOFFICEUI)

## The executable
bin_PROGRAMS = koshell
koshell_SOURCES = dummy.cc
koshell_LDFLAGS = $(all_libraries) $(KDE_RPATH)
koshell_LDADD = koshell.la

dummy.cc:
	echo > dummy.cc

noinst_HEADERS = koshell_shell.h

METASOURCES = AUTO

# The ui standards provide it all for us, currently.
# rcdir = $(kde_datadir)/koshell
# rc_DATA = koshell_shell.rc

applnk_DATA = koshell.desktop
applnkdir = $(kde_appsdir)/Office

messages:
	$(XGETTEXT) *.cc -o $(podir)/koshell.pot
