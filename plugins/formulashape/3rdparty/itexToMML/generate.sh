#! /bin/sh

patch -p2 -o itex2MML.y.patched < bison3-fix.patch
bison -y -p itex2MML_yy -d itex2MML.y.patched
flex -Pitex2MML_yy -o lex.yy.c itex2MML.l
