####### General stuff

INCLUDES= -I$(srcdir) $(all_includes)
# Why is it a -module ? Shouldn't it have -version-info instead ? (DF)
libkwmf_la_LDFLAGS	= -module -avoid-version -no-undefined
libkwmf_la_LIBADD = $(KOFFICE_LIBS)

####### Files

lib_LTLIBRARIES = libkwmf.la

libkwmf_la_SOURCES = kwmf.cc

noinst_HEADERS = kwmf.h
