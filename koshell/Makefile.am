
INCLUDES  = $(KOFFICE_INCLUDES) $(all_includes)
LDFLAGS   = $(all_libraries) $(KDE_RPATH)
LDADD = $(LIB_KOFFICEUI) 

####### Files

bin_PROGRAMS = koshell

koshell_SOURCES = koshell_main.cc koshell_shell.cc

noinst_HEADERS = koshell_shell.h

METASOURCES = koshell_shell.moc

rcdir = $(kde_datadir)/koshell
rc_DATA = koshell_shell.rc

####### Implicit rules

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot *.cc && mv messages.po $(podir)/koshell.pot

