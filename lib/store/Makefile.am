
lib_LTLIBRARIES = libkstore.la
LDFLAGS = -version-info 1:0:0 $(all_libraries)
INCLUDES = $(MICO_INCLUDES) -I$(idldir) $(all_includes)

####### Files

libkstore_la_LIBADD = $(LIB_QT) $(LIBMICO)
libkstore_la_SOURCES = koStore_idl.cc koStore.cc
include_HEADERS = koStore.h

idl_DATA = koStore.idl
idl_HEADERS =  koStore_idl.h

DISTCLEANFILES = koStore_idl.h koStore_idl.cc

####### Build rules

BUILT_SOURCES = koStore_idl.h
koStore_idl.h: koStore.idl
	$(IDL) -I./ --relative-paths --query-server-for-narrow --name koStore_idl $(srcdir)/koStore.idl

