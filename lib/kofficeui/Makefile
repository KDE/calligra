####### General stuff

INCDIR	=	-I$(QTDIR)/include -I$(KDEDIR)/include
CFLAGS	=	-g 
LFLAGS	=	-L$(QTDIR)/lib -L$(KDEDIR)/lib -L/usr/X11/lib -lqt -lkdecore -lkdeui -lX11 -lXext -ldl
CC	=	gcc
MOC	=	moc

####### Files

SOURCES =	koAboutDia.cc koPageLayoutDia.cc koPartSelectDia.cc koPartSelectDia_data.cc
HEADERS =	koAboutDia.h koPageLayoutDia.h koPartSelectDia.h koPartSelectDia_data.h
OBJECTS =	koAboutDia.o koPageLayoutDia.o koPartSelectDia.o koPartSelectDia_data.o
SRCMETA =	koAboutDia.moc koPageLayoutDia.moc koPartSelectDia.moc koPartSelectDia_data.moc
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

all: $(TARGET)

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

koAboutDia.o: koAboutDia.h /usr/local/kde/include/kbuttonbox.h
koAboutDia.o: /usr/local/kde/include/kapp.h /usr/local/kde/include/kconfig.h
koAboutDia.o: /usr/local/kde/include/kconfigbase.h
koAboutDia.o: /usr/local/kde/include/kconfigdata.h
koAboutDia.o: /usr/local/kde/include/kdebug.h
koAboutDia.o: /usr/local/kde/include/klocale.h /usr/local/kde/include/drag.h
koAboutDia.o: /usr/include/X11/X.h
koPageLayoutDia.o: koPageLayoutDia.h /usr/include/stdio.h
koPageLayoutDia.o: /usr/include/libio.h /usr/include/features.h
koPageLayoutDia.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
koPageLayoutDia.o: /usr/include/_G_config.h /usr/include/gnu/types.h
koPageLayoutDia.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
koPageLayoutDia.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stdarg.h
koPageLayoutDia.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/va-alpha.h
koPageLayoutDia.o: /usr/include/stdio_lim.h /usr/include/stdlib.h
koPageLayoutDia.o: /usr/include/sys/types.h /usr/include/time.h
koPageLayoutDia.o: /usr/include/endian.h /usr/include/bytesex.h
koPageLayoutDia.o: /usr/include/sys/select.h /usr/include/selectbits.h
koPageLayoutDia.o: /usr/include/alloca.h /usr/local/kde/include/kapp.h
koPageLayoutDia.o: /usr/local/kde/include/kconfig.h
koPageLayoutDia.o: /usr/local/kde/include/kconfigbase.h
koPageLayoutDia.o: /usr/local/kde/include/kconfigdata.h
koPageLayoutDia.o: /usr/local/kde/include/kdebug.h
koPageLayoutDia.o: /usr/local/kde/include/klocale.h
koPageLayoutDia.o: /usr/local/kde/include/drag.h /usr/include/X11/X.h
koPageLayoutDia.o: /usr/local/kde/include/krestrictedline.h
koPartSelectDia.o: koPartSelectDia.h /usr/local/kde/include/koScanParts.h
koPartSelectDia.o: /usr/local/kde/include/op_app.h
koPartSelectDia.o: /usr/local/kde/include/kom.h
koPartSelectDia.o: /usr/local/kde/include/kom_app.h
koPartSelectDia.o: /usr/local/kde/include/kapp.h
koPartSelectDia.o: /usr/local/kde/include/kconfig.h
koPartSelectDia.o: /usr/local/kde/include/kconfigbase.h
koPartSelectDia.o: /usr/local/kde/include/kconfigdata.h
koPartSelectDia.o: /usr/local/kde/include/kdebug.h
koPartSelectDia.o: /usr/local/kde/include/klocale.h
koPartSelectDia.o: /usr/local/kde/include/drag.h /usr/include/X11/X.h
koPartSelectDia.o: /usr/local/kde/include/topwidget_impl.h
koPartSelectDia.o: /usr/local/kde/include/controls.h /opt/kde/idl/kom.h
koPartSelectDia.o: /usr/local/kde/include/component_impl.h
koPartSelectDia.o: /usr/local/kde/include/base_impl.h
koPartSelectDia.o: /usr/local/kde/include/kom_var.h
koPartSelectDia.o: /usr/local/kde/include/control_frame.h
koPartSelectDia.o: /usr/local/kde/include/control_impl.h
koPartSelectDia.o: /usr/local/kde/include/kwm.h /usr/include/X11/Xlib.h
koPartSelectDia.o: /usr/include/sys/types.h /usr/include/features.h
koPartSelectDia.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
koPartSelectDia.o: /usr/include/gnu/types.h /usr/include/time.h
koPartSelectDia.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
koPartSelectDia.o: /usr/include/endian.h /usr/include/bytesex.h
koPartSelectDia.o: /usr/include/sys/select.h /usr/include/selectbits.h
koPartSelectDia.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
koPartSelectDia.o: /usr/local/kde/include/part_impl.h
koPartSelectDia.o: /usr/local/kde/include/parts.h
koPartSelectDia.o: /usr/local/kde/include/kmenubar.h
koPartSelectDia.o: /usr/local/kde/include/ktoolbar.h
koPartSelectDia.o: /usr/local/kde/include/kcombo.h
koPartSelectDia.o: /usr/local/kde/include/klined.h koPartSelectDia_data.h
koPartSelectDia_data.o: koPartSelectDia_data.h
koPartSelectDia_data.o: /usr/local/kde/include/klocale.h
koPartSelectDia_data.o: /usr/local/kde/include/kapp.h
koPartSelectDia_data.o: /usr/local/kde/include/kconfig.h
koPartSelectDia_data.o: /usr/local/kde/include/kconfigbase.h
koPartSelectDia_data.o: /usr/local/kde/include/kconfigdata.h
koPartSelectDia_data.o: /usr/local/kde/include/kdebug.h
koPartSelectDia_data.o: /usr/local/kde/include/drag.h /usr/include/X11/X.h
