# Makefile.am for KexiMDB's stripped-down version of mdbtools

noinst_LTLIBRARIES = libmdb.la
libmdb_la_SOURCES = catalog.c mem.c file.c table.c data.c dump.c \
  money.c index.c write.c map.c options.c iconv.c sargs.c \
  like.c
libmdb_la_LIBADD=-lm

AM_CFLAGS = -DMETHOD= -DMDB_NO_BACKENDS -DMDB_NO_STATS \
 $(GLIB_CFLAGS) \
 -I${srcdir}/../include
