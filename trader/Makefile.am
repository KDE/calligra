
INCLUDES  = -I$(top_builddir)/lib/kofficecore $(MICO_INCLUDES) $(all_includes) -I$(idldir)
LDFLAGS   = $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD     = -lkdeutil $(LIBMICO) $(LIB_KDECORE) 

####### Files

bin_PROGRAMS = kotrader

kotrader_SOURCES = trader_main.cc main.cc parse.cc parse_tree.cc y.tab.c lex.yy.c trader.cc lookup.cc misc.cc \
		typerepo.cc typerepo_impl.cc init.cc activator.cc scan.cc
noinst_HEADERS = parse.h parse_tree.h y.tab.h trader.h lookup.h misc.h trader_main.h typerepo.h \
		typerepo_impl.h init.h activator.h scan.h


idl_DATA =
idl_HEADERS =

DISTCLEANFILES = $(idl_DATA:.idl=.cc) $(idl_HEADERS) $(SRCMETA) trader.h trader.cc

SUBDIRS = data

####### Implicit rules


####### Build rules

parser:
	flex lex.l
	yacc -d yacc.y
	flex -Pdep lex.dep.l
	bison -p dep -b dep -d yacc.dep.y 

IDL_FLAGS = --query-server-for-narrow --relative-paths -I$(top_srcdir)/lib/kofficecore -I$(top_srcdir)/lib/store -I$(top_srcdir)/lib/interfaces -I$(idldir)

install-exec-local:
	$(INSTALL_SCRIPT) kotrader $(bindir)/kotrader

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot -x $(includedir)/corba.pot  *.cc && mv messages.po ../po/kotrader.pot


typerepo.cc: typerepo.h

trader.cc: trader.h
trader.h: $(top_srcdir)/lib/interfaces/trader.idl
	$(IDL) $(IDL_FLAGS) $(top_srcdir)/lib/interfaces/trader.idl
