include $(top_srcdir)/kexi/Makefile.global

noinst_LTLIBRARIES = libkexiuuid.la

INCLUDES = $(all_includes)

SUBDIRS = .


libkexiuuid_la_SOURCES = gen_uuid.c parse.c unparse.c compare.c isnull.c pack.c unpack.c copy.c
libkexiuuid_la_LDFLAGS = -no-undefined $(VER_INFO)
libkexiuuid_la_LIBADD =


