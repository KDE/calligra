
INCLUDES  = $(PYTHONINC) $(MICO_INCLUDES) -I../lib/store -I../lib/koml -I../lib/kofficecore -I../lib/kofficeui -I../lib/interface $(all_includes) -I$(idldir)
LDFLAGS   = $(LIB_KIMGIO) $(all_libraries) $(PYTHONLIB) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD = ../lib/kofficeui/libkofficeui.la \
	../lib/kofficecore/libkofficecore.la ../lib/koml/libkoml.la \
	../lib/store/libkstore.la -lkparts -lkpartsui \
	-lkom -lkio -lkdeutil $(LIBMICO) $(LIBPYTHON) $(LIB_KFILE) 

####### Files

bin_PROGRAMS = koshell.bin

koshell_bin_SOURCES = koshell_main.cc koshell_shell.cc

noinst_HEADERS = koshell_main.h koshell_shell.h

SRCMETA = koshell_main.moc koshell_shell.moc

#idl_DATA = koshell.idl
#idl_HEADERS = koshell.h

DISTCLEANFILES = $(idl_DATA:.idl=.cc) $(idl_HEADERS) $(SRCMETA)

#kdelnk_DATA = koshell_part.kdelnk
#kdelnkdir = $(kde_datadir)/koffice/partlnk

####### Implicit rules

SUFFIXES = .h .moc

.h.moc: 
	$(MOC) $< -o $@

####### Build rules

IDL_FLAGS = --query-server-for-narrow --relative-paths -I../lib/kofficecore -I../lib/store -I../lib/interfaces -I$(idldir)

install-exec-local:
	$(INSTALL_SCRIPT) koshell $(bindir)/koshell

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot -x $(includedir)/corba.pot  *.cc && mv messages.po ../po/koshell.pot

# koshell.h: koshell.idl
#	$(IDL) $(IDL_FLAGS) koshell.idl

koshell_main.cc: koshell_main.moc
koshell_shell.cc: koshell_shell.moc
