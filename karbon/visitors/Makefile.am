INCLUDES = \
	$(KOFFICE_INCLUDES) $(KOPAINTER_INCLUDES) \
	-I$(srcdir)/.. \
	-I$(srcdir)/../core \
	-I$(srcdir)/../tools \
	-I$(srcdir)/../render \
	$(LIBFREETYPE_CFLAGS) $(all_includes)

noinst_LTLIBRARIES = libkarbonvisitors.la

noinst_HEADERS = \
	vselectnodes.h \
	vselectobjects.h \
	vdrawselection.h \
	vselectiondesc.h \
	vtransformnodes.h 

libkarbonvisitors_la_SOURCES = \
	vselectnodes.cc \
	vselectobjects.cc \
	vdrawselection.cc \
	vselectiondesc.cc \
	vtransformnodes.cc 

libkarbonvisitors_la_METASOURCES = \
	AUTO
