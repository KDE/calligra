####### General stuff

SUBDIRS = pics

bin_PROGRAMS = kformula.bin

INCLUDES = $(all_includes)
IBADD = $(all_libraries) -lkdeui -lkdecore -lqt $(LIBDL) $(LIBSOCKET) -lm

####### Files

kformula_bin_SOURCES = part.cpp widget.cpp main.cpp

noinst_HEADERS = part.h widget.h main.h

BUILTSOURCES =	part.moc widget.moc main.moc

part.cpp: part.moc
widget.cpp: widget.moc
main.cpp: main.moc

SUFFIXES = .h .moc

.h.moc: 
	$(MOC) $< -o $@      

bin_SCRIPTS = kformula

kdelnk_DATA = kformula_part.kdelnk
kdelnkdir = $(kde_datadir)/koffice/partlnk
