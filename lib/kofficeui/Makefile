####### General stuff

INCDIR	=	-I$(QTDIR)/include -I$(KDEDIR)/include
CFLAGS	=	-g 
LFLAGS	=	-L$(QTDIR)/lib -L$(KDEDIR)/lib -L/usr/X11/lib -lqt -lkdecore -lkdeui -lX11 -lXext -ldl
CC	=	gcc
MOC	=	moc

####### Files

SOURCES =	koAboutDia.cc koPageLayoutDia.cc
HEADERS =	koAboutDia.h koPageLayoutDia.h
OBJECTS =	koAboutDia.o koPageLayoutDia.o
SRCMETA =	koAboutDia.moc koPageLayoutDia.moc
TARGET	=	libkofficeui.so.$(SOMAJOR).$(SOMINOR)

####### Library version

SOREV 	=	1.0.0
SOMAJOR = 	1
SOMINOR = 	0
SOPATCH =	0	

####### Install

INSTALL = install -m 0755 -s
INSTALL_SHELL = install -m 0755
INSTALL_DATA = install -m 0644
INSTALL_DIR = install -d -m 0755
INSTALL_DIR_PRIVATE = install -d -m 0700

####### Implicit rules

.SUFFIXES: .cc

.cc.o:
	$(CC) -c $(CFLAGS) -DSTDC_HEADERS -DHAVE_UNISTD_H $(INCDIR) -fPIC $<

.moc.o:
	$(CC) -c $(CFLAGS) -DSTDC_HEADERS -DHAVE_UNISTD_H $(INCDIR) -fPIC $<

%.moc: %.h
	$(MOC) $< -o $@      

####### Build rules

all: depend $(TARGET)

$(TARGET): $(SRCMETA) $(OBJECTS)
	rm -f $@
	$(CC) -shared -Wl,-soname,$(TARGET) -o $@ $(OBJECTS) $(LFLAGS)

depend:
	@makedepend -I$(INCDIR) $(SOURCES) 2> /dev/null

showfiles:
	@echo $(SOURCES) $(HEADERS) $(IDLH) Makefile

clean:
	-rm -f *.o *.bak *~ *% #*
	-rm -f $(SRCMETA) $(TARGET)

install:
	rm -f $(KDEDIR)/lib/libkofficeui.so*
	$(INSTALL_DATA) libkofficeui* $(KDEDIR)/lib
	ln -s $(KDEDIR)/lib/$(TARGET) $(KDEDIR)/lib/libkofficeui.so.$(SOMAJOR)
	ln -s $(KDEDIR)/lib/$(TARGET) $(KDEDIR)/lib/libkofficeui.so
	$(INSTALL_DATA) $(HEADERS) $(KDEDIR)/include

install-exec:
	rm -f $(KDEDIR)/lib/libkofficeui.so*
	$(INSTALL_DATA) libkofficeui* $(KDEDIR)/lib
	ln -s $(KDEDIR)/lib/$(TARGET) $(KDEDIR)/lib/libkofficeui.so.$(SOMAJOR)
	ln -s $(KDEDIR)/lib/$(TARGET) $(KDEDIR)/lib/libkofficeui.so

####### Meta objects



# DO NOT DELETE THIS LINE -- make depend depends on it.

