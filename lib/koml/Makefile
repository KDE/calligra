####### General stuff

INCDIR	=	
CFLAGS	=	-g -fexceptions -O0
LFLAGS	=	
CC	=	g++
MOC	=	moc

####### Library version

SOREV 	=	1.0.0
SOMAJOR = 	1
SOMINOR = 	0
SOPATCH =	0	

####### Files

SOURCES =	komlParser.cc komlFeed.cc komlFileFeed.cc komlWriter.cc komlStreamFeed.cc torben.cc
HEADERS =	komlParser.h komlFeed.h komlFileFeed.h komlWriter.h torben.h komlStreamFeed.h
OBJECTS =	komlParser.o komlFeed.o komlFileFeed.o komlWriter.o komlStreamFeed.o torben.o
SRCMETA =	
TARGET	=	libkoml.so.$(SOMAJOR).$(SOMINOR)
IDL	=	
IDLH	=	

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

%.moc: %.h
	$(MOC) $< -o $@      

####### Build rules

all: $(TARGET)

$(TARGET): $(SRCMETA) $(OBJECTS)
	rm -f $@
	$(CC) -shared -Wl,-soname,$(TARGET) -o $@ $(OBJECTS) $(LFLAGS)

depend:
	@makedepend -I$(INCDIR) $(SOURCES) 2> /dev/null

showfiles:
	@echo $(SOURCES) $(HEADERS) Makefile

clean:
	-rm -f *.o *.bak *~ *% #*
	-rm -f $(SRCMETA) $(TARGET)

install:
	rm -f $(KDEDIR)/lib/libkoml.so*
	$(INSTALL_DATA) libkoml* $(KDEDIR)/lib
	ln -s $(KDEDIR)/lib/$(TARGET) $(KDEDIR)/lib/libkoml.so.$(SOMAJOR)
	ln -s $(KDEDIR)/lib/$(TARGET) $(KDEDIR)/lib/libkoml.so
	$(INSTALL_DIR) $(KDEDIR)/idl
	$(INSTALL_DATA) $(HEADERS) $(KDEDIR)/include

install-exec:
	rm -f $(KDEDIR)/lib/libkoml.so*
	$(INSTALL_DATA) libkoml* $(KDEDIR)/lib
	ln -s $(KDEDIR)/lib/$(TARGET) $(KDEDIR)/lib/libkoml.so.$(SOMAJOR)
	ln -s $(KDEDIR)/lib/$(TARGET) $(KDEDIR)/lib/libkoml.so

idl:

####### Meta objects



# DO NOT DELETE THIS LINE -- make depend depends on it.

komlFeed.o: komlFeed.h
komlFileFeed.o: komlFileFeed.h /usr/include/stdio.h /usr/include/libio.h
komlFileFeed.o: /usr/include/features.h /usr/include/sys/cdefs.h
komlFileFeed.o: /usr/include/gnu/stubs.h /usr/include/_G_config.h
komlFileFeed.o: /usr/include/gnu/types.h
komlFileFeed.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
komlFileFeed.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stdarg.h
komlFileFeed.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/va-alpha.h
komlFileFeed.o: /usr/include/stdio_lim.h komlFeed.h
komlWriter.o: komlWriter.h
komlStreamFeed.o: komlStreamFeed.h komlFeed.h
torben.o: torben.h /usr/include/ctype.h /usr/include/features.h
torben.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
torben.o: /usr/include/endian.h /usr/include/bytesex.h
