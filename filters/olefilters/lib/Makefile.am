####### General stuff

INCLUDES = -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)
LDFLAGS	= $(all_libraries)

####### Files

noinst_LTLIBRARIES = libfilterbase.la libklaola.la

libfilterbase_la_SOURCES = filterbase.cc
libfilterbase_la_METASOURCES = filterbase.moc

libklaola_la_SOURCES = klaola.cc

noinst_HEADERS = filterbase.h klaola.h
