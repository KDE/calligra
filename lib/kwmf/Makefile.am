####### General stuff

INCLUDES= -I$(srcdir) $(all_includes)
libkwmf_la_LDFLAGS = $(all_libraries) -version-info 1:0:0 -no-undefined
# We use kdecore for kdDebug :)
libkwmf_la_LIBADD = $(LIB_KDECORE)

####### Files

lib_LTLIBRARIES = libkwmf.la

libkwmf_la_SOURCES = kwmf.cc qwmf.cc

noinst_HEADERS = kwmf.h qwmf.h metafuncs.h wmfstruct.h
