include $(top_srcdir)/kexi/Makefile.global

lib_LTLIBRARIES = libkexisql2.la

bin_PROGRAMS = ksqlite2

INCLUDES = $(all_includes)

libkexisql2_la_SOURCES = attach.c auth.c btree.c btree_rb.c build.c date.c expr.c func.c hash.c \
insert.c os.c pager.c parse.c pragma.c printf.c random.c select.c copy.c delete.c encode.c \
table.c tokenize.c trigger.c update.c util.c vacuum.c vdbe.c vdbeaux.c where.c opcodes.c main.c

AM_CFLAGS = -DNO_TCL=1

ksqlite2_SOURCES = shell.c
ksqlite2_LDADD = libkexisql2.la $(LIBREADLINE)

libkexisql2_la_LIBADD =
libkexisql2_la_LDFLAGS = -version-info 2:8 $(all_libraries) --no-undefined

