INCLUDES = $(KOFFICE_INCLUDES) \
	-I$(srcdir)/.. \
	-I$(srcdir)/../core \
	-I$(srcdir)/../commands \
	-I$(srcdir)/../tools \
	-I$(srcdir)/../widgets \
	$(all_includes)

noinst_LTLIBRARIES = \
	libkarbondockers.la

noinst_HEADERS = \
	vdocker.h

libkarbondockers_la_SOURCES = \
	dummy.cc \
	vdocker.cc

libkarbondialogs_la_METASOURCES = \
	AUTO

DISTCLEANFILES = \
	dummy.cc

dummy.cc:
	echo > dummy.cc

