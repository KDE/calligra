kde4_header()

include_directories( ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/kexidb ${KDE4_INCLUDES}  )


########### next target ###############

set(kexidbparser_LIB_SRCS 
   sqlscanner.cpp 
   sqlparser.cpp 
   parser.cpp 
   parser_p.cpp )

kde4_automoc(${kexidbparser_LIB_SRCS})

kde4_add_library(kexidbparser SHARED ${kexidbparser_LIB_SRCS})

target_link_libraries(kexidbparser  ${KDE4_KDECORE_LIBS} ${KDE4_KPARTS_LIBS} ${KDE4_KDEUI_LIBS} kexidb )

set_target_properties(kexidbparser PROPERTIES VERSION 1.0.0 SOVERSION 1 )
install_targets(${LIB_INSTALL_DIR} kexidbparser )


########### install files ###############


kde4_footer()



#original Makefile.am contents follow:

#include $(top_srcdir)/kexi/Makefile.global
#
#lib_LTLIBRARIES = libkexidbparser.la
#libkexidbparser_la_SOURCES = sqlscanner.cpp sqlparser.cpp parser.cpp parser_p.cpp
#libkexidbparser_la_LIBADD = $(LIB_KPARTS) $(LIB_KDEUI) ../libkexidb.la
#libkexidbparser_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) $(VER_INFO)
#
#noinst_HEADERS = parser_p.h
#
#INCLUDES = -I$(top_srcdir)/kexi -I$(top_srcdir)/kexi/kexidb $(all_includes)
#METASOURCES = AUTO
#
#parser:
#	cd $(srcdir); \
#	lex -osqlscanner.cpp sqlscanner.l; \
#	bison -dv sqlparser.y; \
#	echo '#ifndef _SQLPARSER_H_' > sqlparser.h; \
#	echo '#define _SQLPARSER_H_' >> sqlparser.h; \
#	echo '#include "field.h"' >> sqlparser.h; \
#	echo '#include "parser.h"' >> sqlparser.h; \
#	echo '#include "sqltypes.h"' >> sqlparser.h; \
#	echo '' >> sqlparser.h; \
#	echo 'bool parseData(KexiDB::Parser *p, const char *data);' >> sqlparser.h; \
#	cat sqlparser.tab.h >> sqlparser.h; \
#	echo '#endif' >> sqlparser.h; \
#	cat sqlparser.tab.c > sqlparser.cpp; \
#	echo "const char * const tname(int offset) { return yytname[offset]; }" >> sqlparser.cpp; \
#	./extract_tokens.sh > tokens.cpp; \
#	rm -f sqlparser.tab.h sqlparser.tab.c
#
#coffie:
#	echo 'making coffie...'
#	sleep 5
#
#KDE_OPTIONS=nofinal
#KDE_CXXFLAGS += -DYYERROR_VERBOSE=1
#
#.PHONY: parser	coffie
