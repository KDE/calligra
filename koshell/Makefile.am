
INCLUDES  = $(MICO_INCLUDES) $(KOFFICE_INCLUDES) $(all_includes) -I$(idldir)
LDFLAGS   = $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD = $(LIB_KOFFICEUI)

####### Files

bin_PROGRAMS = koshell

koshell_SOURCES = koshell_main.cc koshell_shell.cc

noinst_HEADERS = koshell_main.h koshell_shell.h

METASOURCES = koshell_main.moc koshell_shell.moc

####### Implicit rules

IDL_FLAGS = --query-server-for-narrow --relative-paths -I$(top_srcdir)/lib/kofficecore -I$(top_srcdir)/lib/store -I$(top_srcdir)/lib/interfaces -I$(idldir)

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot *.cc && mv messages.po ../po/koshell.pot

