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

koAboutDia.o: koAboutDia.h /opt/kde/include/kbuttonbox.h
koAboutDia.o: /opt/kde/include/kapp.h /opt/kde/include/kconfig.h
koAboutDia.o: /opt/kde/include/kconfigbase.h /opt/kde/include/kconfigdata.h
koAboutDia.o: /opt/kde/include/kdebug.h /opt/kde/include/klocale.h
koAboutDia.o: /opt/kde/include/drag.h /usr/include/X11/X.h koAboutDia.moc
koPageLayoutDia.o: koPageLayoutDia.h /usr/include/stdio.h
koPageLayoutDia.o: /usr/include/libio.h /usr/include/_G_config.h
koPageLayoutDia.o: /usr/include/sys/cdefs.h /usr/include/stdlib.h
koPageLayoutDia.o: /usr/include/features.h
koPageLayoutDia.o: /usr/lib/gcc-lib/i486-linux/2.7.2.1/include/stddef.h
koPageLayoutDia.o: /usr/include/errno.h /usr/include/linux/errno.h
koPageLayoutDia.o: /usr/include/asm/errno.h /usr/include/alloca.h
koPageLayoutDia.o: /opt/kde/include/kapp.h /opt/kde/include/kconfig.h
koPageLayoutDia.o: /opt/kde/include/kconfigbase.h
koPageLayoutDia.o: /opt/kde/include/kconfigdata.h /opt/kde/include/kdebug.h
koPageLayoutDia.o: /opt/kde/include/klocale.h /opt/kde/include/drag.h
koPageLayoutDia.o: /usr/include/X11/X.h /opt/kde/include/krestrictedline.h
koPageLayoutDia.o: koPageLayoutDia.moc
koPartSelectDia.o: koPartSelectDia.h /opt/kde/include/koScanParts.h
koPartSelectDia.o: /opt/kde/include/op_app.h /opt/kde/include/kom.h
koPartSelectDia.o: /opt/kde/include/kom_app.h /opt/kde/include/kapp.h
koPartSelectDia.o: /opt/kde/include/kconfig.h /opt/kde/include/kconfigbase.h
koPartSelectDia.o: /opt/kde/include/kconfigdata.h /opt/kde/include/kdebug.h
koPartSelectDia.o: /opt/kde/include/klocale.h /opt/kde/include/drag.h
koPartSelectDia.o: /usr/include/X11/X.h /opt/kde/include/topwidget_impl.h
koPartSelectDia.o: /opt/kde/include/controls.h /opt/kde/idl/kom.h
koPartSelectDia.o: /opt/kde/include/component_impl.h
koPartSelectDia.o: /opt/kde/include/base_impl.h /opt/kde/include/kom_var.h
koPartSelectDia.o: /opt/kde/include/control_frame.h
koPartSelectDia.o: /opt/kde/include/control_impl.h /opt/kde/include/kwm.h
koPartSelectDia.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
koPartSelectDia.o: /usr/include/linux/types.h
koPartSelectDia.o: /usr/include/linux/posix_types.h
koPartSelectDia.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
koPartSelectDia.o: /usr/include/sys/bitypes.h /usr/include/X11/Xfuncproto.h
koPartSelectDia.o: /usr/include/X11/Xosdefs.h
koPartSelectDia.o: /usr/lib/gcc-lib/i486-linux/2.7.2.1/include/stddef.h
koPartSelectDia.o: /opt/kde/include/part_impl.h /opt/kde/include/parts.h
koPartSelectDia.o: /opt/kde/include/kmenubar.h /opt/kde/include/ktoolbar.h
koPartSelectDia.o: /opt/kde/include/kcombo.h /opt/kde/include/klined.h
koPartSelectDia.o: koPartSelectDia_data.h
koPartSelectDia_data.o: koPartSelectDia_data.h /opt/kde/include/klocale.h
koPartSelectDia_data.o: /opt/kde/include/kapp.h /opt/kde/include/kconfig.h
koPartSelectDia_data.o: /opt/kde/include/kconfigbase.h
koPartSelectDia_data.o: /opt/kde/include/kconfigdata.h
koPartSelectDia_data.o: /opt/kde/include/kdebug.h /opt/kde/include/drag.h
koPartSelectDia_data.o: /usr/include/X11/X.h
