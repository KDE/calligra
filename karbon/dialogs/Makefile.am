INCLUDES = $(KOFFICE_INCLUDES) $(KOPAINTER_INCLUDES) \
	-I$(srcdir)/.. \
	-I$(srcdir)/../core \
	-I$(srcdir)/../commands \
	$(all_includes)

noinst_LTLIBRARIES = \
	libkarbondialogs.la

noinst_HEADERS = \
	vconfiguredlg.h \
	vfilldlg.h \
	vcolortab.h \
	vstrokedlg.h 

libkarbondialogs_la_SOURCES = \
	dummy.cc \
	vconfiguredlg.cc \
	vfilldlg.cc \
	vcolortab.cc \
	vstrokedlg.cc 

libkarbondialogs_la_METASOURCES = \
	AUTO

DISTCLEANFILES = \
	dummy.cc

dummy.cc:
	echo > dummy.cc

