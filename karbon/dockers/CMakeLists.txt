




########### next target ###############


########### install files ###############






#original Makefile.am contents follow:

#INCLUDES = $(KOFFICE_INCLUDES) $(KOPAINTER_INCLUDES) \
#	-I$(srcdir)/.. \
#	-I$(srcdir)/../core \
#	-I$(srcdir)/../commands \
#	-I$(srcdir)/../render \
#	-I$(srcdir)/../tools \
#	-I$(srcdir)/../widgets \
#	-I$(srcdir)/../dialogs \
#	$(all_includes)
#
#noinst_LTLIBRARIES = \
#	libkarbondockers.la
#
#noinst_HEADERS = \
#	vcolordocker.h \
#	vdocumentdocker.h \
#	vstrokedocker.h \
#	vstyledocker.h \
#	vtransformdocker.h
#
#libkarbondockers_la_SOURCES = \
#	dummy.cc \
#	vcolordocker.cc \
#	vdocumentdocker.cc \
#	vstrokedocker.cc \
#	vstyledocker.cc \
#	vtransformdocker.cc
#
#libkarbondockers_la_METASOURCES = AUTO
#
#DISTCLEANFILES = \
#	dummy.cc
#
#dummy.cc:
#	echo > dummy.cc
#
