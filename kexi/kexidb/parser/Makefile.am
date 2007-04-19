include $(top_srcdir)/kexi/Makefile.global

lib_LTLIBRARIES = libkexidbparser.la
libkexidbparser_la_SOURCES = sqlscanner.cpp sqlparser.cpp parser.cpp parser_p.cpp
libkexidbparser_la_LIBADD = $(LIB_KPARTS) $(LIB_KDEUI) ../libkexidb.la
libkexidbparser_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) $(VER_INFO)

noinst_HEADERS = parser_p.h

kexidbparserinclude_HEADERS = parser.h

INCLUDES = -I$(top_srcdir)/kexi -I$(top_srcdir)/kexi/kexidb $(all_includes)
METASOURCES = AUTO

parser:
	cd $(srcdir); \
	lex -osqlscanner.cpp sqlscanner.l; \
	bison -dv sqlparser.y; \
	echo '#ifndef _SQLPARSER_H_' > sqlparser.h; \
	echo '#define _SQLPARSER_H_' >> sqlparser.h; \
	echo '#include "field.h"' >> sqlparser.h; \
	echo '#include "parser.h"' >> sqlparser.h; \
	echo '#include "sqltypes.h"' >> sqlparser.h; \
	echo '' >> sqlparser.h; \
	echo 'bool parseData(KexiDB::Parser *p, const char *data);' >> sqlparser.h; \
	cat sqlparser.tab.h >> sqlparser.h; \
	echo '#endif' >> sqlparser.h; \
	cat sqlparser.tab.c > sqlparser.cpp; \
	echo "const char * const tname(int offset) { return yytname[offset]; }" >> sqlparser.cpp; \
	./extract_tokens.sh > tokens.cpp; \
	rm -f sqlparser.tab.h sqlparser.tab.c

coffie:
	echo 'making coffie...'
	sleep 5

KDE_OPTIONS=nofinal
KDE_CXXFLAGS += -DYYERROR_VERBOSE=1

.PHONY: parser	coffie
