
INCLUDES  = $(KOFFICE_INCLUDES) $(all_includes)
LDFLAGS   = $(all_libraries) $(KDE_RPATH)
LDADD = $(LIB_KOFFICEUI) 

####### Files

bin_PROGRAMS = koshell

koshell_SOURCES = koshell_main.cc koshell_shell.cc

noinst_HEADERS = koshell_shell.h

METASOURCES = koshell_shell.moc

# The ui standards provide it all for us, currently.
# rcdir = $(kde_datadir)/koshell
# rc_DATA = koshell_shell.rc

applnk_DATA = koshell.desktop
applnkdir = $(kde_appsdir)/Office

####### Implicit rules

messages: rc.cpp
	$(XGETTEXT) rc.cpp *.cc -o $(podir)/koshell.pot

