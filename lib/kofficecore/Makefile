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

SOURCES =	koIMR.cc koScanParts.cc koStream.cc
HEADERS =	koIMR.h koScanParts.h koStream.h
OBJECTS =	koIMR.o koScanParts.o koStream.o
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
koIMR.o: /usr/include/gnu/stubs.h
koIMR.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
koIMR.o: /usr/lib/qt/include/qlist.h /usr/lib/qt/include/qglist.h
koIMR.o: /usr/lib/qt/include/qcollect.h /usr/lib/qt/include/qdstream.h
koIMR.o: /usr/lib/qt/include/qiodev.h /usr/local/kde/include/op_app.h
koIMR.o: /usr/lib/qt/include/qapp.h /usr/lib/qt/include/qwidget.h
koIMR.o: /usr/lib/qt/include/qwindefs.h /usr/lib/qt/include/qobjdefs.h
koIMR.o: /usr/lib/qt/include/qobject.h /usr/lib/qt/include/qevent.h
koIMR.o: /usr/lib/qt/include/qrect.h /usr/lib/qt/include/qsize.h
koIMR.o: /usr/lib/qt/include/qpoint.h /usr/lib/qt/include/qpaintd.h
koIMR.o: /usr/lib/qt/include/qpalette.h /usr/lib/qt/include/qcolor.h
koIMR.o: /usr/lib/qt/include/qcursor.h /usr/lib/qt/include/qfont.h
koIMR.o: /usr/lib/qt/include/qfontmet.h /usr/lib/qt/include/qfontinf.h
koIMR.o: /usr/local/kde/include/kom.h /usr/local/kde/include/kom_app.h
koIMR.o: /usr/lib/qt/include/qsocknot.h /usr/lib/qt/include/qtimer.h
koIMR.o: /usr/lib/qt/include/qintdict.h /usr/lib/qt/include/qgdict.h
koIMR.o: /usr/local/kde/include/kapp.h /usr/local/kde/include/kconfig.h
koIMR.o: /usr/local/kde/include/kconfigbase.h
koIMR.o: /usr/local/kde/include/kconfigdata.h /usr/lib/qt/include/qdict.h
koIMR.o: /usr/lib/qt/include/qtstream.h /usr/include/stdio.h
koIMR.o: /usr/include/libio.h /usr/include/_G_config.h
koIMR.o: /usr/include/gnu/types.h
koIMR.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stdarg.h
koIMR.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/va-alpha.h
koIMR.o: /usr/include/stdio_lim.h /usr/lib/qt/include/qfile.h
koIMR.o: /usr/local/kde/include/kdebug.h /usr/local/kde/include/klocale.h
koIMR.o: /usr/local/kde/include/drag.h /usr/lib/qt/include/qpixmap.h
koIMR.o: /usr/include/X11/X.h /usr/lib/qt/include/qpopmenu.h
koIMR.o: /usr/lib/qt/include/qtablevw.h /usr/lib/qt/include/qframe.h
koIMR.o: /usr/lib/qt/include/qmenudta.h
koIMR.o: /usr/local/kde/include/topwidget_impl.h
koIMR.o: /usr/local/kde/include/controls.h /opt/kde/idl/kom.h
koIMR.o: /usr/local/kde/include/component_impl.h
koIMR.o: /usr/local/kde/include/base_impl.h /usr/local/kde/include/kom_var.h
koIMR.o: /usr/local/kde/include/control_frame.h
koIMR.o: /usr/local/kde/include/control_impl.h /usr/local/kde/include/kwm.h
koIMR.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h /usr/include/time.h
koIMR.o: /usr/include/endian.h /usr/include/bytesex.h
koIMR.o: /usr/include/sys/select.h /usr/include/selectbits.h
koIMR.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
koIMR.o: /usr/local/kde/include/part_impl.h /usr/local/kde/include/parts.h
koIMR.o: /usr/local/kde/include/kmenubar.h /usr/lib/qt/include/qmenubar.h
koIMR.o: /usr/local/kde/include/ktoolbar.h /usr/lib/qt/include/qbutton.h
koIMR.o: /usr/local/kde/include/kcombo.h /usr/lib/qt/include/qcombo.h
koIMR.o: /usr/local/kde/include/klined.h /usr/lib/qt/include/qlined.h
koScanParts.o: koScanParts.h /usr/lib/qt/include/qstring.h
koScanParts.o: /usr/lib/qt/include/qarray.h /usr/lib/qt/include/qgarray.h
koScanParts.o: /usr/lib/qt/include/qshared.h /usr/lib/qt/include/qglobal.h
koScanParts.o: /usr/lib/qt/include/qgeneric.h /usr/include/string.h
koScanParts.o: /usr/include/features.h /usr/include/sys/cdefs.h
koScanParts.o: /usr/include/gnu/stubs.h
koScanParts.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
koScanParts.o: /usr/lib/qt/include/qstrlist.h /usr/lib/qt/include/qlist.h
koScanParts.o: /usr/lib/qt/include/qglist.h /usr/lib/qt/include/qcollect.h
koScanParts.o: /usr/lib/qt/include/qdstream.h /usr/lib/qt/include/qiodev.h
koScanParts.o: /usr/local/kde/include/op_app.h /usr/lib/qt/include/qapp.h
koScanParts.o: /usr/lib/qt/include/qwidget.h /usr/lib/qt/include/qwindefs.h
koScanParts.o: /usr/lib/qt/include/qobjdefs.h /usr/lib/qt/include/qobject.h
koScanParts.o: /usr/lib/qt/include/qevent.h /usr/lib/qt/include/qrect.h
koScanParts.o: /usr/lib/qt/include/qsize.h /usr/lib/qt/include/qpoint.h
koScanParts.o: /usr/lib/qt/include/qpaintd.h /usr/lib/qt/include/qpalette.h
koScanParts.o: /usr/lib/qt/include/qcolor.h /usr/lib/qt/include/qcursor.h
koScanParts.o: /usr/lib/qt/include/qfont.h /usr/lib/qt/include/qfontmet.h
koScanParts.o: /usr/lib/qt/include/qfontinf.h /usr/local/kde/include/kom.h
koScanParts.o: /usr/local/kde/include/kom_app.h
koScanParts.o: /usr/lib/qt/include/qsocknot.h /usr/lib/qt/include/qtimer.h
koScanParts.o: /usr/lib/qt/include/qintdict.h /usr/lib/qt/include/qgdict.h
koScanParts.o: /usr/local/kde/include/kapp.h /usr/local/kde/include/kconfig.h
koScanParts.o: /usr/local/kde/include/kconfigbase.h
koScanParts.o: /usr/local/kde/include/kconfigdata.h
koScanParts.o: /usr/lib/qt/include/qdict.h /usr/lib/qt/include/qtstream.h
koScanParts.o: /usr/include/stdio.h /usr/include/libio.h
koScanParts.o: /usr/include/_G_config.h /usr/include/gnu/types.h
koScanParts.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stdarg.h
koScanParts.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/va-alpha.h
koScanParts.o: /usr/include/stdio_lim.h /usr/lib/qt/include/qfile.h
koScanParts.o: /usr/local/kde/include/kdebug.h
koScanParts.o: /usr/local/kde/include/klocale.h /usr/local/kde/include/drag.h
koScanParts.o: /usr/lib/qt/include/qpixmap.h /usr/include/X11/X.h
koScanParts.o: /usr/lib/qt/include/qpopmenu.h /usr/lib/qt/include/qtablevw.h
koScanParts.o: /usr/lib/qt/include/qframe.h /usr/lib/qt/include/qmenudta.h
koScanParts.o: /usr/local/kde/include/topwidget_impl.h
koScanParts.o: /usr/local/kde/include/controls.h /opt/kde/idl/kom.h
koScanParts.o: /usr/local/kde/include/component_impl.h
koScanParts.o: /usr/local/kde/include/base_impl.h
koScanParts.o: /usr/local/kde/include/kom_var.h
koScanParts.o: /usr/local/kde/include/control_frame.h
koScanParts.o: /usr/local/kde/include/control_impl.h
koScanParts.o: /usr/local/kde/include/kwm.h /usr/include/X11/Xlib.h
koScanParts.o: /usr/include/sys/types.h /usr/include/time.h
koScanParts.o: /usr/include/endian.h /usr/include/bytesex.h
koScanParts.o: /usr/include/sys/select.h /usr/include/selectbits.h
koScanParts.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
koScanParts.o: /usr/local/kde/include/part_impl.h
koScanParts.o: /usr/local/kde/include/parts.h
koScanParts.o: /usr/local/kde/include/kmenubar.h
koScanParts.o: /usr/lib/qt/include/qmenubar.h
koScanParts.o: /usr/local/kde/include/ktoolbar.h
koScanParts.o: /usr/lib/qt/include/qbutton.h /usr/local/kde/include/kcombo.h
koScanParts.o: /usr/lib/qt/include/qcombo.h /usr/local/kde/include/klined.h
koScanParts.o: /usr/lib/qt/include/qlined.h koIMR.h /usr/include/stdlib.h
koScanParts.o: /usr/include/alloca.h /usr/include/dirent.h
koScanParts.o: /usr/include/direntry.h /usr/include/posix1_lim.h
koScanParts.o: /usr/include/local_lim.h /usr/include/linux/limits.h
koScanParts.o: /usr/include/sys/stat.h /usr/include/statbuf.h
koScanParts.o: /usr/local/kde/include/ksimpleconfig.h
koStream.o: koStream.h /usr/local/kde/include/komlParser.h
koStream.o: /usr/include/string.h /usr/include/features.h
koStream.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
koStream.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
koStream.o: /usr/local/kde/include/torben.h /usr/local/kde/include/komlFeed.h
koStream.o: /usr/local/kde/include/komlWriter.h /usr/lib/qt/include/qrect.h
koStream.o: /usr/lib/qt/include/qsize.h /usr/lib/qt/include/qpoint.h
koStream.o: /usr/lib/qt/include/qwindefs.h /usr/lib/qt/include/qobjdefs.h
koStream.o: /usr/lib/qt/include/qglobal.h /usr/lib/qt/include/qcolor.h
koStream.o: /usr/lib/qt/include/qpen.h /usr/lib/qt/include/qshared.h
koStream.o: /usr/lib/qt/include/qfont.h /usr/lib/qt/include/qstring.h
koStream.o: /usr/lib/qt/include/qarray.h /usr/lib/qt/include/qgarray.h
koStream.o: /usr/lib/qt/include/qgeneric.h /usr/include/stdlib.h
koStream.o: /usr/include/sys/types.h /usr/include/gnu/types.h
koStream.o: /usr/include/time.h /usr/include/endian.h /usr/include/bytesex.h
koStream.o: /usr/include/sys/select.h /usr/include/selectbits.h
koStream.o: /usr/include/alloca.h
