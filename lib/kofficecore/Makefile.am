####### General stuff

INCLUDES= -I../koml -I$(idldir) $(all_includes) $(MICO_INCLUDES)
LDFLAGS	= $(all_libraries)  -version-info 1:0:0
LIBADD  = -lkom -lkfile -lkdeui -lkdecore -lqt -lkdecore \
	$(LIBMICO) -lXext -lX11

####### Files

lib_LTLIBRARIES = libkofficecore.la
libkofficecore_la_SOURCES = koIMR.cc koScanParts.cc koStream.cc koDocument.cc
include_HEADERS = koIMR.h koScanParts.h koStream.h koDocument.h

SUFFICES = .h .moc

.h.moc:
	$(MOC) $< -o $@      

idl:

