####### General stuff

INCLUDES= -I../koml -I$(idldir) $(all_includes)
LDFLAGS	= $(all_libraries) -version-info 1:0:0
LIBADD  = -lkom -lkfile -lkdeui -lkdecore -lqt -lkdecore \
	$(LIBMICO) -lXext -lX11 -ldl 

####### Files

lib_LTLIBRARIES = libkofficecore.la
libkofficecore_la_SOURCES = koIMR.cc koScanParts.cc koStream.cc
include_HEADERS = koIMR.h koScanParts.h koStream.h

SUFFICES = .h .moc

.h.moc:
	$(MOC) $< -o $@      

idl:

