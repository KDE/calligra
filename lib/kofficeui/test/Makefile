# Generated automatically from Makefile.in by configure.

INCDIR		=	-I$(QTDIR)/include -I$(KDEDIR)/include  -I..
CXXFLAGS	=	$(INCDIR) -g 
LDFLAGS		=	-L$(KDEDIR)/lib -L/usr/X11/lib -L$(QTDIR)/lib -L.. -lkdeui -lkdecore -lX11 -lXext -ldl -lqt -lkofficeui -lkofficecore -lmico2.0.5 -lkfile -lkfm -lkom -lop -lkoml
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

test.o: test.h ../koPartSelectDia.h /opt/kde/include/koScanParts.h
test.o: /usr/lib/qt/include/qstring.h /usr/lib/qt/include/qarray.h
test.o: /usr/lib/qt/include/qgarray.h /usr/lib/qt/include/qshared.h
test.o: /usr/lib/qt/include/qglobal.h /usr/lib/qt/include/qgeneric.h
test.o: /usr/include/string.h /usr/include/features.h
test.o: /usr/include/sys/cdefs.h
test.o: /usr/lib/gcc-lib/i486-linux/2.7.2.1/include/stddef.h
test.o: /usr/lib/qt/include/qstrlist.h /usr/lib/qt/include/qlist.h
test.o: /usr/lib/qt/include/qglist.h /usr/lib/qt/include/qcollect.h
test.o: /usr/lib/qt/include/qdstream.h /usr/lib/qt/include/qiodev.h
test.o: /opt/kde/include/op_app.h /usr/lib/qt/include/qapp.h
test.o: /usr/lib/qt/include/qwidget.h /usr/lib/qt/include/qwindefs.h
test.o: /usr/lib/qt/include/qobjdefs.h /usr/lib/qt/include/qobject.h
test.o: /usr/lib/qt/include/qevent.h /usr/lib/qt/include/qrect.h
test.o: /usr/lib/qt/include/qsize.h /usr/lib/qt/include/qpoint.h
test.o: /usr/lib/qt/include/qpaintd.h /usr/lib/qt/include/qpalette.h
test.o: /usr/lib/qt/include/qcolor.h /usr/lib/qt/include/qcursor.h
test.o: /usr/lib/qt/include/qfont.h /usr/lib/qt/include/qfontmet.h
test.o: /usr/lib/qt/include/qfontinf.h /opt/kde/include/kom.h
test.o: /opt/kde/include/kom_app.h /usr/lib/qt/include/qsocknot.h
test.o: /usr/lib/qt/include/qtimer.h /usr/lib/qt/include/qintdict.h
test.o: /usr/lib/qt/include/qgdict.h /opt/kde/include/kapp.h
test.o: /opt/kde/include/kconfig.h /opt/kde/include/kconfigbase.h
test.o: /opt/kde/include/kconfigdata.h /usr/lib/qt/include/qdict.h
test.o: /usr/lib/qt/include/qtstream.h /usr/include/stdio.h
test.o: /usr/include/libio.h /usr/include/_G_config.h
test.o: /usr/lib/qt/include/qfile.h /opt/kde/include/kdebug.h
test.o: /opt/kde/include/klocale.h /opt/kde/include/drag.h
test.o: /usr/lib/qt/include/qpixmap.h /usr/include/X11/X.h
test.o: /usr/lib/qt/include/qpopmenu.h /usr/lib/qt/include/qtablevw.h
test.o: /usr/lib/qt/include/qframe.h /usr/lib/qt/include/qmenudta.h
test.o: /opt/kde/include/topwidget_impl.h /opt/kde/include/controls.h
test.o: /opt/kde/idl/kom.h /opt/kde/include/component_impl.h
test.o: /opt/kde/include/base_impl.h /opt/kde/include/kom_var.h
test.o: /opt/kde/include/control_frame.h /opt/kde/include/control_impl.h
test.o: /opt/kde/include/kwm.h /usr/include/X11/Xlib.h
test.o: /usr/include/sys/types.h /usr/include/linux/types.h
test.o: /usr/include/linux/posix_types.h /usr/include/asm/posix_types.h
test.o: /usr/include/asm/types.h /usr/include/sys/bitypes.h
test.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
test.o: /opt/kde/include/part_impl.h /opt/kde/include/parts.h
test.o: /opt/kde/include/kmenubar.h /usr/lib/qt/include/qmenubar.h
test.o: /opt/kde/include/ktoolbar.h /usr/lib/qt/include/qbutton.h
test.o: /opt/kde/include/kcombo.h /usr/lib/qt/include/qcombo.h
test.o: /opt/kde/include/klined.h /usr/lib/qt/include/qlined.h
test.o: ../koPartSelectDia_data.h /usr/lib/qt/include/qdialog.h
test.o: /usr/lib/qt/include/qlistbox.h /usr/lib/qt/include/qpushbt.h
test.o: ../koAboutDia.h /usr/lib/qt/include/qlabel.h
test.o: /usr/lib/qt/include/qmsgbox.h /usr/lib/qt/include/qlayout.h
test.o: /usr/lib/qt/include/qgmanagr.h /opt/kde/include/kbuttonbox.h
test.o: ../koPageLayoutDia.h /usr/include/stdlib.h /usr/include/errno.h
test.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
test.o: /usr/include/alloca.h /usr/lib/qt/include/qtabdlg.h
test.o: /usr/lib/qt/include/qgrpbox.h /usr/lib/qt/include/qpainter.h
test.o: /usr/lib/qt/include/qregion.h /usr/lib/qt/include/qpen.h
test.o: /usr/lib/qt/include/qbrush.h /usr/lib/qt/include/qpntarry.h
test.o: /usr/lib/qt/include/qwmatrix.h /opt/kde/include/krestrictedline.h
test.o: test.moc /usr/lib/qt/include/qmetaobj.h
test.o: /usr/lib/qt/include/qconnect.h
