#!/bin/sh

# a little helper script to regenerate yacc.[c,h] and lex.c

YACC="bison"
LEX="flex"

$YACC -p kiotrader -d -o yacc.c yacc.y
$LEX -Pkiotrader -B -i -olex.c lex.l
