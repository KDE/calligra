INCLUDES = $(KOFFICE_INCLUDES) $(KOPAINTER_INCLUDES) \
	-I$(srcdir)/.. \
	-I$(srcdir)/../core \
	-I$(srcdir)/../commands \
	-I$(srcdir)/../tools \
	-I$(srcdir)/../widgets \
	$(all_includes)

noinst_LTLIBRARIES = \
	libkarbondialogs.la

noinst_HEADERS = \
	vconfiguredlg.h \
	vfilldlg.h \
	vcolortab.h \
	vflattendlg.h \
	vinsertknotsdlg.h \
	vroundcornersdlg.h \
	vstrokedlg.h \
	vtransformdlg.h \
	vwhirlpinchdlg.h

libkarbondialogs_la_SOURCES = \
	dummy.cc \
	vconfiguredlg.cc \
	vfilldlg.cc \
	vcolortab.cc \
	vflattendlg.cc \
	vinsertknotsdlg.cc \
	vroundcornersdlg.cc \
	vstrokedlg.cc \
	vtransformdlg.cc \
	vwhirlpinchdlg.cc

libkarbondialogs_la_METASOURCES = \
	AUTO

DISTCLEANFILES = \
	dummy.cc

dummy.cc:
	echo > dummy.cc

