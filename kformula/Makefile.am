####### General stuff

SUBDIRS = pics
libdir = $(prefix)/parts

lib_LTLIBRARIES = libkformula.la

CXXFLAGS += -export-dynamic
INCLUDES=	$(all_includes)
libkformula_la_LIBADD  =	$(all_libraries) -lkdeui -lkdecore -lqt -ldl -lm
MOC	=	@MOC@

####### Files

libkformula_la_SOURCES = part.cpp widget.cpp main.cpp

noinst_HEADERS = part.h widget.h

BUILTSOURCES =	part.moc widget.moc

part.cpp: part.moc
widget.cpp: widget.moc

%.moc: %.h
	$(MOC) $< -o $@      

all: $(TARGET)

install-data-local:
	$(mkinstalldirs) $(prefix)/lib/kformula/scripts
	$(INSTALL) kformula.kdelnk $(libdir)

install-exec-local:
	$(mkinstalldirs) $(bindir)
	$(INSTALL_SCRIPT) kformula $(bindir)

