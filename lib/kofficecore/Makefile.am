####### General stuff

INCLUDES= $(KOML_INCLUDES) $(KSTORE_INCLUDES) -I$(srcdir)/../ $(all_includes) \
$(MICO_INCLUDES)
LDFLAGS	= $(all_libraries)  -version-info 1:0:0
libkofficecore_la_LIBADD  = -lkparts ../koml/libkoml.la ../store/libkstore.la $(LIB_KIO) $(LIB_KFILE)

####### Files

lib_LTLIBRARIES = libkofficecore.la

libkofficecore_la_SOURCES = koDocument.cc \
	koStream.cc koFilterManager.cc \
	koMainWindow.cc \
	koApplication.cc koQueryTrader.cc \
	koUndo.cc koIcons.cc \
	koTrader.cc koTraderParse.cc koTraderParseTree.cc \
	yacc.c lex.c koDataTool.cc koFilter.cc

include_HEADERS = koFilter.h \
	koStream.h koDocument.h \
	koMainWindow.h \
	koApplication.h koQueryTypes.h \
	koFilterManager.h koUndo.h koIcons.h \
	koTrader.h koTraderParse.h koTraderParseTree.h \
	yacc.h koDataTool.h

noinst_HEADERS = koTraderParse.h koTraderParseTree.h yacc.h

libkofficecore_la_METASOURCES = AUTO

# idl_DATA = koffice.idl

IDL_FLAGS = --relative-paths --any -I$(top_srcdir)/lib/store -I$(interfacedir) -I$(srcdir) -I$(idldir)

# koffice.idl: $(idldir)/openparts.idl $(idldir)/openparts_ui.idl $(idldir)/kom.idl $(top_srcdir)/lib/store/koStoreIf.idl


parserfiles = yacc.y lex.l
EXTRA_DIST = $(parserfiles)

parser: $(parserfiles)
	cd $(srcdir) ;\
	flex lex.l && mv lex.yy.c lex.c ;\
	bison -d yacc.y && mv yacc.tab.c yacc.c; mv yacc.tab.h yacc.h
