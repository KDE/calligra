# Generated automatically from Makefile.in by configure.

INCDIR		=	-I$(QTDIR)/include -I$(KDEDIR)/include -I$(KDEDIR)/idl -I../lib/kofficeui
CXXFLAGS	=	$(INCDIR) -g -fexceptions -O0
LDFLAGS		=	-L$(KDEDIR)/lib -L/usr/X11/lib -L$(QTDIR)/lib -L$../lib/kofficeui -lkdeui -lkdecore -lkfm -lmico2.0.5 -lX11 -lXext -ldl -lkom -lop -ljpeg -lkfile -lqt -lkofficeui -lkofficecore
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

kpresenter_main.o: kpresenter_main.h /opt/kde/include/op_app.h
kpresenter_main.o: /usr/lib/qt/include/qapp.h /usr/lib/qt/include/qwidget.h
kpresenter_main.o: /usr/lib/qt/include/qwindefs.h
kpresenter_main.o: /usr/lib/qt/include/qobjdefs.h
kpresenter_main.o: /usr/lib/qt/include/qglobal.h
kpresenter_main.o: /usr/lib/qt/include/qobject.h
kpresenter_main.o: /usr/lib/qt/include/qstring.h /usr/lib/qt/include/qarray.h
kpresenter_main.o: /usr/lib/qt/include/qgarray.h
kpresenter_main.o: /usr/lib/qt/include/qshared.h
kpresenter_main.o: /usr/lib/qt/include/qgeneric.h /usr/include/string.h
kpresenter_main.o: /usr/include/features.h /usr/include/sys/cdefs.h
kpresenter_main.o: /usr/lib/gcc-lib/i486-linux/2.7.2.1/include/stddef.h
kpresenter_main.o: /usr/lib/qt/include/qevent.h /usr/lib/qt/include/qrect.h
kpresenter_main.o: /usr/lib/qt/include/qsize.h /usr/lib/qt/include/qpoint.h
kpresenter_main.o: /usr/lib/qt/include/qpaintd.h
kpresenter_main.o: /usr/lib/qt/include/qpalette.h
kpresenter_main.o: /usr/lib/qt/include/qcolor.h /usr/lib/qt/include/qcursor.h
kpresenter_main.o: /usr/lib/qt/include/qfont.h /usr/lib/qt/include/qfontmet.h
kpresenter_main.o: /usr/lib/qt/include/qfontinf.h /opt/kde/include/kom.h
kpresenter_main.o: /opt/kde/include/kom_app.h /usr/lib/qt/include/qsocknot.h
kpresenter_main.o: /usr/lib/qt/include/qtimer.h /usr/lib/qt/include/qlist.h
kpresenter_main.o: /usr/lib/qt/include/qglist.h
kpresenter_main.o: /usr/lib/qt/include/qcollect.h
kpresenter_main.o: /usr/lib/qt/include/qintdict.h
kpresenter_main.o: /usr/lib/qt/include/qgdict.h /opt/kde/include/kapp.h
kpresenter_main.o: /opt/kde/include/kconfig.h /opt/kde/include/kconfigbase.h
kpresenter_main.o: /usr/lib/qt/include/qstrlist.h
kpresenter_main.o: /usr/lib/qt/include/qdstream.h
kpresenter_main.o: /usr/lib/qt/include/qiodev.h
kpresenter_main.o: /opt/kde/include/kconfigdata.h /usr/lib/qt/include/qdict.h
kpresenter_main.o: /usr/lib/qt/include/qtstream.h /usr/include/stdio.h
kpresenter_main.o: /usr/include/libio.h /usr/include/_G_config.h
kpresenter_main.o: /usr/lib/qt/include/qfile.h /opt/kde/include/kdebug.h
kpresenter_main.o: /opt/kde/include/klocale.h /opt/kde/include/drag.h
kpresenter_main.o: /usr/lib/qt/include/qpixmap.h /usr/include/X11/X.h
kpresenter_main.o: /usr/lib/qt/include/qpopmenu.h
kpresenter_main.o: /usr/lib/qt/include/qtablevw.h
kpresenter_main.o: /usr/lib/qt/include/qframe.h
kpresenter_main.o: /usr/lib/qt/include/qmenudta.h
kpresenter_main.o: /opt/kde/include/topwidget_impl.h
kpresenter_main.o: /opt/kde/include/controls.h /opt/kde/idl/kom.h
kpresenter_main.o: /opt/kde/include/component_impl.h
kpresenter_main.o: /opt/kde/include/base_impl.h /opt/kde/include/kom_var.h
kpresenter_main.o: /opt/kde/include/control_frame.h
kpresenter_main.o: /opt/kde/include/control_impl.h /opt/kde/include/kwm.h
kpresenter_main.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
kpresenter_main.o: /usr/include/linux/types.h
kpresenter_main.o: /usr/include/linux/posix_types.h
kpresenter_main.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
kpresenter_main.o: /usr/include/sys/bitypes.h /usr/include/X11/Xfuncproto.h
kpresenter_main.o: /usr/include/X11/Xosdefs.h /opt/kde/include/part_impl.h
kpresenter_main.o: /opt/kde/include/parts.h /opt/kde/include/kmenubar.h
kpresenter_main.o: /usr/lib/qt/include/qmenubar.h /opt/kde/include/ktoolbar.h
kpresenter_main.o: /usr/lib/qt/include/qbutton.h /opt/kde/include/kcombo.h
kpresenter_main.o: /usr/lib/qt/include/qcombo.h /opt/kde/include/klined.h
kpresenter_main.o: /usr/lib/qt/include/qlined.h
kpresenter_main.o: /opt/kde/include/part_shell_impl.h
kpresenter_main.o: /opt/kde/include/ktopwidget.h /usr/include/stdlib.h
kpresenter_main.o: /usr/include/errno.h /usr/include/linux/errno.h
kpresenter_main.o: /usr/include/asm/errno.h /usr/include/alloca.h
kpresenter_main.o: /opt/kde/include/kstatusbar.h /usr/lib/qt/include/qlabel.h
kpresenter_main.o: /opt/kde/include/toolbar_impl.h
kpresenter_main.o: /usr/lib/qt/include/qpushbt.h /opt/kde/include/menu_impl.h
kpresenter_main.o: kpresenter_doc.h /opt/kde/include/document_impl.h
kpresenter_main.o: /opt/kde/include/view_impl.h
kpresenter_main.o: /usr/lib/qt/include/qpainter.h
kpresenter_main.o: /usr/lib/qt/include/qregion.h /usr/lib/qt/include/qpen.h
kpresenter_main.o: /usr/lib/qt/include/qbrush.h
kpresenter_main.o: /usr/lib/qt/include/qpntarry.h
kpresenter_main.o: /usr/lib/qt/include/qwmatrix.h
kpresenter_main.o: /opt/kde/include/koPageLayoutDia.h
kpresenter_main.o: /usr/lib/qt/include/qtabdlg.h
kpresenter_main.o: /usr/lib/qt/include/qdialog.h
kpresenter_main.o: /usr/lib/qt/include/qgrpbox.h
kpresenter_main.o: /usr/lib/qt/include/qlayout.h
kpresenter_main.o: /usr/lib/qt/include/qgmanagr.h
kpresenter_main.o: /opt/kde/include/krestrictedline.h kpresenter_view.h
kpresenter_main.o: /opt/kde/include/part_frame_impl.h
kpresenter_main.o: /opt/kde/include/utils.h /usr/lib/qt/include/qmsgbox.h
kpresenter_main.o: /usr/lib/qt/include/qkeycode.h
kpresenter_main.o: /usr/lib/qt/include/qscrbar.h
kpresenter_main.o: /usr/lib/qt/include/qrangect.h
kpresenter_main.o: /usr/lib/qt/include/qdrawutl.h
kpresenter_main.o: /usr/lib/qt/include/qfileinf.h
kpresenter_main.o: /usr/lib/qt/include/qdatetm.h /opt/kde/include/kcolordlg.h
kpresenter_main.o: /opt/kde/include/kselect.h /opt/kde/include/koAboutDia.h
kpresenter_main.o: /opt/kde/include/kbuttonbox.h
kpresenter_main.o: /opt/kde/include/kfontdialog.h
kpresenter_main.o: /usr/lib/qt/include/qbttngrp.h
kpresenter_main.o: /usr/lib/qt/include/qchkbox.h
kpresenter_main.o: /usr/lib/qt/include/qlistbox.h
kpresenter_main.o: /usr/lib/qt/include/qradiobt.h
kpresenter_main.o: /usr/lib/qt/include/qtooltip.h kpresenter.h
kpresenter_main.o: /opt/kde/idl/controls.h /opt/kde/idl/parts.h global.h
kpresenter_main.o: /usr/lib/qt/include/qpicture.h
kpresenter_main.o: /usr/lib/qt/include/qbuffer.h backdia.h qwmf.h afchoose.h
kpresenter_main.o: /opt/kde/include/kiconloaderdialog.h
kpresenter_main.o: /opt/kde/include/kiconloader.h styledia.h ktextobject.h
kpresenter_main.o: graphobj.h atfinterpreter.h /usr/lib/qt/include/qstack.h
kpresenter_main.o: kpresenter_shell.h /opt/kde/include/default_shell_impl.h
kpresenter_main.o: /opt/kde/include/default_shell.h formats.h
kpresenter_main.o: /usr/lib/qt/include/qimage.h kpresenter_main.moc
kpresenter_main.o: /usr/lib/qt/include/qmetaobj.h
kpresenter_main.o: /usr/lib/qt/include/qconnect.h
kpresenter_doc.o: kpresenter_doc.h /opt/kde/include/document_impl.h
kpresenter_doc.o: /opt/kde/include/parts.h /opt/kde/idl/kom.h
kpresenter_doc.o: /opt/kde/include/controls.h
kpresenter_doc.o: /opt/kde/include/component_impl.h /opt/kde/include/kom.h
kpresenter_doc.o: /opt/kde/include/base_impl.h /usr/lib/qt/include/qstring.h
kpresenter_doc.o: /usr/lib/qt/include/qarray.h /usr/lib/qt/include/qgarray.h
kpresenter_doc.o: /usr/lib/qt/include/qshared.h /usr/lib/qt/include/qglobal.h
kpresenter_doc.o: /usr/lib/qt/include/qgeneric.h /usr/include/string.h
kpresenter_doc.o: /usr/include/features.h /usr/include/sys/cdefs.h
kpresenter_doc.o: /usr/lib/gcc-lib/i486-linux/2.7.2.1/include/stddef.h
kpresenter_doc.o: /usr/lib/qt/include/qlist.h /usr/lib/qt/include/qglist.h
kpresenter_doc.o: /usr/lib/qt/include/qcollect.h /usr/lib/qt/include/qdict.h
kpresenter_doc.o: /usr/lib/qt/include/qgdict.h /opt/kde/include/kom_var.h
kpresenter_doc.o: /opt/kde/include/view_impl.h /opt/kde/include/part_impl.h
kpresenter_doc.o: /opt/kde/include/control_impl.h
kpresenter_doc.o: /usr/lib/qt/include/qobject.h
kpresenter_doc.o: /usr/lib/qt/include/qobjdefs.h /usr/lib/qt/include/qevent.h
kpresenter_doc.o: /usr/lib/qt/include/qwindefs.h /usr/lib/qt/include/qrect.h
kpresenter_doc.o: /usr/lib/qt/include/qsize.h /usr/lib/qt/include/qpoint.h
kpresenter_doc.o: /usr/lib/qt/include/qpainter.h
kpresenter_doc.o: /usr/lib/qt/include/qpaintd.h /usr/lib/qt/include/qcolor.h
kpresenter_doc.o: /usr/lib/qt/include/qfontmet.h /usr/lib/qt/include/qfont.h
kpresenter_doc.o: /usr/lib/qt/include/qfontinf.h
kpresenter_doc.o: /usr/lib/qt/include/qregion.h /usr/lib/qt/include/qpen.h
kpresenter_doc.o: /usr/lib/qt/include/qbrush.h /usr/lib/qt/include/qpntarry.h
kpresenter_doc.o: /usr/lib/qt/include/qwmatrix.h
kpresenter_doc.o: /usr/lib/qt/include/qpixmap.h
kpresenter_doc.o: /usr/lib/qt/include/qpopmenu.h
kpresenter_doc.o: /usr/lib/qt/include/qtablevw.h /usr/lib/qt/include/qframe.h
kpresenter_doc.o: /usr/lib/qt/include/qwidget.h
kpresenter_doc.o: /usr/lib/qt/include/qpalette.h
kpresenter_doc.o: /usr/lib/qt/include/qcursor.h
kpresenter_doc.o: /usr/lib/qt/include/qmenudta.h
kpresenter_doc.o: /opt/kde/include/koPageLayoutDia.h /usr/include/stdio.h
kpresenter_doc.o: /usr/include/libio.h /usr/include/_G_config.h
kpresenter_doc.o: /usr/include/stdlib.h /usr/include/errno.h
kpresenter_doc.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
kpresenter_doc.o: /usr/include/alloca.h /usr/lib/qt/include/qtabdlg.h
kpresenter_doc.o: /usr/lib/qt/include/qdialog.h /usr/lib/qt/include/qcombo.h
kpresenter_doc.o: /usr/lib/qt/include/qlabel.h /usr/lib/qt/include/qgrpbox.h
kpresenter_doc.o: /usr/lib/qt/include/qlayout.h
kpresenter_doc.o: /usr/lib/qt/include/qgmanagr.h
kpresenter_doc.o: /usr/lib/qt/include/qintdict.h /usr/lib/qt/include/qlined.h
kpresenter_doc.o: /opt/kde/include/kapp.h /opt/kde/include/kconfig.h
kpresenter_doc.o: /opt/kde/include/kconfigbase.h
kpresenter_doc.o: /usr/lib/qt/include/qstrlist.h
kpresenter_doc.o: /usr/lib/qt/include/qdstream.h /usr/lib/qt/include/qiodev.h
kpresenter_doc.o: /opt/kde/include/kconfigdata.h
kpresenter_doc.o: /usr/lib/qt/include/qtstream.h /usr/lib/qt/include/qfile.h
kpresenter_doc.o: /opt/kde/include/kdebug.h /opt/kde/include/klocale.h
kpresenter_doc.o: /opt/kde/include/drag.h /usr/lib/qt/include/qapp.h
kpresenter_doc.o: /usr/include/X11/X.h /opt/kde/include/krestrictedline.h
kpresenter_doc.o: kpresenter_view.h /opt/kde/include/part_frame_impl.h
kpresenter_doc.o: /opt/kde/include/control_frame.h /opt/kde/include/kwm.h
kpresenter_doc.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
kpresenter_doc.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
kpresenter_doc.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
kpresenter_doc.o: /usr/include/sys/bitypes.h /usr/include/X11/Xfuncproto.h
kpresenter_doc.o: /usr/include/X11/Xosdefs.h /opt/kde/include/menu_impl.h
kpresenter_doc.o: /opt/kde/include/kmenubar.h /usr/lib/qt/include/qmenubar.h
kpresenter_doc.o: /opt/kde/include/ktopwidget.h /opt/kde/include/ktoolbar.h
kpresenter_doc.o: /usr/lib/qt/include/qbutton.h /opt/kde/include/kcombo.h
kpresenter_doc.o: /opt/kde/include/klined.h /opt/kde/include/kstatusbar.h
kpresenter_doc.o: /opt/kde/include/toolbar_impl.h
kpresenter_doc.o: /usr/lib/qt/include/qpushbt.h /opt/kde/include/op_app.h
kpresenter_doc.o: /opt/kde/include/kom_app.h /usr/lib/qt/include/qsocknot.h
kpresenter_doc.o: /usr/lib/qt/include/qtimer.h
kpresenter_doc.o: /opt/kde/include/topwidget_impl.h /opt/kde/include/utils.h
kpresenter_doc.o: /usr/lib/qt/include/qmsgbox.h
kpresenter_doc.o: /usr/lib/qt/include/qkeycode.h
kpresenter_doc.o: /usr/lib/qt/include/qscrbar.h
kpresenter_doc.o: /usr/lib/qt/include/qrangect.h
kpresenter_doc.o: /usr/lib/qt/include/qdrawutl.h
kpresenter_doc.o: /usr/lib/qt/include/qfileinf.h
kpresenter_doc.o: /usr/lib/qt/include/qdatetm.h /opt/kde/include/kcolordlg.h
kpresenter_doc.o: /opt/kde/include/kselect.h /opt/kde/include/koAboutDia.h
kpresenter_doc.o: /opt/kde/include/kbuttonbox.h
kpresenter_doc.o: /opt/kde/include/kfontdialog.h
kpresenter_doc.o: /usr/lib/qt/include/qbttngrp.h
kpresenter_doc.o: /usr/lib/qt/include/qchkbox.h
kpresenter_doc.o: /usr/lib/qt/include/qlistbox.h
kpresenter_doc.o: /usr/lib/qt/include/qradiobt.h
kpresenter_doc.o: /usr/lib/qt/include/qtooltip.h kpresenter.h
kpresenter_doc.o: /opt/kde/idl/controls.h /opt/kde/idl/parts.h
kpresenter_doc.o: kpresenter_main.h /opt/kde/include/part_shell_impl.h
kpresenter_doc.o: kpresenter_shell.h /opt/kde/include/default_shell_impl.h
kpresenter_doc.o: /opt/kde/include/default_shell.h formats.h
kpresenter_doc.o: /usr/lib/qt/include/qimage.h global.h
kpresenter_doc.o: /usr/lib/qt/include/qpicture.h
kpresenter_doc.o: /usr/lib/qt/include/qbuffer.h backdia.h qwmf.h afchoose.h
kpresenter_doc.o: /opt/kde/include/kiconloaderdialog.h
kpresenter_doc.o: /opt/kde/include/kiconloader.h styledia.h ktextobject.h
kpresenter_doc.o: graphobj.h atfinterpreter.h /usr/lib/qt/include/qstack.h
kpresenter_doc.o: kpresenter_doc.moc /usr/lib/qt/include/qmetaobj.h
kpresenter_doc.o: /usr/lib/qt/include/qconnect.h
kpresenter_view.o: /opt/kde/include/kfiledialog.h
kpresenter_view.o: /usr/lib/qt/include/qdialog.h
kpresenter_view.o: /usr/lib/qt/include/qwidget.h
kpresenter_view.o: /usr/lib/qt/include/qwindefs.h
kpresenter_view.o: /usr/lib/qt/include/qobjdefs.h
kpresenter_view.o: /usr/lib/qt/include/qglobal.h
kpresenter_view.o: /usr/lib/qt/include/qobject.h
kpresenter_view.o: /usr/lib/qt/include/qstring.h /usr/lib/qt/include/qarray.h
kpresenter_view.o: /usr/lib/qt/include/qgarray.h
kpresenter_view.o: /usr/lib/qt/include/qshared.h
kpresenter_view.o: /usr/lib/qt/include/qgeneric.h /usr/include/string.h
kpresenter_view.o: /usr/include/features.h /usr/include/sys/cdefs.h
kpresenter_view.o: /usr/lib/gcc-lib/i486-linux/2.7.2.1/include/stddef.h
kpresenter_view.o: /usr/lib/qt/include/qevent.h /usr/lib/qt/include/qrect.h
kpresenter_view.o: /usr/lib/qt/include/qsize.h /usr/lib/qt/include/qpoint.h
kpresenter_view.o: /usr/lib/qt/include/qpaintd.h
kpresenter_view.o: /usr/lib/qt/include/qpalette.h
kpresenter_view.o: /usr/lib/qt/include/qcolor.h /usr/lib/qt/include/qcursor.h
kpresenter_view.o: /usr/lib/qt/include/qfont.h /usr/lib/qt/include/qfontmet.h
kpresenter_view.o: /usr/lib/qt/include/qfontinf.h
kpresenter_view.o: /usr/lib/qt/include/qstack.h /usr/lib/qt/include/qglist.h
kpresenter_view.o: /usr/lib/qt/include/qcollect.h
kpresenter_view.o: /usr/lib/qt/include/qstrlist.h /usr/lib/qt/include/qlist.h
kpresenter_view.o: /usr/lib/qt/include/qdstream.h
kpresenter_view.o: /usr/lib/qt/include/qiodev.h /usr/lib/qt/include/qpixmap.h
kpresenter_view.o: /opt/kde/include/kfileinfo.h /usr/lib/qt/include/qdatetm.h
kpresenter_view.o: /usr/include/time.h /usr/include/sys/time.h
kpresenter_view.o: /usr/include/linux/types.h
kpresenter_view.o: /usr/include/linux/posix_types.h
kpresenter_view.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
kpresenter_view.o: /usr/include/linux/time.h /usr/include/sys/types.h
kpresenter_view.o: /usr/include/sys/bitypes.h kpresenter_view.h
kpresenter_view.o: /usr/include/stdlib.h /usr/include/errno.h
kpresenter_view.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
kpresenter_view.o: /usr/include/alloca.h /opt/kde/include/view_impl.h
kpresenter_view.o: /opt/kde/include/parts.h /opt/kde/idl/kom.h
kpresenter_view.o: /opt/kde/include/controls.h /opt/kde/include/part_impl.h
kpresenter_view.o: /opt/kde/include/control_impl.h
kpresenter_view.o: /opt/kde/include/component_impl.h /opt/kde/include/kom.h
kpresenter_view.o: /opt/kde/include/base_impl.h /usr/lib/qt/include/qdict.h
kpresenter_view.o: /usr/lib/qt/include/qgdict.h /opt/kde/include/kom_var.h
kpresenter_view.o: /opt/kde/include/document_impl.h
kpresenter_view.o: /opt/kde/include/part_frame_impl.h
kpresenter_view.o: /opt/kde/include/control_frame.h /opt/kde/include/kwm.h
kpresenter_view.o: /usr/lib/qt/include/qapp.h /usr/include/X11/Xlib.h
kpresenter_view.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
kpresenter_view.o: /usr/include/X11/Xosdefs.h /opt/kde/include/kapp.h
kpresenter_view.o: /opt/kde/include/kconfig.h /opt/kde/include/kconfigbase.h
kpresenter_view.o: /opt/kde/include/kconfigdata.h
kpresenter_view.o: /usr/lib/qt/include/qtstream.h /usr/include/stdio.h
kpresenter_view.o: /usr/include/libio.h /usr/include/_G_config.h
kpresenter_view.o: /usr/lib/qt/include/qfile.h /opt/kde/include/kdebug.h
kpresenter_view.o: /opt/kde/include/klocale.h /usr/lib/qt/include/qintdict.h
kpresenter_view.o: /opt/kde/include/drag.h /usr/lib/qt/include/qpopmenu.h
kpresenter_view.o: /usr/lib/qt/include/qtablevw.h
kpresenter_view.o: /usr/lib/qt/include/qframe.h
kpresenter_view.o: /usr/lib/qt/include/qmenudta.h
kpresenter_view.o: /opt/kde/include/menu_impl.h /opt/kde/include/kmenubar.h
kpresenter_view.o: /usr/lib/qt/include/qmenubar.h
kpresenter_view.o: /opt/kde/include/ktopwidget.h /opt/kde/include/ktoolbar.h
kpresenter_view.o: /usr/lib/qt/include/qbutton.h /opt/kde/include/kcombo.h
kpresenter_view.o: /usr/lib/qt/include/qcombo.h /opt/kde/include/klined.h
kpresenter_view.o: /usr/lib/qt/include/qlined.h /opt/kde/include/kstatusbar.h
kpresenter_view.o: /usr/lib/qt/include/qlabel.h
kpresenter_view.o: /opt/kde/include/toolbar_impl.h
kpresenter_view.o: /usr/lib/qt/include/qpushbt.h /opt/kde/include/op_app.h
kpresenter_view.o: /opt/kde/include/kom_app.h /usr/lib/qt/include/qsocknot.h
kpresenter_view.o: /usr/lib/qt/include/qtimer.h
kpresenter_view.o: /opt/kde/include/topwidget_impl.h /opt/kde/include/utils.h
kpresenter_view.o: /usr/lib/qt/include/qpainter.h
kpresenter_view.o: /usr/lib/qt/include/qregion.h /usr/lib/qt/include/qpen.h
kpresenter_view.o: /usr/lib/qt/include/qbrush.h
kpresenter_view.o: /usr/lib/qt/include/qpntarry.h
kpresenter_view.o: /usr/lib/qt/include/qwmatrix.h
kpresenter_view.o: /usr/lib/qt/include/qmsgbox.h
kpresenter_view.o: /usr/lib/qt/include/qkeycode.h
kpresenter_view.o: /usr/lib/qt/include/qscrbar.h
kpresenter_view.o: /usr/lib/qt/include/qrangect.h
kpresenter_view.o: /usr/lib/qt/include/qdrawutl.h
kpresenter_view.o: /usr/lib/qt/include/qfileinf.h
kpresenter_view.o: /opt/kde/include/kcolordlg.h /opt/kde/include/kselect.h
kpresenter_view.o: /opt/kde/include/koAboutDia.h
kpresenter_view.o: /usr/lib/qt/include/qlayout.h
kpresenter_view.o: /usr/lib/qt/include/qgmanagr.h
kpresenter_view.o: /opt/kde/include/kbuttonbox.h
kpresenter_view.o: /opt/kde/include/koPageLayoutDia.h
kpresenter_view.o: /usr/lib/qt/include/qtabdlg.h
kpresenter_view.o: /usr/lib/qt/include/qgrpbox.h
kpresenter_view.o: /opt/kde/include/krestrictedline.h
kpresenter_view.o: /opt/kde/include/kfontdialog.h
kpresenter_view.o: /usr/lib/qt/include/qbttngrp.h
kpresenter_view.o: /usr/lib/qt/include/qchkbox.h
kpresenter_view.o: /usr/lib/qt/include/qlistbox.h
kpresenter_view.o: /usr/lib/qt/include/qradiobt.h
kpresenter_view.o: /usr/lib/qt/include/qtooltip.h kpresenter.h
kpresenter_view.o: /opt/kde/idl/controls.h /opt/kde/idl/parts.h
kpresenter_view.o: kpresenter_doc.h global.h /usr/lib/qt/include/qpicture.h
kpresenter_view.o: /usr/lib/qt/include/qbuffer.h qwmf.h graphobj.h
kpresenter_view.o: atfinterpreter.h ktextobject.h kpresenter_main.h
kpresenter_view.o: /opt/kde/include/part_shell_impl.h kpresenter_shell.h
kpresenter_view.o: /opt/kde/include/default_shell_impl.h
kpresenter_view.o: /opt/kde/include/default_shell.h formats.h
kpresenter_view.o: /usr/lib/qt/include/qimage.h backdia.h afchoose.h
kpresenter_view.o: /opt/kde/include/kiconloaderdialog.h
kpresenter_view.o: /opt/kde/include/kiconloader.h styledia.h
kpresenter_view.o: kpresenter_view.moc /usr/lib/qt/include/qmetaobj.h
kpresenter_view.o: /usr/lib/qt/include/qconnect.h page.h
kpresenter.o: kpresenter.h /opt/kde/idl/kom.h /opt/kde/idl/controls.h
kpresenter.o: /opt/kde/idl/parts.h
kpresenter_shell.o: kpresenter_shell.h /opt/kde/include/default_shell_impl.h
kpresenter_shell.o: /opt/kde/include/default_shell.h /opt/kde/idl/kom.h
kpresenter_shell.o: /opt/kde/include/controls.h /opt/kde/include/parts.h
kpresenter_shell.o: /opt/kde/include/part_shell_impl.h
kpresenter_shell.o: /opt/kde/include/ktopwidget.h /usr/include/stdlib.h
kpresenter_shell.o: /usr/include/features.h /usr/include/sys/cdefs.h
kpresenter_shell.o: /usr/lib/gcc-lib/i486-linux/2.7.2.1/include/stddef.h
kpresenter_shell.o: /usr/include/errno.h /usr/include/linux/errno.h
kpresenter_shell.o: /usr/include/asm/errno.h /usr/include/alloca.h
kpresenter_shell.o: /usr/lib/qt/include/qwidget.h
kpresenter_shell.o: /usr/lib/qt/include/qwindefs.h
kpresenter_shell.o: /usr/lib/qt/include/qobjdefs.h
kpresenter_shell.o: /usr/lib/qt/include/qglobal.h
kpresenter_shell.o: /usr/lib/qt/include/qobject.h
kpresenter_shell.o: /usr/lib/qt/include/qstring.h
kpresenter_shell.o: /usr/lib/qt/include/qarray.h
kpresenter_shell.o: /usr/lib/qt/include/qgarray.h
kpresenter_shell.o: /usr/lib/qt/include/qshared.h
kpresenter_shell.o: /usr/lib/qt/include/qgeneric.h /usr/include/string.h
kpresenter_shell.o: /usr/lib/qt/include/qevent.h /usr/lib/qt/include/qrect.h
kpresenter_shell.o: /usr/lib/qt/include/qsize.h /usr/lib/qt/include/qpoint.h
kpresenter_shell.o: /usr/lib/qt/include/qpaintd.h
kpresenter_shell.o: /usr/lib/qt/include/qpalette.h
kpresenter_shell.o: /usr/lib/qt/include/qcolor.h
kpresenter_shell.o: /usr/lib/qt/include/qcursor.h /usr/lib/qt/include/qfont.h
kpresenter_shell.o: /usr/lib/qt/include/qfontmet.h
kpresenter_shell.o: /usr/lib/qt/include/qfontinf.h
kpresenter_shell.o: /usr/lib/qt/include/qlist.h /usr/lib/qt/include/qglist.h
kpresenter_shell.o: /usr/lib/qt/include/qcollect.h
kpresenter_shell.o: /opt/kde/include/ktoolbar.h /usr/lib/qt/include/qframe.h
kpresenter_shell.o: /usr/lib/qt/include/qpixmap.h
kpresenter_shell.o: /usr/lib/qt/include/qpopmenu.h
kpresenter_shell.o: /usr/lib/qt/include/qtablevw.h
kpresenter_shell.o: /usr/lib/qt/include/qmenudta.h
kpresenter_shell.o: /usr/lib/qt/include/qbutton.h /opt/kde/include/kcombo.h
kpresenter_shell.o: /usr/lib/qt/include/qcombo.h /opt/kde/include/klined.h
kpresenter_shell.o: /usr/lib/qt/include/qlined.h /opt/kde/include/kmenubar.h
kpresenter_shell.o: /usr/lib/qt/include/qmenubar.h
kpresenter_shell.o: /opt/kde/include/kstatusbar.h
kpresenter_shell.o: /usr/lib/qt/include/qlabel.h /opt/kde/include/kconfig.h
kpresenter_shell.o: /opt/kde/include/kconfigbase.h
kpresenter_shell.o: /usr/lib/qt/include/qstrlist.h
kpresenter_shell.o: /usr/lib/qt/include/qdstream.h
kpresenter_shell.o: /usr/lib/qt/include/qiodev.h
kpresenter_shell.o: /opt/kde/include/kconfigdata.h
kpresenter_shell.o: /usr/lib/qt/include/qdict.h /usr/lib/qt/include/qgdict.h
kpresenter_shell.o: /usr/lib/qt/include/qtstream.h /usr/include/stdio.h
kpresenter_shell.o: /usr/include/libio.h /usr/include/_G_config.h
kpresenter_shell.o: /usr/lib/qt/include/qfile.h /opt/kde/include/kdebug.h
kpresenter_shell.o: /opt/kde/include/part_impl.h
kpresenter_shell.o: /opt/kde/include/control_impl.h
kpresenter_shell.o: /opt/kde/include/component_impl.h /opt/kde/include/kom.h
kpresenter_shell.o: /opt/kde/include/base_impl.h /opt/kde/include/kom_var.h
kpresenter_shell.o: /opt/kde/include/toolbar_impl.h
kpresenter_shell.o: /usr/lib/qt/include/qpushbt.h
kpresenter_shell.o: /usr/lib/qt/include/qintdict.h
kpresenter_shell.o: /opt/kde/include/menu_impl.h
kpresenter_shell.o: /opt/kde/include/control_frame.h /opt/kde/include/kwm.h
kpresenter_shell.o: /usr/lib/qt/include/qapp.h /usr/include/X11/Xlib.h
kpresenter_shell.o: /usr/include/sys/types.h /usr/include/linux/types.h
kpresenter_shell.o: /usr/include/linux/posix_types.h
kpresenter_shell.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
kpresenter_shell.o: /usr/include/sys/bitypes.h /usr/include/X11/X.h
kpresenter_shell.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
kpresenter_shell.o: /opt/kde/include/kapp.h /opt/kde/include/klocale.h
kpresenter_shell.o: /opt/kde/include/drag.h /opt/kde/include/topwidget_impl.h
kpresenter_shell.o: /opt/kde/include/document_impl.h
kpresenter_shell.o: /opt/kde/include/view_impl.h kpresenter.h
kpresenter_shell.o: /opt/kde/idl/controls.h /opt/kde/idl/parts.h
kpresenter_shell.o: kpresenter_doc.h /usr/lib/qt/include/qpainter.h
kpresenter_shell.o: /usr/lib/qt/include/qregion.h /usr/lib/qt/include/qpen.h
kpresenter_shell.o: /usr/lib/qt/include/qbrush.h
kpresenter_shell.o: /usr/lib/qt/include/qpntarry.h
kpresenter_shell.o: /usr/lib/qt/include/qwmatrix.h
kpresenter_shell.o: /opt/kde/include/koPageLayoutDia.h
kpresenter_shell.o: /usr/lib/qt/include/qtabdlg.h
kpresenter_shell.o: /usr/lib/qt/include/qdialog.h
kpresenter_shell.o: /usr/lib/qt/include/qgrpbox.h
kpresenter_shell.o: /usr/lib/qt/include/qlayout.h
kpresenter_shell.o: /usr/lib/qt/include/qgmanagr.h
kpresenter_shell.o: /opt/kde/include/krestrictedline.h kpresenter_view.h
kpresenter_shell.o: /opt/kde/include/part_frame_impl.h
kpresenter_shell.o: /opt/kde/include/op_app.h /opt/kde/include/kom_app.h
kpresenter_shell.o: /usr/lib/qt/include/qsocknot.h
kpresenter_shell.o: /usr/lib/qt/include/qtimer.h /opt/kde/include/utils.h
kpresenter_shell.o: /usr/lib/qt/include/qmsgbox.h
kpresenter_shell.o: /usr/lib/qt/include/qkeycode.h
kpresenter_shell.o: /usr/lib/qt/include/qscrbar.h
kpresenter_shell.o: /usr/lib/qt/include/qrangect.h
kpresenter_shell.o: /usr/lib/qt/include/qdrawutl.h
kpresenter_shell.o: /usr/lib/qt/include/qfileinf.h
kpresenter_shell.o: /usr/lib/qt/include/qdatetm.h
kpresenter_shell.o: /opt/kde/include/kcolordlg.h /opt/kde/include/kselect.h
kpresenter_shell.o: /opt/kde/include/koAboutDia.h
kpresenter_shell.o: /opt/kde/include/kbuttonbox.h
kpresenter_shell.o: /opt/kde/include/kfontdialog.h
kpresenter_shell.o: /usr/lib/qt/include/qbttngrp.h
kpresenter_shell.o: /usr/lib/qt/include/qchkbox.h
kpresenter_shell.o: /usr/lib/qt/include/qlistbox.h
kpresenter_shell.o: /usr/lib/qt/include/qradiobt.h
kpresenter_shell.o: /usr/lib/qt/include/qtooltip.h kpresenter_main.h
kpresenter_shell.o: formats.h /usr/lib/qt/include/qimage.h global.h
kpresenter_shell.o: /usr/lib/qt/include/qpicture.h
kpresenter_shell.o: /usr/lib/qt/include/qbuffer.h backdia.h qwmf.h afchoose.h
kpresenter_shell.o: /opt/kde/include/kiconloaderdialog.h
kpresenter_shell.o: /opt/kde/include/kiconloader.h styledia.h ktextobject.h
kpresenter_shell.o: graphobj.h atfinterpreter.h /usr/lib/qt/include/qstack.h
kpresenter_shell.o: kpresenter_shell.moc /usr/lib/qt/include/qmetaobj.h
kpresenter_shell.o: /usr/lib/qt/include/qconnect.h
graphobj.o: graphobj.h /usr/lib/qt/include/qwidget.h
graphobj.o: /usr/lib/qt/include/qwindefs.h /usr/lib/qt/include/qobjdefs.h
graphobj.o: /usr/lib/qt/include/qglobal.h /usr/lib/qt/include/qobject.h
graphobj.o: /usr/lib/qt/include/qstring.h /usr/lib/qt/include/qarray.h
graphobj.o: /usr/lib/qt/include/qgarray.h /usr/lib/qt/include/qshared.h
graphobj.o: /usr/lib/qt/include/qgeneric.h /usr/include/string.h
graphobj.o: /usr/include/features.h /usr/include/sys/cdefs.h
graphobj.o: /usr/lib/gcc-lib/i486-linux/2.7.2.1/include/stddef.h
graphobj.o: /usr/lib/qt/include/qevent.h /usr/lib/qt/include/qrect.h
graphobj.o: /usr/lib/qt/include/qsize.h /usr/lib/qt/include/qpoint.h
graphobj.o: /usr/lib/qt/include/qpaintd.h /usr/lib/qt/include/qpalette.h
graphobj.o: /usr/lib/qt/include/qcolor.h /usr/lib/qt/include/qcursor.h
graphobj.o: /usr/lib/qt/include/qfont.h /usr/lib/qt/include/qfontmet.h
graphobj.o: /usr/lib/qt/include/qfontinf.h /usr/lib/qt/include/qpainter.h
graphobj.o: /usr/lib/qt/include/qregion.h /usr/lib/qt/include/qpen.h
graphobj.o: /usr/lib/qt/include/qbrush.h /usr/lib/qt/include/qpntarry.h
graphobj.o: /usr/lib/qt/include/qwmatrix.h /usr/lib/qt/include/qpicture.h
graphobj.o: /usr/lib/qt/include/qbuffer.h /usr/lib/qt/include/qiodev.h
graphobj.o: /usr/lib/qt/include/qpixmap.h /usr/lib/qt/include/qlist.h
graphobj.o: /usr/lib/qt/include/qglist.h /usr/lib/qt/include/qcollect.h
graphobj.o: /usr/lib/qt/include/qfileinf.h /usr/lib/qt/include/qfile.h
graphobj.o: /usr/include/stdio.h /usr/include/libio.h
graphobj.o: /usr/include/_G_config.h /usr/lib/qt/include/qdatetm.h
graphobj.o: atfinterpreter.h /usr/lib/qt/include/qstack.h
graphobj.o: /usr/lib/qt/include/qstrlist.h /usr/lib/qt/include/qdstream.h
graphobj.o: qwmf.h global.h graphobj.moc /usr/lib/qt/include/qmetaobj.h
graphobj.o: /usr/lib/qt/include/qconnect.h
atfinterpreter.o: atfinterpreter.h /usr/lib/qt/include/qobject.h
atfinterpreter.o: /usr/lib/qt/include/qobjdefs.h
atfinterpreter.o: /usr/lib/qt/include/qglobal.h /usr/lib/qt/include/qstring.h
atfinterpreter.o: /usr/lib/qt/include/qarray.h /usr/lib/qt/include/qgarray.h
atfinterpreter.o: /usr/lib/qt/include/qshared.h
atfinterpreter.o: /usr/lib/qt/include/qgeneric.h /usr/include/string.h
atfinterpreter.o: /usr/include/features.h /usr/include/sys/cdefs.h
atfinterpreter.o: /usr/lib/gcc-lib/i486-linux/2.7.2.1/include/stddef.h
atfinterpreter.o: /usr/lib/qt/include/qevent.h /usr/lib/qt/include/qwindefs.h
atfinterpreter.o: /usr/lib/qt/include/qrect.h /usr/lib/qt/include/qsize.h
atfinterpreter.o: /usr/lib/qt/include/qpoint.h /usr/lib/qt/include/qstack.h
atfinterpreter.o: /usr/lib/qt/include/qglist.h /usr/lib/qt/include/qcollect.h
atfinterpreter.o: /usr/lib/qt/include/qpntarry.h /usr/lib/qt/include/qlist.h
atfinterpreter.o: /usr/lib/qt/include/qfile.h /usr/lib/qt/include/qiodev.h
atfinterpreter.o: /usr/include/stdio.h /usr/include/libio.h
atfinterpreter.o: /usr/include/_G_config.h /usr/lib/qt/include/qstrlist.h
atfinterpreter.o: /usr/lib/qt/include/qdstream.h atfinterpreter.moc
atfinterpreter.o: /usr/lib/qt/include/qmetaobj.h
atfinterpreter.o: /usr/lib/qt/include/qconnect.h
backdia.o: /opt/kde/include/kfiledialog.h /usr/lib/qt/include/qdialog.h
backdia.o: /usr/lib/qt/include/qwidget.h /usr/lib/qt/include/qwindefs.h
backdia.o: /usr/lib/qt/include/qobjdefs.h /usr/lib/qt/include/qglobal.h
backdia.o: /usr/lib/qt/include/qobject.h /usr/lib/qt/include/qstring.h
backdia.o: /usr/lib/qt/include/qarray.h /usr/lib/qt/include/qgarray.h
backdia.o: /usr/lib/qt/include/qshared.h /usr/lib/qt/include/qgeneric.h
backdia.o: /usr/include/string.h /usr/include/features.h
backdia.o: /usr/include/sys/cdefs.h
backdia.o: /usr/lib/gcc-lib/i486-linux/2.7.2.1/include/stddef.h
backdia.o: /usr/lib/qt/include/qevent.h /usr/lib/qt/include/qrect.h
backdia.o: /usr/lib/qt/include/qsize.h /usr/lib/qt/include/qpoint.h
backdia.o: /usr/lib/qt/include/qpaintd.h /usr/lib/qt/include/qpalette.h
backdia.o: /usr/lib/qt/include/qcolor.h /usr/lib/qt/include/qcursor.h
backdia.o: /usr/lib/qt/include/qfont.h /usr/lib/qt/include/qfontmet.h
backdia.o: /usr/lib/qt/include/qfontinf.h /usr/lib/qt/include/qstack.h
backdia.o: /usr/lib/qt/include/qglist.h /usr/lib/qt/include/qcollect.h
backdia.o: /usr/lib/qt/include/qstrlist.h /usr/lib/qt/include/qlist.h
backdia.o: /usr/lib/qt/include/qdstream.h /usr/lib/qt/include/qiodev.h
backdia.o: /usr/lib/qt/include/qpixmap.h /opt/kde/include/kfileinfo.h
backdia.o: /usr/lib/qt/include/qdatetm.h /usr/include/time.h
backdia.o: /usr/include/sys/time.h /usr/include/linux/types.h
backdia.o: /usr/include/linux/posix_types.h /usr/include/asm/posix_types.h
backdia.o: /usr/include/asm/types.h /usr/include/linux/time.h
backdia.o: /usr/include/sys/types.h /usr/include/sys/bitypes.h backdia.h
backdia.o: /usr/include/stdlib.h /usr/include/errno.h
backdia.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
backdia.o: /usr/include/alloca.h /usr/lib/qt/include/qlabel.h
backdia.o: /usr/lib/qt/include/qframe.h /usr/lib/qt/include/qgrpbox.h
backdia.o: /usr/lib/qt/include/qradiobt.h /usr/lib/qt/include/qbutton.h
backdia.o: /usr/lib/qt/include/qpushbt.h /usr/lib/qt/include/qbttngrp.h
backdia.o: /usr/lib/qt/include/qwmatrix.h /usr/lib/qt/include/qpntarry.h
backdia.o: /usr/lib/qt/include/qapp.h /usr/lib/qt/include/qpicture.h
backdia.o: /usr/lib/qt/include/qbuffer.h /usr/lib/qt/include/qpainter.h
backdia.o: /usr/lib/qt/include/qregion.h /usr/lib/qt/include/qpen.h
backdia.o: /usr/lib/qt/include/qbrush.h /opt/kde/include/kcolordlg.h
backdia.o: /usr/lib/qt/include/qtablevw.h /usr/lib/qt/include/qrangect.h
backdia.o: /usr/lib/qt/include/qlined.h /usr/lib/qt/include/qcombo.h
backdia.o: /opt/kde/include/kselect.h qwmf.h global.h backdia.moc
backdia.o: /usr/lib/qt/include/qmetaobj.h /usr/lib/qt/include/qconnect.h
page.o: page.h kpresenter_view.h /usr/include/stdlib.h
page.o: /usr/include/features.h /usr/include/sys/cdefs.h
page.o: /usr/lib/gcc-lib/i486-linux/2.7.2.1/include/stddef.h
page.o: /usr/include/errno.h /usr/include/linux/errno.h
page.o: /usr/include/asm/errno.h /usr/include/alloca.h
page.o: /opt/kde/include/view_impl.h /opt/kde/include/parts.h
page.o: /opt/kde/idl/kom.h /opt/kde/include/controls.h
page.o: /opt/kde/include/part_impl.h /opt/kde/include/control_impl.h
page.o: /opt/kde/include/component_impl.h /opt/kde/include/kom.h
page.o: /opt/kde/include/base_impl.h /usr/lib/qt/include/qstring.h
page.o: /usr/lib/qt/include/qarray.h /usr/lib/qt/include/qgarray.h
page.o: /usr/lib/qt/include/qshared.h /usr/lib/qt/include/qglobal.h
page.o: /usr/lib/qt/include/qgeneric.h /usr/include/string.h
page.o: /usr/lib/qt/include/qlist.h /usr/lib/qt/include/qglist.h
page.o: /usr/lib/qt/include/qcollect.h /usr/lib/qt/include/qdict.h
page.o: /usr/lib/qt/include/qgdict.h /opt/kde/include/kom_var.h
page.o: /opt/kde/include/document_impl.h /opt/kde/include/part_frame_impl.h
page.o: /opt/kde/include/control_frame.h /usr/lib/qt/include/qwidget.h
page.o: /usr/lib/qt/include/qwindefs.h /usr/lib/qt/include/qobjdefs.h
page.o: /usr/lib/qt/include/qobject.h /usr/lib/qt/include/qevent.h
page.o: /usr/lib/qt/include/qrect.h /usr/lib/qt/include/qsize.h
page.o: /usr/lib/qt/include/qpoint.h /usr/lib/qt/include/qpaintd.h
page.o: /usr/lib/qt/include/qpalette.h /usr/lib/qt/include/qcolor.h
page.o: /usr/lib/qt/include/qcursor.h /usr/lib/qt/include/qfont.h
page.o: /usr/lib/qt/include/qfontmet.h /usr/lib/qt/include/qfontinf.h
page.o: /opt/kde/include/kwm.h /usr/lib/qt/include/qapp.h
page.o: /usr/lib/qt/include/qpixmap.h /usr/include/X11/Xlib.h
page.o: /usr/include/sys/types.h /usr/include/linux/types.h
page.o: /usr/include/linux/posix_types.h /usr/include/asm/posix_types.h
page.o: /usr/include/asm/types.h /usr/include/sys/bitypes.h
page.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
page.o: /usr/include/X11/Xosdefs.h /opt/kde/include/kapp.h
page.o: /opt/kde/include/kconfig.h /opt/kde/include/kconfigbase.h
page.o: /usr/lib/qt/include/qstrlist.h /usr/lib/qt/include/qdstream.h
page.o: /usr/lib/qt/include/qiodev.h /opt/kde/include/kconfigdata.h
page.o: /usr/lib/qt/include/qtstream.h /usr/include/stdio.h
page.o: /usr/include/libio.h /usr/include/_G_config.h
page.o: /usr/lib/qt/include/qfile.h /opt/kde/include/kdebug.h
page.o: /opt/kde/include/klocale.h /usr/lib/qt/include/qintdict.h
page.o: /opt/kde/include/drag.h /usr/lib/qt/include/qpopmenu.h
page.o: /usr/lib/qt/include/qtablevw.h /usr/lib/qt/include/qframe.h
page.o: /usr/lib/qt/include/qmenudta.h /opt/kde/include/menu_impl.h
page.o: /opt/kde/include/kmenubar.h /usr/lib/qt/include/qmenubar.h
page.o: /opt/kde/include/ktopwidget.h /opt/kde/include/ktoolbar.h
page.o: /usr/lib/qt/include/qbutton.h /opt/kde/include/kcombo.h
page.o: /usr/lib/qt/include/qcombo.h /opt/kde/include/klined.h
page.o: /usr/lib/qt/include/qlined.h /opt/kde/include/kstatusbar.h
page.o: /usr/lib/qt/include/qlabel.h /opt/kde/include/toolbar_impl.h
page.o: /usr/lib/qt/include/qpushbt.h /opt/kde/include/op_app.h
page.o: /opt/kde/include/kom_app.h /usr/lib/qt/include/qsocknot.h
page.o: /usr/lib/qt/include/qtimer.h /opt/kde/include/topwidget_impl.h
page.o: /opt/kde/include/utils.h /usr/lib/qt/include/qpainter.h
page.o: /usr/lib/qt/include/qregion.h /usr/lib/qt/include/qpen.h
page.o: /usr/lib/qt/include/qbrush.h /usr/lib/qt/include/qpntarry.h
page.o: /usr/lib/qt/include/qwmatrix.h /usr/lib/qt/include/qmsgbox.h
page.o: /usr/lib/qt/include/qdialog.h /usr/lib/qt/include/qkeycode.h
page.o: /usr/lib/qt/include/qscrbar.h /usr/lib/qt/include/qrangect.h
page.o: /usr/lib/qt/include/qdrawutl.h /usr/lib/qt/include/qfileinf.h
page.o: /usr/lib/qt/include/qdatetm.h /opt/kde/include/kcolordlg.h
page.o: /opt/kde/include/kselect.h /opt/kde/include/koAboutDia.h
page.o: /usr/lib/qt/include/qlayout.h /usr/lib/qt/include/qgmanagr.h
page.o: /opt/kde/include/kbuttonbox.h /opt/kde/include/koPageLayoutDia.h
page.o: /usr/lib/qt/include/qtabdlg.h /usr/lib/qt/include/qgrpbox.h
page.o: /opt/kde/include/krestrictedline.h /opt/kde/include/kfontdialog.h
page.o: /usr/lib/qt/include/qbttngrp.h /usr/lib/qt/include/qchkbox.h
page.o: /usr/lib/qt/include/qlistbox.h /usr/lib/qt/include/qradiobt.h
page.o: /usr/lib/qt/include/qtooltip.h kpresenter.h /opt/kde/idl/controls.h
page.o: /opt/kde/idl/parts.h kpresenter_doc.h global.h
page.o: /usr/lib/qt/include/qpicture.h /usr/lib/qt/include/qbuffer.h qwmf.h
page.o: graphobj.h atfinterpreter.h /usr/lib/qt/include/qstack.h
page.o: ktextobject.h kpresenter_main.h /opt/kde/include/part_shell_impl.h
page.o: kpresenter_shell.h /opt/kde/include/default_shell_impl.h
page.o: /opt/kde/include/default_shell.h formats.h
page.o: /usr/lib/qt/include/qimage.h backdia.h afchoose.h
page.o: /opt/kde/include/kiconloaderdialog.h /opt/kde/include/kiconloader.h
page.o: styledia.h page.moc /usr/lib/qt/include/qmetaobj.h
page.o: /usr/lib/qt/include/qconnect.h
ktextobject.o: ktextobject.h /usr/lib/qt/include/qkeycode.h
ktextobject.o: /usr/lib/qt/include/qglobal.h /usr/lib/qt/include/qtablevw.h
ktextobject.o: /usr/lib/qt/include/qframe.h /usr/lib/qt/include/qwidget.h
ktextobject.o: /usr/lib/qt/include/qwindefs.h /usr/lib/qt/include/qobjdefs.h
ktextobject.o: /usr/lib/qt/include/qobject.h /usr/lib/qt/include/qstring.h
ktextobject.o: /usr/lib/qt/include/qarray.h /usr/lib/qt/include/qgarray.h
ktextobject.o: /usr/lib/qt/include/qshared.h /usr/lib/qt/include/qgeneric.h
ktextobject.o: /usr/include/string.h /usr/include/features.h
ktextobject.o: /usr/include/sys/cdefs.h
ktextobject.o: /usr/lib/gcc-lib/i486-linux/2.7.2.1/include/stddef.h
ktextobject.o: /usr/lib/qt/include/qevent.h /usr/lib/qt/include/qrect.h
ktextobject.o: /usr/lib/qt/include/qsize.h /usr/lib/qt/include/qpoint.h
ktextobject.o: /usr/lib/qt/include/qpaintd.h /usr/lib/qt/include/qpalette.h
ktextobject.o: /usr/lib/qt/include/qcolor.h /usr/lib/qt/include/qcursor.h
ktextobject.o: /usr/lib/qt/include/qfont.h /usr/lib/qt/include/qfontmet.h
ktextobject.o: /usr/lib/qt/include/qfontinf.h /usr/lib/qt/include/qpainter.h
ktextobject.o: /usr/lib/qt/include/qregion.h /usr/lib/qt/include/qpen.h
ktextobject.o: /usr/lib/qt/include/qbrush.h /usr/lib/qt/include/qpntarry.h
ktextobject.o: /usr/lib/qt/include/qwmatrix.h /usr/lib/qt/include/qlist.h
ktextobject.o: /usr/lib/qt/include/qglist.h /usr/lib/qt/include/qcollect.h
ktextobject.o: /usr/lib/qt/include/qpicture.h /usr/lib/qt/include/qbuffer.h
ktextobject.o: /usr/lib/qt/include/qiodev.h ktextobject.moc
ktextobject.o: /usr/lib/qt/include/qmetaobj.h /usr/lib/qt/include/qconnect.h
afchoose.o: afchoose.h /usr/lib/qt/include/qtabdlg.h
afchoose.o: /usr/lib/qt/include/qdialog.h /usr/lib/qt/include/qwidget.h
afchoose.o: /usr/lib/qt/include/qwindefs.h /usr/lib/qt/include/qobjdefs.h
afchoose.o: /usr/lib/qt/include/qglobal.h /usr/lib/qt/include/qobject.h
afchoose.o: /usr/lib/qt/include/qstring.h /usr/lib/qt/include/qarray.h
afchoose.o: /usr/lib/qt/include/qgarray.h /usr/lib/qt/include/qshared.h
afchoose.o: /usr/lib/qt/include/qgeneric.h /usr/include/string.h
afchoose.o: /usr/include/features.h /usr/include/sys/cdefs.h
afchoose.o: /usr/lib/gcc-lib/i486-linux/2.7.2.1/include/stddef.h
afchoose.o: /usr/lib/qt/include/qevent.h /usr/lib/qt/include/qrect.h
afchoose.o: /usr/lib/qt/include/qsize.h /usr/lib/qt/include/qpoint.h
afchoose.o: /usr/lib/qt/include/qpaintd.h /usr/lib/qt/include/qpalette.h
afchoose.o: /usr/lib/qt/include/qcolor.h /usr/lib/qt/include/qcursor.h
afchoose.o: /usr/lib/qt/include/qfont.h /usr/lib/qt/include/qfontmet.h
afchoose.o: /usr/lib/qt/include/qfontinf.h /usr/lib/qt/include/qlabel.h
afchoose.o: /usr/lib/qt/include/qframe.h /usr/lib/qt/include/qpushbt.h
afchoose.o: /usr/lib/qt/include/qbutton.h /usr/lib/qt/include/qlistbox.h
afchoose.o: /usr/lib/qt/include/qtablevw.h /usr/lib/qt/include/qpixmap.h
afchoose.o: /usr/lib/qt/include/qlist.h /usr/lib/qt/include/qglist.h
afchoose.o: /usr/lib/qt/include/qcollect.h /usr/lib/qt/include/qfileinf.h
afchoose.o: /usr/lib/qt/include/qfile.h /usr/lib/qt/include/qiodev.h
afchoose.o: /usr/include/stdio.h /usr/include/libio.h
afchoose.o: /usr/include/_G_config.h /usr/lib/qt/include/qdatetm.h
afchoose.o: /opt/kde/include/kiconloaderdialog.h /usr/lib/qt/include/qapp.h
afchoose.o: /usr/lib/qt/include/qstrlist.h /usr/lib/qt/include/qdstream.h
afchoose.o: /usr/lib/qt/include/qlined.h /usr/lib/qt/include/qcombo.h
afchoose.o: /opt/kde/include/kapp.h /opt/kde/include/kconfig.h
afchoose.o: /opt/kde/include/kconfigbase.h /opt/kde/include/kconfigdata.h
afchoose.o: /usr/lib/qt/include/qdict.h /usr/lib/qt/include/qgdict.h
afchoose.o: /usr/lib/qt/include/qtstream.h /opt/kde/include/kdebug.h
afchoose.o: /opt/kde/include/klocale.h /usr/lib/qt/include/qintdict.h
afchoose.o: /opt/kde/include/drag.h /usr/include/X11/X.h
afchoose.o: /usr/lib/qt/include/qpopmenu.h /usr/lib/qt/include/qmenudta.h
afchoose.o: /opt/kde/include/kiconloader.h afchoose.moc
afchoose.o: /usr/lib/qt/include/qmetaobj.h /usr/lib/qt/include/qconnect.h
styledia.o: styledia.h /usr/include/stdlib.h /usr/include/features.h
styledia.o: /usr/include/sys/cdefs.h
styledia.o: /usr/lib/gcc-lib/i486-linux/2.7.2.1/include/stddef.h
styledia.o: /usr/include/errno.h /usr/include/linux/errno.h
styledia.o: /usr/include/asm/errno.h /usr/include/alloca.h
styledia.o: /usr/lib/qt/include/qdialog.h /usr/lib/qt/include/qwidget.h
styledia.o: /usr/lib/qt/include/qwindefs.h /usr/lib/qt/include/qobjdefs.h
styledia.o: /usr/lib/qt/include/qglobal.h /usr/lib/qt/include/qobject.h
styledia.o: /usr/lib/qt/include/qstring.h /usr/lib/qt/include/qarray.h
styledia.o: /usr/lib/qt/include/qgarray.h /usr/lib/qt/include/qshared.h
styledia.o: /usr/lib/qt/include/qgeneric.h /usr/include/string.h
styledia.o: /usr/lib/qt/include/qevent.h /usr/lib/qt/include/qrect.h
styledia.o: /usr/lib/qt/include/qsize.h /usr/lib/qt/include/qpoint.h
styledia.o: /usr/lib/qt/include/qpaintd.h /usr/lib/qt/include/qpalette.h
styledia.o: /usr/lib/qt/include/qcolor.h /usr/lib/qt/include/qcursor.h
styledia.o: /usr/lib/qt/include/qfont.h /usr/lib/qt/include/qfontmet.h
styledia.o: /usr/lib/qt/include/qfontinf.h /usr/lib/qt/include/qlabel.h
styledia.o: /usr/lib/qt/include/qframe.h /usr/lib/qt/include/qpushbt.h
styledia.o: /usr/lib/qt/include/qbutton.h /usr/lib/qt/include/qbttngrp.h
styledia.o: /usr/lib/qt/include/qgrpbox.h /usr/lib/qt/include/qcombo.h
styledia.o: /usr/lib/qt/include/qpen.h /usr/lib/qt/include/qbrush.h
styledia.o: /usr/lib/qt/include/qpainter.h /usr/lib/qt/include/qregion.h
styledia.o: /usr/lib/qt/include/qpntarry.h /usr/lib/qt/include/qwmatrix.h
styledia.o: /opt/kde/include/kcolordlg.h /usr/lib/qt/include/qtablevw.h
styledia.o: /usr/lib/qt/include/qrangect.h /usr/lib/qt/include/qlined.h
styledia.o: /usr/lib/qt/include/qpixmap.h /opt/kde/include/kselect.h
styledia.o: styledia.moc /usr/lib/qt/include/qmetaobj.h
styledia.o: /usr/lib/qt/include/qconnect.h
jpeg.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
jpeg.o: /usr/include/assert.h /usr/include/features.h
jpeg.o: /usr/include/sys/cdefs.h /usr/lib/qt/include/qimage.h
jpeg.o: /usr/lib/qt/include/qpixmap.h /usr/lib/qt/include/qpaintd.h
jpeg.o: /usr/lib/qt/include/qwindefs.h /usr/lib/qt/include/qobjdefs.h
jpeg.o: /usr/lib/qt/include/qglobal.h /usr/lib/qt/include/qrect.h
jpeg.o: /usr/lib/qt/include/qsize.h /usr/lib/qt/include/qpoint.h
jpeg.o: /usr/lib/qt/include/qcolor.h /usr/lib/qt/include/qshared.h
jpeg.o: /usr/lib/qt/include/qstrlist.h /usr/lib/qt/include/qstring.h
jpeg.o: /usr/lib/qt/include/qarray.h /usr/lib/qt/include/qgarray.h
jpeg.o: /usr/lib/qt/include/qgeneric.h /usr/include/string.h
jpeg.o: /usr/lib/gcc-lib/i486-linux/2.7.2.1/include/stddef.h
jpeg.o: /usr/lib/qt/include/qlist.h /usr/lib/qt/include/qglist.h
jpeg.o: /usr/lib/qt/include/qcollect.h /usr/lib/qt/include/qdstream.h
jpeg.o: /usr/lib/qt/include/qiodev.h jpeg.h jpeginc.h
jpeg.o: /opt/kde/include/jpeglib.h /opt/kde/include/jconfig.h
jpeg.o: /opt/kde/include/jmorecfg.h
formats.o: /usr/lib/qt/include/qimage.h /usr/lib/qt/include/qpixmap.h
formats.o: /usr/lib/qt/include/qpaintd.h /usr/lib/qt/include/qwindefs.h
formats.o: /usr/lib/qt/include/qobjdefs.h /usr/lib/qt/include/qglobal.h
formats.o: /usr/lib/qt/include/qrect.h /usr/lib/qt/include/qsize.h
formats.o: /usr/lib/qt/include/qpoint.h /usr/lib/qt/include/qcolor.h
formats.o: /usr/lib/qt/include/qshared.h /usr/lib/qt/include/qstrlist.h
formats.o: /usr/lib/qt/include/qstring.h /usr/lib/qt/include/qarray.h
formats.o: /usr/lib/qt/include/qgarray.h /usr/lib/qt/include/qgeneric.h
formats.o: /usr/include/string.h /usr/include/features.h
formats.o: /usr/include/sys/cdefs.h
formats.o: /usr/lib/gcc-lib/i486-linux/2.7.2.1/include/stddef.h
formats.o: /usr/lib/qt/include/qlist.h /usr/lib/qt/include/qglist.h
formats.o: /usr/lib/qt/include/qcollect.h /usr/lib/qt/include/qdstream.h
formats.o: /usr/lib/qt/include/qiodev.h formats.h
qwmf.o: /usr/lib/qt/include/qfile.h /usr/lib/qt/include/qiodev.h
qwmf.o: /usr/lib/qt/include/qglobal.h /usr/lib/qt/include/qstring.h
qwmf.o: /usr/lib/qt/include/qarray.h /usr/lib/qt/include/qgarray.h
qwmf.o: /usr/lib/qt/include/qshared.h /usr/lib/qt/include/qgeneric.h
qwmf.o: /usr/include/string.h /usr/include/features.h
qwmf.o: /usr/include/sys/cdefs.h
qwmf.o: /usr/lib/gcc-lib/i486-linux/2.7.2.1/include/stddef.h
qwmf.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
qwmf.o: /usr/lib/qt/include/qfileinf.h /usr/lib/qt/include/qdatetm.h
qwmf.o: /usr/lib/qt/include/qpainter.h /usr/lib/qt/include/qpaintd.h
qwmf.o: /usr/lib/qt/include/qwindefs.h /usr/lib/qt/include/qobjdefs.h
qwmf.o: /usr/lib/qt/include/qrect.h /usr/lib/qt/include/qsize.h
qwmf.o: /usr/lib/qt/include/qpoint.h /usr/lib/qt/include/qcolor.h
qwmf.o: /usr/lib/qt/include/qfontmet.h /usr/lib/qt/include/qfont.h
qwmf.o: /usr/lib/qt/include/qfontinf.h /usr/lib/qt/include/qregion.h
qwmf.o: /usr/lib/qt/include/qpen.h /usr/lib/qt/include/qbrush.h
qwmf.o: /usr/lib/qt/include/qpntarry.h /usr/lib/qt/include/qwmatrix.h
qwmf.o: /usr/lib/qt/include/qdstream.h /usr/include/assert.h
qwmf.o: /usr/lib/qt/include/qapp.h /usr/lib/qt/include/qwidget.h
qwmf.o: /usr/lib/qt/include/qobject.h /usr/lib/qt/include/qevent.h
qwmf.o: /usr/lib/qt/include/qpalette.h /usr/lib/qt/include/qcursor.h
qwmf.o: /usr/include/endian.h /usr/include/bytesex.h qwmf.h
qwmf.o: /usr/lib/qt/include/qpicture.h /usr/lib/qt/include/qbuffer.h
qwmf.o: wmfstruct.h metafuncs.h
