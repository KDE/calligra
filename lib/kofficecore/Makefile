####### General stuff

INCDIR	=	-I../kom -I$(QTDIR)/include -I$(KDEDIR)/include -I$(KDEDIR)/idl
CFLAGS	=	-g -fexceptions -O0
LFLAGS	=	-L$(QTDIR)/lib -L$(KDEDIR)/lib -L/usr/X11/lib -lqt -lkdecore -lkdeui -lkfile -lmico2.0.5 -lX11 -lXext -ldl -lkom
CC	=	gcc
MOC	=	moc

####### Library version

SOREV 	=	1.0.0
SOMAJOR = 	1
SOMINOR = 	0
SOPATCH =	0	

####### Files

SOURCES =	koIMR.cc koScanParts.cc
HEADERS =	koIMR.h koScanParts.h
OBJECTS =	koIMR.o koScanParts.o
SRCMETA =	
TARGET	=	libkofficecore.so.$(SOMAJOR).$(SOMINOR)
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
	rm -f $(KDEDIR)/lib/libkofficecore.so*
	$(INSTALL_DATA) libkofficecore* $(KDEDIR)/lib
	ln -s $(KDEDIR)/lib/$(TARGET) $(KDEDIR)/lib/libkofficecore.so.$(SOMAJOR)
	ln -s $(KDEDIR)/lib/$(TARGET) $(KDEDIR)/lib/libkofficecore.so
	$(INSTALL_DIR) $(KDEDIR)/idl
#	$(INSTALL_DATA) $(IDL) $(KDEDIR)/idl
#	$(INSTALL_DATA) $(IDLH) $(KDEDIR)/idl
	$(INSTALL_DATA) $(HEADERS) $(KDEDIR)/include

install-exec:
	rm -f $(KDEDIR)/lib/libkofficecore.so*
	$(INSTALL_DATA) libkofficecore* $(KDEDIR)/lib
	ln -s $(KDEDIR)/lib/$(TARGET) $(KDEDIR)/lib/libkofficecore.so.$(SOMAJOR)
	ln -s $(KDEDIR)/lib/$(TARGET) $(KDEDIR)/lib/libkofficecore.so

idl:

####### Meta objects



# DO NOT DELETE THIS LINE -- make depend depends on it.

koIMR.o: koIMR.h /usr/lib/qt/include/qstrlist.h /usr/lib/qt/include/qstring.h
koIMR.o: /usr/lib/qt/include/qarray.h /usr/lib/qt/include/qgarray.h
koIMR.o: /usr/lib/qt/include/qshared.h /usr/lib/qt/include/qglobal.h
koIMR.o: /usr/lib/qt/include/qgeneric.h /usr/include/string.h
koIMR.o: /usr/include/features.h /usr/include/sys/cdefs.h
koIMR.o: /usr/lib/gcc-lib/i486-linux/2.7.2.1/include/stddef.h
koIMR.o: /usr/lib/qt/include/qlist.h /usr/lib/qt/include/qglist.h
koIMR.o: /usr/lib/qt/include/qcollect.h /usr/lib/qt/include/qdstream.h
koIMR.o: /usr/lib/qt/include/qiodev.h /opt/kde/include/op_app.h
koIMR.o: /usr/lib/qt/include/qapp.h /usr/lib/qt/include/qwidget.h
koIMR.o: /usr/lib/qt/include/qwindefs.h /usr/lib/qt/include/qobjdefs.h
koIMR.o: /usr/lib/qt/include/qobject.h /usr/lib/qt/include/qevent.h
koIMR.o: /usr/lib/qt/include/qrect.h /usr/lib/qt/include/qsize.h
koIMR.o: /usr/lib/qt/include/qpoint.h /usr/lib/qt/include/qpaintd.h
koIMR.o: /usr/lib/qt/include/qpalette.h /usr/lib/qt/include/qcolor.h
koIMR.o: /usr/lib/qt/include/qcursor.h /usr/lib/qt/include/qfont.h
koIMR.o: /usr/lib/qt/include/qfontmet.h /usr/lib/qt/include/qfontinf.h
koIMR.o: /opt/kde/include/kom.h /opt/kde/include/kom_app.h
koIMR.o: /usr/lib/qt/include/qsocknot.h /usr/lib/qt/include/qtimer.h
koIMR.o: /usr/lib/qt/include/qintdict.h /usr/lib/qt/include/qgdict.h
koIMR.o: /opt/kde/include/kapp.h /opt/kde/include/kconfig.h
koIMR.o: /opt/kde/include/kconfigbase.h /opt/kde/include/kconfigdata.h
koIMR.o: /usr/lib/qt/include/qdict.h /usr/lib/qt/include/qtstream.h
koIMR.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
koIMR.o: /usr/lib/qt/include/qfile.h /opt/kde/include/kdebug.h
koIMR.o: /opt/kde/include/klocale.h /opt/kde/include/drag.h
koIMR.o: /usr/lib/qt/include/qpixmap.h /usr/include/X11/X.h
koIMR.o: /usr/lib/qt/include/qpopmenu.h /usr/lib/qt/include/qtablevw.h
koIMR.o: /usr/lib/qt/include/qframe.h /usr/lib/qt/include/qmenudta.h
koIMR.o: /opt/kde/include/topwidget_impl.h /opt/kde/include/controls.h
koIMR.o: /opt/kde/idl/kom.h /opt/kde/include/component_impl.h
koIMR.o: /opt/kde/include/base_impl.h /opt/kde/include/kom_var.h
koIMR.o: /opt/kde/include/control_frame.h /opt/kde/include/control_impl.h
koIMR.o: /opt/kde/include/kwm.h /usr/include/X11/Xlib.h
koIMR.o: /usr/include/sys/types.h /usr/include/linux/types.h
koIMR.o: /usr/include/linux/posix_types.h /usr/include/asm/posix_types.h
koIMR.o: /usr/include/asm/types.h /usr/include/sys/bitypes.h
koIMR.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
koIMR.o: /opt/kde/include/part_impl.h /opt/kde/include/parts.h
koIMR.o: /opt/kde/include/kmenubar.h /usr/lib/qt/include/qmenubar.h
koIMR.o: /opt/kde/include/ktoolbar.h /usr/lib/qt/include/qbutton.h
koIMR.o: /opt/kde/include/kcombo.h /usr/lib/qt/include/qcombo.h
koIMR.o: /opt/kde/include/klined.h /usr/lib/qt/include/qlined.h
koScanParts.o: koScanParts.h /usr/lib/qt/include/qstring.h
koScanParts.o: /usr/lib/qt/include/qarray.h /usr/lib/qt/include/qgarray.h
koScanParts.o: /usr/lib/qt/include/qshared.h /usr/lib/qt/include/qglobal.h
koScanParts.o: /usr/lib/qt/include/qgeneric.h /usr/include/string.h
koScanParts.o: /usr/include/features.h /usr/include/sys/cdefs.h
koScanParts.o: /usr/lib/gcc-lib/i486-linux/2.7.2.1/include/stddef.h
koScanParts.o: /usr/lib/qt/include/qstrlist.h /usr/lib/qt/include/qlist.h
koScanParts.o: /usr/lib/qt/include/qglist.h /usr/lib/qt/include/qcollect.h
koScanParts.o: /usr/lib/qt/include/qdstream.h /usr/lib/qt/include/qiodev.h
koScanParts.o: /opt/kde/include/op_app.h /usr/lib/qt/include/qapp.h
koScanParts.o: /usr/lib/qt/include/qwidget.h /usr/lib/qt/include/qwindefs.h
koScanParts.o: /usr/lib/qt/include/qobjdefs.h /usr/lib/qt/include/qobject.h
koScanParts.o: /usr/lib/qt/include/qevent.h /usr/lib/qt/include/qrect.h
koScanParts.o: /usr/lib/qt/include/qsize.h /usr/lib/qt/include/qpoint.h
koScanParts.o: /usr/lib/qt/include/qpaintd.h /usr/lib/qt/include/qpalette.h
koScanParts.o: /usr/lib/qt/include/qcolor.h /usr/lib/qt/include/qcursor.h
koScanParts.o: /usr/lib/qt/include/qfont.h /usr/lib/qt/include/qfontmet.h
koScanParts.o: /usr/lib/qt/include/qfontinf.h /opt/kde/include/kom.h
koScanParts.o: /opt/kde/include/kom_app.h /usr/lib/qt/include/qsocknot.h
koScanParts.o: /usr/lib/qt/include/qtimer.h /usr/lib/qt/include/qintdict.h
koScanParts.o: /usr/lib/qt/include/qgdict.h /opt/kde/include/kapp.h
koScanParts.o: /opt/kde/include/kconfig.h /opt/kde/include/kconfigbase.h
koScanParts.o: /opt/kde/include/kconfigdata.h /usr/lib/qt/include/qdict.h
koScanParts.o: /usr/lib/qt/include/qtstream.h /usr/include/stdio.h
koScanParts.o: /usr/include/libio.h /usr/include/_G_config.h
koScanParts.o: /usr/lib/qt/include/qfile.h /opt/kde/include/kdebug.h
koScanParts.o: /opt/kde/include/klocale.h /opt/kde/include/drag.h
koScanParts.o: /usr/lib/qt/include/qpixmap.h /usr/include/X11/X.h
koScanParts.o: /usr/lib/qt/include/qpopmenu.h /usr/lib/qt/include/qtablevw.h
koScanParts.o: /usr/lib/qt/include/qframe.h /usr/lib/qt/include/qmenudta.h
koScanParts.o: /opt/kde/include/topwidget_impl.h /opt/kde/include/controls.h
koScanParts.o: /opt/kde/idl/kom.h /opt/kde/include/component_impl.h
koScanParts.o: /opt/kde/include/base_impl.h /opt/kde/include/kom_var.h
koScanParts.o: /opt/kde/include/control_frame.h
koScanParts.o: /opt/kde/include/control_impl.h /opt/kde/include/kwm.h
koScanParts.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
koScanParts.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
koScanParts.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
koScanParts.o: /usr/include/sys/bitypes.h /usr/include/X11/Xfuncproto.h
koScanParts.o: /usr/include/X11/Xosdefs.h /opt/kde/include/part_impl.h
koScanParts.o: /opt/kde/include/parts.h /opt/kde/include/kmenubar.h
koScanParts.o: /usr/lib/qt/include/qmenubar.h /opt/kde/include/ktoolbar.h
koScanParts.o: /usr/lib/qt/include/qbutton.h /opt/kde/include/kcombo.h
koScanParts.o: /usr/lib/qt/include/qcombo.h /opt/kde/include/klined.h
koScanParts.o: /usr/lib/qt/include/qlined.h koIMR.h /usr/include/stdlib.h
koScanParts.o: /usr/include/errno.h /usr/include/linux/errno.h
koScanParts.o: /usr/include/asm/errno.h /usr/include/alloca.h
koScanParts.o: /usr/include/dirent.h /usr/include/gnu/types.h
koScanParts.o: /usr/include/linux/limits.h /usr/include/linux/dirent.h
koScanParts.o: /usr/include/posix1_lim.h /usr/include/sys/stat.h
koScanParts.o: /usr/include/linux/stat.h /opt/kde/include/ksimpleconfig.h
