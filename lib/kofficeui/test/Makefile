# Generated automatically from Makefile.in by configure.

INCDIR		=	-I$(QTDIR)/include -I$(KDEDIR)/include  -I..
CXXFLAGS	=	$(INCDIR) -g 
LDFLAGS		=	-L$(KDEDIR)/lib -L/usr/X11/lib -L$(QTDIR)/lib -L.. -lkdeui -lkdecore -lX11 -lXext -ldl -lqt -lkofficeui
CXX		=	g++
MOC		=	/usr/bin/moc

####### Files

SOURCES	=	test.cc
HEADERS =	test.h
OBJECTS =	test.o
SRCMETA =	test.moc
TARGET	=	test

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

####### Meta objects

# DO NOT DELETE THIS LINE -- make depend depends on it.

