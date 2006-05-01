#!/bin/sh

# A script for Kexi Developers
# Run this script to generate parser code using Lex and Bison tools

lex -osqlscanner.cpp sqlscanner.l
bison -dv sqlparser.y
echo '#ifndef _SQLPARSER_H_' > sqlparser.h
echo '#define _SQLPARSER_H_' >> sqlparser.h
echo '#include "field.h"' >> sqlparser.h
echo '#include "parser.h"' >> sqlparser.h
echo '#include "sqltypes.h"' >> sqlparser.h
echo '' >> sqlparser.h
echo 'bool parseData(KexiDB::Parser *p, const char *data);' >> sqlparser.h
cat sqlparser.tab.h >> sqlparser.h
echo '#endif' >> sqlparser.h
cat sqlparser.tab.c > sqlparser.cpp
echo "const char * const tname(int offset) { return yytname[offset]; }" >> sqlparser.cpp
./extract_tokens.sh > tokens.cpp
rm -f sqlparser.tab.h sqlparser.tab.c
