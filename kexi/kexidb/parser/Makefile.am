lib_LTLIBRARIES = libkexidbparser.la
libkexidbparser_la_SOURCES = sqlscanner.cpp sqlparser.cpp parser.cpp
libkexidbparser_la_LIBADD = $(LIB_KPARTS) $(LIB_KDEUI) ../libkexidb.la
libkexidbparser_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) --no-undefined

INCLUDES = $(all_includes) -I$(top_srcdir)/kexi -I$(top_srcdir)/kexi/kexidb
METASOURCES = AUTO

include $(top_srcdir)/kexi/Makefile.global

parser:
	lex sqlscanner.l
	mv lex.yy.c sqlscanner.cpp
	yacc -d sqlparser.y
	echo '#include <field.h>' > sqlparser.h
	echo '#include "parser.h"' >> sqlparser.h
#	echo 'using namespace KexiDB;' >> sqlparser.h
	echo '' >> sqlparser.h
	echo 'void parseData(KexiDB::Parser *p, const char *data);' >> sqlparser.h
	cat y.tab.h >> sqlparser.h

	cat y.tab.c > sqlparser.cpp

	rm -rf y.tab.h
	rm -rf y.tab.c

coffie:
	echo 'making coffie...'
	sleep 5

