####### General stuff

INCLUDES = -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)

####### Files

noinst_LTLIBRARIES = libfilterbase.la libklaola.la

libfilterbase_la_SOURCES = filterbase.cc
libfilterbase_la_METASOURCES = filterbase.moc
libfilterbase_la_LDFLAGS = $(all_libraries)

libklaola_la_SOURCES = klaola.cc
libklaola_la_LDFLAGS = $(all_libraries)

noinst_HEADERS = filterbase.h klaola.h
