INCLUDES = $(KOFFICE_INCLUDES) $(KOPAINTER_INCLUDES) \
	-I$(srcdir)/.. \
	-I$(srcdir)/../commands \
	-I$(srcdir)/../core \
	-I$(srcdir)/../dialogs \
	-I$(srcdir)/../widgets \
	-I$(srcdir)/../dockers \
	-I$(srcdir)/../render \
	-I$(srcdir)/../shapes \
	-I$(srcdir)/../visitors \
	$(all_includes)

noinst_LTLIBRARIES = libkarbontools.la

noinst_HEADERS = \
	vrotatetool.h \
	vselectnodestool.h \
	vselecttool.h \
	vsheartool.h \
	vtoolfactory.h \
	vtool.h

libkarbontools_la_SOURCES = \
	vrotatetool.cc \
	vselectnodestool.cc \
	vselecttool.cc \
	vsheartool.cc \
	vtoolfactory.cc \
	vtool.cc

libkarbontools_la_METASOURCES = \
	AUTO
