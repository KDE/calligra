# Generated automatically from Makefile.in by configure.

INCDIR		=	-I$(QTDIR)/include -I$(KDEDIR)/include -I$(KDEDIR)/idl -I../lib/kofficeui
CXXFLAGS	=	$(INCDIR) -g -fexceptions -O0
LDFLAGS		=	-L$(KDEDIR)/lib -L/usr/X11/lib -L$(QTDIR)/lib -L$../lib/kofficeui -lkdeui -lkdecore -lkfm -lmico2.0.5 -lX11 -lXext -ldl -lkom -lop -ljpeg -lkfile -lqt -lkofficeui
CXX		=	g++
MOC		=	/usr/bin/moc

####### Files

SOURCES	=	kpresenter_main.cc kpresenter_doc.cc kpresenter_view.cc \
		kpresenter.cc kpresenter_shell.cc graphobj.cc atfinterpreter.cc\
		backdia.cc page.cc ktextobject.cc afchoose.cc styledia.cc \
		jpeg.cc formats.cc qwmf.cc 
HEADERS =	kpresenter_main.h kpresenter_doc.h kpresenter_view.h \
		kpresenter.h kpresenter_shell.h graphobj.h atfinterpreter.h \
		backdia.h page.h ktextobject.h afchoose.h styledia.h \
		jpeg.h formats.h qwmf.h global.h
OBJECTS =	kpresenter_main.o kpresenter_doc.o kpresenter_view.o \
		kpresenter.o kpresenter_shell.o graphobj.o atfinterpreter.o \
		backdia.o page.o ktextobject.o afchoose.o styledia.o \
		jpeg.o formats.o qwmf.o
SRCMETA =	kpresenter_main.moc kpresenter_doc.moc kpresenter_view.moc \
		kpresenter_shell.moc graphobj.moc atfinterpreter.moc page.moc ktextobject.moc\
		backdia.moc afchoose.moc styledia.moc
TARGET	=	kpresenter
IDL	=	kpresenter.idl
IDLH	=	kpresenter.h

####### Implicit rules

.SUFFIXES: .cc

.cc.o:
	$(CXX) -c $(CXXFLAGS) -DSTDC_HEADERS -DHAVE_UNISTD_H $(INCDIR) $<

%.moc: %.h
	$(MOC) $< -o $@      

####### Build rules

all: $(TARGET)

$(TARGET): $(SRCMETA) $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

depend:
	@makedepend $(INCDIR) $(SOURCES) 2> /dev/null

showfiles:
	@echo $(SOURCES) $(HEADERS) Makefile

clean:
	-rm -f *.o *.bak *~ *% #*
	-rm -f $(SRCMETA) $(TARGET)

idl: kpresenter.idl
	idl -I$(KDEDIR)/idl kpresenter.idl

####### Meta objects

# DO NOT DELETE THIS LINE -- make depend depends on it.
