# Generated automatically from Makefile.in by configure.

INCDIR  =	-I$(QTDIR)/include -I$(KDEDIR)/include -I$(KDEDIR)/include/idl -I./
CXXFLAGS =	$(INCDIR) -g -fexceptions -O0
LDFLAGS =       -L/usr/X11/lib -L/opt/kde/lib -lqt -lX11 -lXext -lkdecore -lkdeui -lmico2.0.7 -lkom -lop -lkoml -ldl -lkfile -lkfm -lkofficecore -lkofficeui
CXX     =       g++
MOC     =       /usr/bin/moc

####### Files

SOURCES = 
HEADERS = 
OBJECTS = 
SRCMETA =	
TARGET	=	

####### Install

INSTALL = install -m 0755 -s
INSTALL_SHELL = install -m 0755
INSTALL_DATA = install -m 0644
INSTALL_DIR = install -d -m 0755
INSTALL_DIR_PRIVATE = install -d -m 0700

####### Implicit rules

.SUFFIXES: .cpp

.cpp.o:
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

idl: chart.idl
	idl --query-server-for-narrow -I$(KDEDIR)/include/idl chart.idl

install:
	$(INSTALL_DIR) $(KDEDIR)/idl
	$(INSTALL_DATA) chart.idl $(KDEDIR)/idl/chart.idl

####### Meta objects

# DO NOT DELETE THIS LINE -- make depend depends on it.

