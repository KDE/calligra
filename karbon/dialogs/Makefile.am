INCLUDES = $(KOFFICE_INCLUDES) \
	-I$(srcdir)/.. \
	-I$(srcdir)/../core \
	-I$(srcdir)/../commands \
	-I$(srcdir)/../tools \
	-I$(srcdir)/../widgets \
	$(all_includes)

noinst_LTLIBRARIES = \
	libkarbondialogs.la

noinst_HEADERS = \
	vcolordlg.h \
	vconfiguredlg.h \
	vellipsedlg.h \
	vfilldlg.h \
	vinsertknotsdlg.h \
	vpolygondlg.h \
	vpolygonizedlg.h \
	vrectangledlg.h \
	vroundrectdlg.h \
	vsinusdlg.h \
	vspiraldlg.h \
	vstardlg.h \
	vstrokedlg.h \
	vwhirlpinchdlg.h

libkarbondialogs_la_SOURCES = \
	dummy.cc \
	vcolordlg.cc \
	vconfiguredlg.cc \
	vellipsedlg.cc \
	vfilldlg.cc \
	vinsertknotsdlg.cc \
	vpolygondlg.cc \
	vpolygonizedlg.cc \
	vrectangledlg.cc \
	vroundrectdlg.cc \
	vsinusdlg.cc \
	vspiraldlg.cc \
	vstardlg.cc \
	vstrokedlg.cc \
	vwhirlpinchdlg.cc

libkarbondialogs_la_METASOURCES = \
	AUTO

DISTCLEANFILES = \
	dummy.cc

dummy.cc:
	echo > dummy.cc

