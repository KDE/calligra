lib_LTLIBRARIES = libkexisql.la

INCLUDES = $(all_includes)

libkexisql_la_SOURCES = attach.c auth.c btree.c btree_rb.c build.c  expr.c func.c hash.c \
insert.c os.c pager.c parse.c pragma.c printf.c random.c select.c copy.c delete.c encode.c \
table.c tokenize.c trigger.c update.c util.c vacuum.c vdbe.c where.c opcodes.c main.c
libkexisql_la_LIBADD =
libkexisql_la_LDFLAGS = $(all_libraries) --no-undefined

include $(top_srcdir)/kexi/Makefile.global
