
INCLUDES  = -I../lib/kofficecore $(MICO_INCLUDES) $(all_includes) -I$(idldir)
LDFLAGS   = $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD     = ../lib/kofficecore/libkofficecore.la \
	    ../lib/koml/libkoml.la ../lib/store/libkstore.la \
	    -lkparts -lkpartsui -lkom \
	    -lkio -lkdeutil $(LIBMICO) $(LIB_KDEUI)

####### Files

bin_PROGRAMS = kotrader.bin

kotrader_bin_SOURCES = trader_main.cc main.cc parse.cc parse_tree.cc y.tab.c \
		  lex.yy.c lookup.cc misc.cc \
		  typerepo_impl.cc init.cc activator.cc scan.cc \
		  proxy.cc link.cc register.cc kany.cc
noinst_HEADERS = parse.h parse_tree.h y.tab.h lookup.h misc.h \
	        trader_main.h typerepo_impl.h init.h activator.h \
		scan.h proxy.h link.h register.h kany.h kstrstream.h

idl_DATA =
idl_HEADERS =

DISTCLEANFILES = $(idl_DATA:.idl=.cc) $(idl_HEADERS) $(SRCMETA)

SUBDIRS = data

####### Implicit rules

SUFFIXES = .h .moc

.h.moc: 
	$(MOC) $< -o $@

####### Build rules

parser:
	flex lex.l
	yacc -d yacc.y
	flex -Pdep lex.dep.l
	bison -p dep -b dep -d yacc.dep.y 

IDL_FLAGS = --query-server-for-narrow --relative-paths -I./ -I../lib/kofficecore -I../lib/store -I../lib/interfaces -I$(idldir)

install-exec-local:
	$(INSTALL_SCRIPT) kotrader $(bindir)/kotrader

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot -x $(includedir)/corba.pot  *.cc && mv messages.po ../po/kotrader.pot

#trader.h: trader.idl
#	$(IDL) $(IDL_FLAGS) trader.idl
#
#typerepo.h: typerepo.idl
#	$(IDL) $(IDL_FLAGS) typerepo.idl
#
#tarder.cc: trader.h
#typerepo.cc: typerepo.h

