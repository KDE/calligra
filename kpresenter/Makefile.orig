# Generated automatically from Makefile.in by configure.

INCDIR		=	-I$(QTDIR)/include -I$(KDEDIR)/include -I$(KDEDIR)/idl -I../lib/kofficeui
CXXFLAGS	=	$(INCDIR) -g -fexceptions -O0
LDFLAGS		=	-L$(KDEDIR)/lib -L/usr/X11/lib -L$(QTDIR)/lib -L$../lib/kofficeui -lkdeui -lkdecore -lkfm -lmico2.0.5 -lX11 -lXext -ldl -lkom -lop -ljpeg -lkfile -lqt -lkofficeui -lkofficecore -lkoml
CXX		=	g++
MOC		=	/usr/bin/moc

####### Files

SOURCES	=	kpresenter_main.cc kpresenter_doc.cc kpresenter_view.cc \
		kpresenter.cc kpresenter_shell.cc graphobj.cc atfinterpreter.cc\
		backdia.cc page.cc ktextobject.cc afchoose.cc styledia.cc \
		jpeg.cc formats.cc qwmf.cc \
		kenumlistdia.cc kcharselect.cc kcharselectdia.cc 
HEADERS =	kpresenter_main.h kpresenter_doc.h kpresenter_view.h \
		kpresenter.h kpresenter_shell.h graphobj.h atfinterpreter.h \
		backdia.h page.h ktextobject.h afchoose.h styledia.h \
		jpeg.h formats.h qwmf.h global.h \
		kenumlistdia.h kcharselect.h kcharselectdia.h 
OBJECTS =	kpresenter_main.o kpresenter_doc.o kpresenter_view.o \
		kpresenter.o kpresenter_shell.o graphobj.o atfinterpreter.o \
		backdia.o page.o ktextobject.o afchoose.o styledia.o \
		jpeg.o formats.o qwmf.o \
		kenumlistdia.o kcharselect.o kcharselectdia.o 
SRCMETA =	kpresenter_main.moc kpresenter_doc.moc kpresenter_view.moc \
		kpresenter_shell.moc graphobj.moc atfinterpreter.moc page.moc ktextobject.moc\
		backdia.moc afchoose.moc styledia.moc \
		kenumlistdia.moc kcharselect.moc kcharselectdia.moc 
TARGET	=	kpresenter
IDL	=	kpresenter.idl
IDLH	=	kpresenter.h

####### Install

INSTALL = install -m 0755 -s
INSTALL_SHELL = install -m 0755
INSTALL_DATA = install -m 0644
INSTALL_DIR = install -d -m 0755
INSTALL_DIR_PRIVATE = install -d -m 0700

####### Implicit rules

.SUFFIXES: .cc

.cc.o:
	$(CXX) -c $(CXXFLAGS) -DSTDC_HEADERS -DHAVE_UNISTD_H $(INCDIR) $<

%.moc: %.h
	$(MOC) $< -o $@      

####### Build rules

all: $(TARGET)
	(cd autoformEdit ; make all)

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

install:
	$(INSTALL_DIR) $(KDEDIR)/bin
	$(INSTALL_DIR) $(KDEDIR)/share/apps/koffice
	$(INSTALL_DIR) $(KDEDIR)/share/apps/koffice/partlnk
	$(INSTALL_DATA) kpresenter.kdelnk $(KDEDIR)/share/apps/koffice/partlnk/kpresenter.kdelnk
	$(INSTALL_DIR) $(KDEDIR)/idl
	$(INSTALL_DATA) kpresenter.idl $(KDEDIR)/idl/kpresenter.idl
	$(INSTALL_SHELL) kpresenter $(KDEDIR)/bin/kpresenter
	(cd pics ; make install)
	(cd toolbar ; make install)
	(cd autoforms ; make install)
	(cd autoformEdit ; make install)

####### Meta objects

# DO NOT DELETE THIS LINE -- make depend depends on it.

kpresenter_main.o: kpresenter_main.h /usr/local/kde/include/op_app.h
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
kpresenter_main.o: /usr/include/gnu/stubs.h
kpresenter_main.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
kpresenter_main.o: /usr/lib/qt/include/qevent.h /usr/lib/qt/include/qrect.h
kpresenter_main.o: /usr/lib/qt/include/qsize.h /usr/lib/qt/include/qpoint.h
kpresenter_main.o: /usr/lib/qt/include/qpaintd.h
kpresenter_main.o: /usr/lib/qt/include/qpalette.h
kpresenter_main.o: /usr/lib/qt/include/qcolor.h /usr/lib/qt/include/qcursor.h
kpresenter_main.o: /usr/lib/qt/include/qfont.h /usr/lib/qt/include/qfontmet.h
kpresenter_main.o: /usr/lib/qt/include/qfontinf.h
kpresenter_main.o: /usr/local/kde/include/kom.h
kpresenter_main.o: /usr/local/kde/include/kom_app.h
kpresenter_main.o: /usr/lib/qt/include/qsocknot.h
kpresenter_main.o: /usr/lib/qt/include/qtimer.h /usr/lib/qt/include/qlist.h
kpresenter_main.o: /usr/lib/qt/include/qglist.h
kpresenter_main.o: /usr/lib/qt/include/qcollect.h
kpresenter_main.o: /usr/lib/qt/include/qintdict.h
kpresenter_main.o: /usr/lib/qt/include/qgdict.h /usr/local/kde/include/kapp.h
kpresenter_main.o: /usr/local/kde/include/kconfig.h
kpresenter_main.o: /usr/local/kde/include/kconfigbase.h
kpresenter_main.o: /usr/lib/qt/include/qstrlist.h
kpresenter_main.o: /usr/lib/qt/include/qdstream.h
kpresenter_main.o: /usr/lib/qt/include/qiodev.h
kpresenter_main.o: /usr/local/kde/include/kconfigdata.h
kpresenter_main.o: /usr/lib/qt/include/qdict.h /usr/lib/qt/include/qtstream.h
kpresenter_main.o: /usr/include/stdio.h /usr/include/libio.h
kpresenter_main.o: /usr/include/_G_config.h /usr/include/gnu/types.h
kpresenter_main.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stdarg.h
kpresenter_main.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/va-alpha.h
kpresenter_main.o: /usr/include/stdio_lim.h /usr/lib/qt/include/qfile.h
kpresenter_main.o: /usr/local/kde/include/kdebug.h
kpresenter_main.o: /usr/local/kde/include/klocale.h
kpresenter_main.o: /usr/local/kde/include/drag.h
kpresenter_main.o: /usr/lib/qt/include/qpixmap.h /usr/include/X11/X.h
kpresenter_main.o: /usr/lib/qt/include/qpopmenu.h
kpresenter_main.o: /usr/lib/qt/include/qtablevw.h
kpresenter_main.o: /usr/lib/qt/include/qframe.h
kpresenter_main.o: /usr/lib/qt/include/qmenudta.h
kpresenter_main.o: /usr/local/kde/include/topwidget_impl.h
kpresenter_main.o: /usr/local/kde/include/controls.h /opt/kde/idl/kom.h
kpresenter_main.o: /usr/local/kde/include/component_impl.h
kpresenter_main.o: /usr/local/kde/include/base_impl.h
kpresenter_main.o: /usr/local/kde/include/kom_var.h
kpresenter_main.o: /usr/local/kde/include/control_frame.h
kpresenter_main.o: /usr/local/kde/include/control_impl.h
kpresenter_main.o: /usr/local/kde/include/kwm.h /usr/include/X11/Xlib.h
kpresenter_main.o: /usr/include/sys/types.h /usr/include/time.h
kpresenter_main.o: /usr/include/endian.h /usr/include/bytesex.h
kpresenter_main.o: /usr/include/sys/select.h /usr/include/selectbits.h
kpresenter_main.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
kpresenter_main.o: /usr/local/kde/include/part_impl.h
kpresenter_main.o: /usr/local/kde/include/parts.h
kpresenter_main.o: /usr/local/kde/include/kmenubar.h
kpresenter_main.o: /usr/lib/qt/include/qmenubar.h
kpresenter_main.o: /usr/local/kde/include/ktoolbar.h
kpresenter_main.o: /usr/lib/qt/include/qbutton.h
kpresenter_main.o: /usr/local/kde/include/kcombo.h
kpresenter_main.o: /usr/lib/qt/include/qcombo.h
kpresenter_main.o: /usr/local/kde/include/klined.h
kpresenter_main.o: /usr/lib/qt/include/qlined.h
kpresenter_main.o: /usr/local/kde/include/part_shell_impl.h
kpresenter_main.o: /usr/local/kde/include/ktopwidget.h /usr/include/stdlib.h
kpresenter_main.o: /usr/include/alloca.h /usr/local/kde/include/kstatusbar.h
kpresenter_main.o: /usr/lib/qt/include/qlabel.h
kpresenter_main.o: /usr/local/kde/include/toolbar_impl.h
kpresenter_main.o: /usr/lib/qt/include/qpushbt.h
kpresenter_main.o: /usr/local/kde/include/menu_impl.h kpresenter_doc.h
kpresenter_main.o: /usr/local/kde/include/document_impl.h
kpresenter_main.o: /usr/local/kde/include/view_impl.h
kpresenter_main.o: /usr/lib/qt/include/qpainter.h
kpresenter_main.o: /usr/lib/qt/include/qregion.h /usr/lib/qt/include/qpen.h
kpresenter_main.o: /usr/lib/qt/include/qbrush.h
kpresenter_main.o: /usr/lib/qt/include/qpntarry.h
kpresenter_main.o: /usr/lib/qt/include/qwmatrix.h
kpresenter_main.o: /usr/local/kde/include/koPageLayoutDia.h
kpresenter_main.o: /usr/lib/qt/include/qtabdlg.h
kpresenter_main.o: /usr/lib/qt/include/qdialog.h
kpresenter_main.o: /usr/lib/qt/include/qgrpbox.h
kpresenter_main.o: /usr/lib/qt/include/qlayout.h
kpresenter_main.o: /usr/lib/qt/include/qgmanagr.h
kpresenter_main.o: /usr/local/kde/include/krestrictedline.h kpresenter_view.h
kpresenter_main.o: /usr/local/kde/include/part_frame_impl.h
kpresenter_main.o: /usr/local/kde/include/utils.h
kpresenter_main.o: /usr/lib/qt/include/qmsgbox.h
kpresenter_main.o: /usr/lib/qt/include/qkeycode.h
kpresenter_main.o: /usr/lib/qt/include/qscrbar.h
kpresenter_main.o: /usr/lib/qt/include/qrangect.h
kpresenter_main.o: /usr/lib/qt/include/qdrawutl.h
kpresenter_main.o: /usr/lib/qt/include/qfileinf.h
kpresenter_main.o: /usr/lib/qt/include/qdatetm.h
kpresenter_main.o: /usr/local/kde/include/kcolordlg.h
kpresenter_main.o: /usr/local/kde/include/kselect.h
kpresenter_main.o: /usr/local/kde/include/koAboutDia.h
kpresenter_main.o: /usr/local/kde/include/kbuttonbox.h
kpresenter_main.o: /usr/local/kde/include/kfontdialog.h
kpresenter_main.o: /usr/lib/qt/include/qbttngrp.h
kpresenter_main.o: /usr/lib/qt/include/qchkbox.h
kpresenter_main.o: /usr/lib/qt/include/qlistbox.h
kpresenter_main.o: /usr/lib/qt/include/qradiobt.h
kpresenter_main.o: /usr/lib/qt/include/qtooltip.h kpresenter.h
kpresenter_main.o: /usr/local/kde/idl/kom.h /usr/local/kde/idl/controls.h
kpresenter_main.o: /usr/local/kde/idl/parts.h global.h
kpresenter_main.o: /usr/lib/qt/include/qpicture.h
kpresenter_main.o: /usr/lib/qt/include/qbuffer.h backdia.h qwmf.h afchoose.h
kpresenter_main.o: /usr/local/kde/include/kiconloaderdialog.h
kpresenter_main.o: /usr/local/kde/include/kiconloader.h styledia.h
kpresenter_main.o: ktextobject.h graphobj.h atfinterpreter.h
kpresenter_main.o: /usr/lib/qt/include/qstack.h kpresenter_shell.h
kpresenter_main.o: /usr/local/kde/include/default_shell_impl.h
kpresenter_main.o: /usr/local/kde/include/default_shell.h formats.h
kpresenter_main.o: /usr/lib/qt/include/qimage.h kpresenter_main.moc
kpresenter_main.o: /usr/lib/qt/include/qmetaobj.h
kpresenter_main.o: /usr/lib/qt/include/qconnect.h
kpresenter_doc.o: kpresenter_doc.h /usr/local/kde/include/document_impl.h
kpresenter_doc.o: /usr/local/kde/include/parts.h /opt/kde/idl/kom.h
kpresenter_doc.o: /usr/local/kde/include/controls.h
kpresenter_doc.o: /usr/local/kde/include/component_impl.h
kpresenter_doc.o: /usr/local/kde/include/kom.h
kpresenter_doc.o: /usr/local/kde/include/base_impl.h
kpresenter_doc.o: /usr/lib/qt/include/qstring.h /usr/lib/qt/include/qarray.h
kpresenter_doc.o: /usr/lib/qt/include/qgarray.h /usr/lib/qt/include/qshared.h
kpresenter_doc.o: /usr/lib/qt/include/qglobal.h
kpresenter_doc.o: /usr/lib/qt/include/qgeneric.h /usr/include/string.h
kpresenter_doc.o: /usr/include/features.h /usr/include/sys/cdefs.h
kpresenter_doc.o: /usr/include/gnu/stubs.h
kpresenter_doc.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
kpresenter_doc.o: /usr/lib/qt/include/qlist.h /usr/lib/qt/include/qglist.h
kpresenter_doc.o: /usr/lib/qt/include/qcollect.h /usr/lib/qt/include/qdict.h
kpresenter_doc.o: /usr/lib/qt/include/qgdict.h
kpresenter_doc.o: /usr/local/kde/include/kom_var.h
kpresenter_doc.o: /usr/local/kde/include/view_impl.h
kpresenter_doc.o: /usr/local/kde/include/part_impl.h
kpresenter_doc.o: /usr/local/kde/include/control_impl.h
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
kpresenter_doc.o: /usr/local/kde/include/koPageLayoutDia.h
kpresenter_doc.o: /usr/include/stdio.h /usr/include/libio.h
kpresenter_doc.o: /usr/include/_G_config.h /usr/include/gnu/types.h
kpresenter_doc.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stdarg.h
kpresenter_doc.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/va-alpha.h
kpresenter_doc.o: /usr/include/stdio_lim.h /usr/include/stdlib.h
kpresenter_doc.o: /usr/include/sys/types.h /usr/include/time.h
kpresenter_doc.o: /usr/include/endian.h /usr/include/bytesex.h
kpresenter_doc.o: /usr/include/sys/select.h /usr/include/selectbits.h
kpresenter_doc.o: /usr/include/alloca.h /usr/lib/qt/include/qtabdlg.h
kpresenter_doc.o: /usr/lib/qt/include/qdialog.h /usr/lib/qt/include/qcombo.h
kpresenter_doc.o: /usr/lib/qt/include/qlabel.h /usr/lib/qt/include/qgrpbox.h
kpresenter_doc.o: /usr/lib/qt/include/qlayout.h
kpresenter_doc.o: /usr/lib/qt/include/qgmanagr.h
kpresenter_doc.o: /usr/lib/qt/include/qintdict.h /usr/lib/qt/include/qlined.h
kpresenter_doc.o: /usr/local/kde/include/kapp.h
kpresenter_doc.o: /usr/local/kde/include/kconfig.h
kpresenter_doc.o: /usr/local/kde/include/kconfigbase.h
kpresenter_doc.o: /usr/lib/qt/include/qstrlist.h
kpresenter_doc.o: /usr/lib/qt/include/qdstream.h /usr/lib/qt/include/qiodev.h
kpresenter_doc.o: /usr/local/kde/include/kconfigdata.h
kpresenter_doc.o: /usr/lib/qt/include/qtstream.h /usr/lib/qt/include/qfile.h
kpresenter_doc.o: /usr/local/kde/include/kdebug.h
kpresenter_doc.o: /usr/local/kde/include/klocale.h
kpresenter_doc.o: /usr/local/kde/include/drag.h /usr/lib/qt/include/qapp.h
kpresenter_doc.o: /usr/include/X11/X.h
kpresenter_doc.o: /usr/local/kde/include/krestrictedline.h kpresenter_view.h
kpresenter_doc.o: /usr/local/kde/include/part_frame_impl.h
kpresenter_doc.o: /usr/local/kde/include/control_frame.h
kpresenter_doc.o: /usr/local/kde/include/kwm.h /usr/include/X11/Xlib.h
kpresenter_doc.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
kpresenter_doc.o: /usr/local/kde/include/menu_impl.h
kpresenter_doc.o: /usr/local/kde/include/kmenubar.h
kpresenter_doc.o: /usr/lib/qt/include/qmenubar.h
kpresenter_doc.o: /usr/local/kde/include/ktopwidget.h
kpresenter_doc.o: /usr/local/kde/include/ktoolbar.h
kpresenter_doc.o: /usr/lib/qt/include/qbutton.h
kpresenter_doc.o: /usr/local/kde/include/kcombo.h
kpresenter_doc.o: /usr/local/kde/include/klined.h
kpresenter_doc.o: /usr/local/kde/include/kstatusbar.h
kpresenter_doc.o: /usr/lib/qt/include/qtimer.h
kpresenter_doc.o: /usr/local/kde/include/toolbar_impl.h
kpresenter_doc.o: /usr/lib/qt/include/qpushbt.h
kpresenter_doc.o: /usr/local/kde/include/op_app.h
kpresenter_doc.o: /usr/local/kde/include/kom_app.h
kpresenter_doc.o: /usr/lib/qt/include/qsocknot.h
kpresenter_doc.o: /usr/local/kde/include/topwidget_impl.h
kpresenter_doc.o: /usr/local/kde/include/utils.h
kpresenter_doc.o: /usr/lib/qt/include/qmsgbox.h
kpresenter_doc.o: /usr/lib/qt/include/qkeycode.h
kpresenter_doc.o: /usr/lib/qt/include/qscrbar.h
kpresenter_doc.o: /usr/lib/qt/include/qrangect.h
kpresenter_doc.o: /usr/lib/qt/include/qdrawutl.h
kpresenter_doc.o: /usr/lib/qt/include/qfileinf.h
kpresenter_doc.o: /usr/lib/qt/include/qdatetm.h
kpresenter_doc.o: /usr/local/kde/include/kcolordlg.h
kpresenter_doc.o: /usr/local/kde/include/kselect.h
kpresenter_doc.o: /usr/local/kde/include/koAboutDia.h
kpresenter_doc.o: /usr/local/kde/include/kbuttonbox.h
kpresenter_doc.o: /usr/local/kde/include/kfontdialog.h
kpresenter_doc.o: /usr/lib/qt/include/qbttngrp.h
kpresenter_doc.o: /usr/lib/qt/include/qchkbox.h
kpresenter_doc.o: /usr/lib/qt/include/qlistbox.h
kpresenter_doc.o: /usr/lib/qt/include/qradiobt.h
kpresenter_doc.o: /usr/lib/qt/include/qtooltip.h kpresenter.h
kpresenter_doc.o: /usr/local/kde/idl/kom.h /usr/local/kde/idl/controls.h
kpresenter_doc.o: /usr/local/kde/idl/parts.h kpresenter_main.h
kpresenter_doc.o: /usr/local/kde/include/part_shell_impl.h kpresenter_shell.h
kpresenter_doc.o: /usr/local/kde/include/default_shell_impl.h
kpresenter_doc.o: /usr/local/kde/include/default_shell.h formats.h
kpresenter_doc.o: /usr/lib/qt/include/qimage.h global.h
kpresenter_doc.o: /usr/lib/qt/include/qpicture.h
kpresenter_doc.o: /usr/lib/qt/include/qbuffer.h backdia.h qwmf.h afchoose.h
kpresenter_doc.o: /usr/local/kde/include/kiconloaderdialog.h
kpresenter_doc.o: /usr/local/kde/include/kiconloader.h styledia.h
kpresenter_doc.o: ktextobject.h graphobj.h atfinterpreter.h
kpresenter_doc.o: /usr/lib/qt/include/qstack.h kpresenter_doc.moc
kpresenter_doc.o: /usr/lib/qt/include/qmetaobj.h
kpresenter_doc.o: /usr/lib/qt/include/qconnect.h
kpresenter_view.o: /usr/local/kde/include/kfiledialog.h
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
kpresenter_view.o: /usr/include/gnu/stubs.h
kpresenter_view.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
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
kpresenter_view.o: /usr/local/kde/include/kfileinfo.h
kpresenter_view.o: /usr/lib/qt/include/qdatetm.h /usr/include/time.h
kpresenter_view.o: /usr/include/gnu/types.h kpresenter_view.h
kpresenter_view.o: /usr/include/stdlib.h /usr/include/sys/types.h
kpresenter_view.o: /usr/include/endian.h /usr/include/bytesex.h
kpresenter_view.o: /usr/include/sys/select.h /usr/include/selectbits.h
kpresenter_view.o: /usr/include/alloca.h /usr/local/kde/include/view_impl.h
kpresenter_view.o: /usr/local/kde/include/parts.h /opt/kde/idl/kom.h
kpresenter_view.o: /usr/local/kde/include/controls.h
kpresenter_view.o: /usr/local/kde/include/part_impl.h
kpresenter_view.o: /usr/local/kde/include/control_impl.h
kpresenter_view.o: /usr/local/kde/include/component_impl.h
kpresenter_view.o: /usr/local/kde/include/kom.h
kpresenter_view.o: /usr/local/kde/include/base_impl.h
kpresenter_view.o: /usr/lib/qt/include/qdict.h /usr/lib/qt/include/qgdict.h
kpresenter_view.o: /usr/local/kde/include/kom_var.h
kpresenter_view.o: /usr/local/kde/include/document_impl.h
kpresenter_view.o: /usr/local/kde/include/part_frame_impl.h
kpresenter_view.o: /usr/local/kde/include/control_frame.h
kpresenter_view.o: /usr/local/kde/include/kwm.h /usr/lib/qt/include/qapp.h
kpresenter_view.o: /usr/include/X11/Xlib.h /usr/include/X11/X.h
kpresenter_view.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
kpresenter_view.o: /usr/local/kde/include/kapp.h
kpresenter_view.o: /usr/local/kde/include/kconfig.h
kpresenter_view.o: /usr/local/kde/include/kconfigbase.h
kpresenter_view.o: /usr/local/kde/include/kconfigdata.h
kpresenter_view.o: /usr/lib/qt/include/qtstream.h /usr/include/stdio.h
kpresenter_view.o: /usr/include/libio.h /usr/include/_G_config.h
kpresenter_view.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stdarg.h
kpresenter_view.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/va-alpha.h
kpresenter_view.o: /usr/include/stdio_lim.h /usr/lib/qt/include/qfile.h
kpresenter_view.o: /usr/local/kde/include/kdebug.h
kpresenter_view.o: /usr/local/kde/include/klocale.h
kpresenter_view.o: /usr/lib/qt/include/qintdict.h
kpresenter_view.o: /usr/local/kde/include/drag.h
kpresenter_view.o: /usr/lib/qt/include/qpopmenu.h
kpresenter_view.o: /usr/lib/qt/include/qtablevw.h
kpresenter_view.o: /usr/lib/qt/include/qframe.h
kpresenter_view.o: /usr/lib/qt/include/qmenudta.h
kpresenter_view.o: /usr/local/kde/include/menu_impl.h
kpresenter_view.o: /usr/local/kde/include/kmenubar.h
kpresenter_view.o: /usr/lib/qt/include/qmenubar.h
kpresenter_view.o: /usr/local/kde/include/ktopwidget.h
kpresenter_view.o: /usr/local/kde/include/ktoolbar.h
kpresenter_view.o: /usr/lib/qt/include/qbutton.h
kpresenter_view.o: /usr/local/kde/include/kcombo.h
kpresenter_view.o: /usr/lib/qt/include/qcombo.h
kpresenter_view.o: /usr/local/kde/include/klined.h
kpresenter_view.o: /usr/lib/qt/include/qlined.h
kpresenter_view.o: /usr/local/kde/include/kstatusbar.h
kpresenter_view.o: /usr/lib/qt/include/qtimer.h /usr/lib/qt/include/qlabel.h
kpresenter_view.o: /usr/local/kde/include/toolbar_impl.h
kpresenter_view.o: /usr/lib/qt/include/qpushbt.h
kpresenter_view.o: /usr/local/kde/include/op_app.h
kpresenter_view.o: /usr/local/kde/include/kom_app.h
kpresenter_view.o: /usr/lib/qt/include/qsocknot.h
kpresenter_view.o: /usr/local/kde/include/topwidget_impl.h
kpresenter_view.o: /usr/local/kde/include/utils.h
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
kpresenter_view.o: /usr/local/kde/include/kcolordlg.h
kpresenter_view.o: /usr/local/kde/include/kselect.h
kpresenter_view.o: /usr/local/kde/include/koAboutDia.h
kpresenter_view.o: /usr/lib/qt/include/qlayout.h
kpresenter_view.o: /usr/lib/qt/include/qgmanagr.h
kpresenter_view.o: /usr/local/kde/include/kbuttonbox.h
kpresenter_view.o: /usr/local/kde/include/koPageLayoutDia.h
kpresenter_view.o: /usr/lib/qt/include/qtabdlg.h
kpresenter_view.o: /usr/lib/qt/include/qgrpbox.h
kpresenter_view.o: /usr/local/kde/include/krestrictedline.h
kpresenter_view.o: /usr/local/kde/include/kfontdialog.h
kpresenter_view.o: /usr/lib/qt/include/qbttngrp.h
kpresenter_view.o: /usr/lib/qt/include/qchkbox.h
kpresenter_view.o: /usr/lib/qt/include/qlistbox.h
kpresenter_view.o: /usr/lib/qt/include/qradiobt.h
kpresenter_view.o: /usr/lib/qt/include/qtooltip.h kpresenter.h
kpresenter_view.o: /usr/local/kde/idl/kom.h /usr/local/kde/idl/controls.h
kpresenter_view.o: /usr/local/kde/idl/parts.h kpresenter_doc.h global.h
kpresenter_view.o: /usr/lib/qt/include/qpicture.h
kpresenter_view.o: /usr/lib/qt/include/qbuffer.h qwmf.h graphobj.h
kpresenter_view.o: atfinterpreter.h ktextobject.h kpresenter_main.h
kpresenter_view.o: /usr/local/kde/include/part_shell_impl.h
kpresenter_view.o: kpresenter_shell.h
kpresenter_view.o: /usr/local/kde/include/default_shell_impl.h
kpresenter_view.o: /usr/local/kde/include/default_shell.h formats.h
kpresenter_view.o: /usr/lib/qt/include/qimage.h backdia.h afchoose.h
kpresenter_view.o: /usr/local/kde/include/kiconloaderdialog.h
kpresenter_view.o: /usr/local/kde/include/kiconloader.h styledia.h
kpresenter_view.o: kpresenter_view.moc /usr/lib/qt/include/qmetaobj.h
kpresenter_view.o: /usr/lib/qt/include/qconnect.h page.h
kpresenter.o: kpresenter.h /usr/local/kde/idl/kom.h
kpresenter.o: /usr/local/kde/idl/controls.h /usr/local/kde/idl/parts.h
kpresenter_shell.o: kpresenter_shell.h
kpresenter_shell.o: /usr/local/kde/include/default_shell_impl.h
kpresenter_shell.o: /usr/local/kde/include/default_shell.h /opt/kde/idl/kom.h
kpresenter_shell.o: /usr/local/kde/include/controls.h
kpresenter_shell.o: /usr/local/kde/include/parts.h
kpresenter_shell.o: /usr/local/kde/include/part_shell_impl.h
kpresenter_shell.o: /usr/local/kde/include/ktopwidget.h /usr/include/stdlib.h
kpresenter_shell.o: /usr/include/features.h /usr/include/sys/cdefs.h
kpresenter_shell.o: /usr/include/gnu/stubs.h
kpresenter_shell.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
kpresenter_shell.o: /usr/include/sys/types.h /usr/include/gnu/types.h
kpresenter_shell.o: /usr/include/time.h /usr/include/endian.h
kpresenter_shell.o: /usr/include/bytesex.h /usr/include/sys/select.h
kpresenter_shell.o: /usr/include/selectbits.h /usr/include/alloca.h
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
kpresenter_shell.o: /usr/local/kde/include/ktoolbar.h
kpresenter_shell.o: /usr/lib/qt/include/qframe.h
kpresenter_shell.o: /usr/lib/qt/include/qpixmap.h
kpresenter_shell.o: /usr/lib/qt/include/qpopmenu.h
kpresenter_shell.o: /usr/lib/qt/include/qtablevw.h
kpresenter_shell.o: /usr/lib/qt/include/qmenudta.h
kpresenter_shell.o: /usr/lib/qt/include/qbutton.h
kpresenter_shell.o: /usr/local/kde/include/kcombo.h
kpresenter_shell.o: /usr/lib/qt/include/qcombo.h
kpresenter_shell.o: /usr/local/kde/include/klined.h
kpresenter_shell.o: /usr/lib/qt/include/qlined.h
kpresenter_shell.o: /usr/local/kde/include/kmenubar.h
kpresenter_shell.o: /usr/lib/qt/include/qmenubar.h
kpresenter_shell.o: /usr/local/kde/include/kstatusbar.h
kpresenter_shell.o: /usr/lib/qt/include/qtimer.h /usr/lib/qt/include/qlabel.h
kpresenter_shell.o: /usr/local/kde/include/kconfig.h
kpresenter_shell.o: /usr/local/kde/include/kconfigbase.h
kpresenter_shell.o: /usr/lib/qt/include/qstrlist.h
kpresenter_shell.o: /usr/lib/qt/include/qdstream.h
kpresenter_shell.o: /usr/lib/qt/include/qiodev.h
kpresenter_shell.o: /usr/local/kde/include/kconfigdata.h
kpresenter_shell.o: /usr/lib/qt/include/qdict.h /usr/lib/qt/include/qgdict.h
kpresenter_shell.o: /usr/lib/qt/include/qtstream.h /usr/include/stdio.h
kpresenter_shell.o: /usr/include/libio.h /usr/include/_G_config.h
kpresenter_shell.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stdarg.h
kpresenter_shell.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/va-alpha.h
kpresenter_shell.o: /usr/include/stdio_lim.h /usr/lib/qt/include/qfile.h
kpresenter_shell.o: /usr/local/kde/include/kdebug.h
kpresenter_shell.o: /usr/local/kde/include/part_impl.h
kpresenter_shell.o: /usr/local/kde/include/control_impl.h
kpresenter_shell.o: /usr/local/kde/include/component_impl.h
kpresenter_shell.o: /usr/local/kde/include/kom.h
kpresenter_shell.o: /usr/local/kde/include/base_impl.h
kpresenter_shell.o: /usr/local/kde/include/kom_var.h
kpresenter_shell.o: /usr/local/kde/include/toolbar_impl.h
kpresenter_shell.o: /usr/lib/qt/include/qpushbt.h
kpresenter_shell.o: /usr/lib/qt/include/qintdict.h
kpresenter_shell.o: /usr/local/kde/include/menu_impl.h
kpresenter_shell.o: /usr/local/kde/include/control_frame.h
kpresenter_shell.o: /usr/local/kde/include/kwm.h /usr/lib/qt/include/qapp.h
kpresenter_shell.o: /usr/include/X11/Xlib.h /usr/include/X11/X.h
kpresenter_shell.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
kpresenter_shell.o: /usr/local/kde/include/kapp.h
kpresenter_shell.o: /usr/local/kde/include/klocale.h
kpresenter_shell.o: /usr/local/kde/include/drag.h
kpresenter_shell.o: /usr/local/kde/include/topwidget_impl.h
kpresenter_shell.o: /usr/local/kde/include/document_impl.h
kpresenter_shell.o: /usr/local/kde/include/view_impl.h kpresenter.h
kpresenter_shell.o: /usr/local/kde/idl/kom.h /usr/local/kde/idl/controls.h
kpresenter_shell.o: /usr/local/kde/idl/parts.h kpresenter_doc.h
kpresenter_shell.o: /usr/lib/qt/include/qpainter.h
kpresenter_shell.o: /usr/lib/qt/include/qregion.h /usr/lib/qt/include/qpen.h
kpresenter_shell.o: /usr/lib/qt/include/qbrush.h
kpresenter_shell.o: /usr/lib/qt/include/qpntarry.h
kpresenter_shell.o: /usr/lib/qt/include/qwmatrix.h
kpresenter_shell.o: /usr/local/kde/include/koPageLayoutDia.h
kpresenter_shell.o: /usr/lib/qt/include/qtabdlg.h
kpresenter_shell.o: /usr/lib/qt/include/qdialog.h
kpresenter_shell.o: /usr/lib/qt/include/qgrpbox.h
kpresenter_shell.o: /usr/lib/qt/include/qlayout.h
kpresenter_shell.o: /usr/lib/qt/include/qgmanagr.h
kpresenter_shell.o: /usr/local/kde/include/krestrictedline.h
kpresenter_shell.o: kpresenter_view.h
kpresenter_shell.o: /usr/local/kde/include/part_frame_impl.h
kpresenter_shell.o: /usr/local/kde/include/op_app.h
kpresenter_shell.o: /usr/local/kde/include/kom_app.h
kpresenter_shell.o: /usr/lib/qt/include/qsocknot.h
kpresenter_shell.o: /usr/local/kde/include/utils.h
kpresenter_shell.o: /usr/lib/qt/include/qmsgbox.h
kpresenter_shell.o: /usr/lib/qt/include/qkeycode.h
kpresenter_shell.o: /usr/lib/qt/include/qscrbar.h
kpresenter_shell.o: /usr/lib/qt/include/qrangect.h
kpresenter_shell.o: /usr/lib/qt/include/qdrawutl.h
kpresenter_shell.o: /usr/lib/qt/include/qfileinf.h
kpresenter_shell.o: /usr/lib/qt/include/qdatetm.h
kpresenter_shell.o: /usr/local/kde/include/kcolordlg.h
kpresenter_shell.o: /usr/local/kde/include/kselect.h
kpresenter_shell.o: /usr/local/kde/include/koAboutDia.h
kpresenter_shell.o: /usr/local/kde/include/kbuttonbox.h
kpresenter_shell.o: /usr/local/kde/include/kfontdialog.h
kpresenter_shell.o: /usr/lib/qt/include/qbttngrp.h
kpresenter_shell.o: /usr/lib/qt/include/qchkbox.h
kpresenter_shell.o: /usr/lib/qt/include/qlistbox.h
kpresenter_shell.o: /usr/lib/qt/include/qradiobt.h
kpresenter_shell.o: /usr/lib/qt/include/qtooltip.h kpresenter_main.h
kpresenter_shell.o: formats.h /usr/lib/qt/include/qimage.h global.h
kpresenter_shell.o: /usr/lib/qt/include/qpicture.h
kpresenter_shell.o: /usr/lib/qt/include/qbuffer.h backdia.h qwmf.h afchoose.h
kpresenter_shell.o: /usr/local/kde/include/kiconloaderdialog.h
kpresenter_shell.o: /usr/local/kde/include/kiconloader.h styledia.h
kpresenter_shell.o: ktextobject.h graphobj.h atfinterpreter.h
kpresenter_shell.o: /usr/lib/qt/include/qstack.h kpresenter_shell.moc
kpresenter_shell.o: /usr/lib/qt/include/qmetaobj.h
kpresenter_shell.o: /usr/lib/qt/include/qconnect.h
graphobj.o: graphobj.h /usr/lib/qt/include/qwidget.h
graphobj.o: /usr/lib/qt/include/qwindefs.h /usr/lib/qt/include/qobjdefs.h
graphobj.o: /usr/lib/qt/include/qglobal.h /usr/lib/qt/include/qobject.h
graphobj.o: /usr/lib/qt/include/qstring.h /usr/lib/qt/include/qarray.h
graphobj.o: /usr/lib/qt/include/qgarray.h /usr/lib/qt/include/qshared.h
graphobj.o: /usr/lib/qt/include/qgeneric.h /usr/include/string.h
graphobj.o: /usr/include/features.h /usr/include/sys/cdefs.h
graphobj.o: /usr/include/gnu/stubs.h
graphobj.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
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
graphobj.o: /usr/include/_G_config.h /usr/include/gnu/types.h
graphobj.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stdarg.h
graphobj.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/va-alpha.h
graphobj.o: /usr/include/stdio_lim.h /usr/lib/qt/include/qdatetm.h
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
atfinterpreter.o: /usr/include/gnu/stubs.h
atfinterpreter.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
atfinterpreter.o: /usr/lib/qt/include/qevent.h /usr/lib/qt/include/qwindefs.h
atfinterpreter.o: /usr/lib/qt/include/qrect.h /usr/lib/qt/include/qsize.h
atfinterpreter.o: /usr/lib/qt/include/qpoint.h /usr/lib/qt/include/qstack.h
atfinterpreter.o: /usr/lib/qt/include/qglist.h /usr/lib/qt/include/qcollect.h
atfinterpreter.o: /usr/lib/qt/include/qpntarry.h /usr/lib/qt/include/qlist.h
atfinterpreter.o: /usr/lib/qt/include/qfile.h /usr/lib/qt/include/qiodev.h
atfinterpreter.o: /usr/include/stdio.h /usr/include/libio.h
atfinterpreter.o: /usr/include/_G_config.h /usr/include/gnu/types.h
atfinterpreter.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stdarg.h
atfinterpreter.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/va-alpha.h
atfinterpreter.o: /usr/include/stdio_lim.h /usr/lib/qt/include/qstrlist.h
atfinterpreter.o: /usr/lib/qt/include/qdstream.h atfinterpreter.moc
atfinterpreter.o: /usr/lib/qt/include/qmetaobj.h
atfinterpreter.o: /usr/lib/qt/include/qconnect.h
backdia.o: /usr/local/kde/include/kfiledialog.h /usr/lib/qt/include/qdialog.h
backdia.o: /usr/lib/qt/include/qwidget.h /usr/lib/qt/include/qwindefs.h
backdia.o: /usr/lib/qt/include/qobjdefs.h /usr/lib/qt/include/qglobal.h
backdia.o: /usr/lib/qt/include/qobject.h /usr/lib/qt/include/qstring.h
backdia.o: /usr/lib/qt/include/qarray.h /usr/lib/qt/include/qgarray.h
backdia.o: /usr/lib/qt/include/qshared.h /usr/lib/qt/include/qgeneric.h
backdia.o: /usr/include/string.h /usr/include/features.h
backdia.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
backdia.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
backdia.o: /usr/lib/qt/include/qevent.h /usr/lib/qt/include/qrect.h
backdia.o: /usr/lib/qt/include/qsize.h /usr/lib/qt/include/qpoint.h
backdia.o: /usr/lib/qt/include/qpaintd.h /usr/lib/qt/include/qpalette.h
backdia.o: /usr/lib/qt/include/qcolor.h /usr/lib/qt/include/qcursor.h
backdia.o: /usr/lib/qt/include/qfont.h /usr/lib/qt/include/qfontmet.h
backdia.o: /usr/lib/qt/include/qfontinf.h /usr/lib/qt/include/qstack.h
backdia.o: /usr/lib/qt/include/qglist.h /usr/lib/qt/include/qcollect.h
backdia.o: /usr/lib/qt/include/qstrlist.h /usr/lib/qt/include/qlist.h
backdia.o: /usr/lib/qt/include/qdstream.h /usr/lib/qt/include/qiodev.h
backdia.o: /usr/lib/qt/include/qpixmap.h /usr/local/kde/include/kfileinfo.h
backdia.o: /usr/lib/qt/include/qdatetm.h /usr/include/time.h
backdia.o: /usr/include/gnu/types.h backdia.h /usr/include/stdlib.h
backdia.o: /usr/include/sys/types.h /usr/include/endian.h
backdia.o: /usr/include/bytesex.h /usr/include/sys/select.h
backdia.o: /usr/include/selectbits.h /usr/include/alloca.h
backdia.o: /usr/lib/qt/include/qlabel.h /usr/lib/qt/include/qframe.h
backdia.o: /usr/lib/qt/include/qgrpbox.h /usr/lib/qt/include/qradiobt.h
backdia.o: /usr/lib/qt/include/qbutton.h /usr/lib/qt/include/qpushbt.h
backdia.o: /usr/lib/qt/include/qbttngrp.h /usr/lib/qt/include/qwmatrix.h
backdia.o: /usr/lib/qt/include/qpntarry.h /usr/lib/qt/include/qapp.h
backdia.o: /usr/lib/qt/include/qpicture.h /usr/lib/qt/include/qbuffer.h
backdia.o: /usr/lib/qt/include/qpainter.h /usr/lib/qt/include/qregion.h
backdia.o: /usr/lib/qt/include/qpen.h /usr/lib/qt/include/qbrush.h
backdia.o: /usr/local/kde/include/kcolordlg.h /usr/lib/qt/include/qtablevw.h
backdia.o: /usr/lib/qt/include/qrangect.h /usr/lib/qt/include/qlined.h
backdia.o: /usr/lib/qt/include/qcombo.h /usr/local/kde/include/kselect.h
backdia.o: qwmf.h global.h backdia.moc /usr/lib/qt/include/qmetaobj.h
backdia.o: /usr/lib/qt/include/qconnect.h
page.o: page.h kpresenter_view.h /usr/include/stdlib.h
page.o: /usr/include/features.h /usr/include/sys/cdefs.h
page.o: /usr/include/gnu/stubs.h
page.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
page.o: /usr/include/sys/types.h /usr/include/gnu/types.h /usr/include/time.h
page.o: /usr/include/endian.h /usr/include/bytesex.h
page.o: /usr/include/sys/select.h /usr/include/selectbits.h
page.o: /usr/include/alloca.h /usr/local/kde/include/view_impl.h
page.o: /usr/local/kde/include/parts.h /opt/kde/idl/kom.h
page.o: /usr/local/kde/include/controls.h /usr/local/kde/include/part_impl.h
page.o: /usr/local/kde/include/control_impl.h
page.o: /usr/local/kde/include/component_impl.h /usr/local/kde/include/kom.h
page.o: /usr/local/kde/include/base_impl.h /usr/lib/qt/include/qstring.h
page.o: /usr/lib/qt/include/qarray.h /usr/lib/qt/include/qgarray.h
page.o: /usr/lib/qt/include/qshared.h /usr/lib/qt/include/qglobal.h
page.o: /usr/lib/qt/include/qgeneric.h /usr/include/string.h
page.o: /usr/lib/qt/include/qlist.h /usr/lib/qt/include/qglist.h
page.o: /usr/lib/qt/include/qcollect.h /usr/lib/qt/include/qdict.h
page.o: /usr/lib/qt/include/qgdict.h /usr/local/kde/include/kom_var.h
page.o: /usr/local/kde/include/document_impl.h
page.o: /usr/local/kde/include/part_frame_impl.h
page.o: /usr/local/kde/include/control_frame.h /usr/lib/qt/include/qwidget.h
page.o: /usr/lib/qt/include/qwindefs.h /usr/lib/qt/include/qobjdefs.h
page.o: /usr/lib/qt/include/qobject.h /usr/lib/qt/include/qevent.h
page.o: /usr/lib/qt/include/qrect.h /usr/lib/qt/include/qsize.h
page.o: /usr/lib/qt/include/qpoint.h /usr/lib/qt/include/qpaintd.h
page.o: /usr/lib/qt/include/qpalette.h /usr/lib/qt/include/qcolor.h
page.o: /usr/lib/qt/include/qcursor.h /usr/lib/qt/include/qfont.h
page.o: /usr/lib/qt/include/qfontmet.h /usr/lib/qt/include/qfontinf.h
page.o: /usr/local/kde/include/kwm.h /usr/lib/qt/include/qapp.h
page.o: /usr/lib/qt/include/qpixmap.h /usr/include/X11/Xlib.h
page.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
page.o: /usr/include/X11/Xosdefs.h /usr/local/kde/include/kapp.h
page.o: /usr/local/kde/include/kconfig.h /usr/local/kde/include/kconfigbase.h
page.o: /usr/lib/qt/include/qstrlist.h /usr/lib/qt/include/qdstream.h
page.o: /usr/lib/qt/include/qiodev.h /usr/local/kde/include/kconfigdata.h
page.o: /usr/lib/qt/include/qtstream.h /usr/include/stdio.h
page.o: /usr/include/libio.h /usr/include/_G_config.h
page.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stdarg.h
page.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/va-alpha.h
page.o: /usr/include/stdio_lim.h /usr/lib/qt/include/qfile.h
page.o: /usr/local/kde/include/kdebug.h /usr/local/kde/include/klocale.h
page.o: /usr/lib/qt/include/qintdict.h /usr/local/kde/include/drag.h
page.o: /usr/lib/qt/include/qpopmenu.h /usr/lib/qt/include/qtablevw.h
page.o: /usr/lib/qt/include/qframe.h /usr/lib/qt/include/qmenudta.h
page.o: /usr/local/kde/include/menu_impl.h /usr/local/kde/include/kmenubar.h
page.o: /usr/lib/qt/include/qmenubar.h /usr/local/kde/include/ktopwidget.h
page.o: /usr/local/kde/include/ktoolbar.h /usr/lib/qt/include/qbutton.h
page.o: /usr/local/kde/include/kcombo.h /usr/lib/qt/include/qcombo.h
page.o: /usr/local/kde/include/klined.h /usr/lib/qt/include/qlined.h
page.o: /usr/local/kde/include/kstatusbar.h /usr/lib/qt/include/qtimer.h
page.o: /usr/lib/qt/include/qlabel.h /usr/local/kde/include/toolbar_impl.h
page.o: /usr/lib/qt/include/qpushbt.h /usr/local/kde/include/op_app.h
page.o: /usr/local/kde/include/kom_app.h /usr/lib/qt/include/qsocknot.h
page.o: /usr/local/kde/include/topwidget_impl.h
page.o: /usr/local/kde/include/utils.h /usr/lib/qt/include/qpainter.h
page.o: /usr/lib/qt/include/qregion.h /usr/lib/qt/include/qpen.h
page.o: /usr/lib/qt/include/qbrush.h /usr/lib/qt/include/qpntarry.h
page.o: /usr/lib/qt/include/qwmatrix.h /usr/lib/qt/include/qmsgbox.h
page.o: /usr/lib/qt/include/qdialog.h /usr/lib/qt/include/qkeycode.h
page.o: /usr/lib/qt/include/qscrbar.h /usr/lib/qt/include/qrangect.h
page.o: /usr/lib/qt/include/qdrawutl.h /usr/lib/qt/include/qfileinf.h
page.o: /usr/lib/qt/include/qdatetm.h /usr/local/kde/include/kcolordlg.h
page.o: /usr/local/kde/include/kselect.h /usr/local/kde/include/koAboutDia.h
page.o: /usr/lib/qt/include/qlayout.h /usr/lib/qt/include/qgmanagr.h
page.o: /usr/local/kde/include/kbuttonbox.h
page.o: /usr/local/kde/include/koPageLayoutDia.h
page.o: /usr/lib/qt/include/qtabdlg.h /usr/lib/qt/include/qgrpbox.h
page.o: /usr/local/kde/include/krestrictedline.h
page.o: /usr/local/kde/include/kfontdialog.h /usr/lib/qt/include/qbttngrp.h
page.o: /usr/lib/qt/include/qchkbox.h /usr/lib/qt/include/qlistbox.h
page.o: /usr/lib/qt/include/qradiobt.h /usr/lib/qt/include/qtooltip.h
page.o: kpresenter.h /usr/local/kde/idl/kom.h /usr/local/kde/idl/controls.h
page.o: /usr/local/kde/idl/parts.h kpresenter_doc.h global.h
page.o: /usr/lib/qt/include/qpicture.h /usr/lib/qt/include/qbuffer.h qwmf.h
page.o: graphobj.h atfinterpreter.h /usr/lib/qt/include/qstack.h
page.o: ktextobject.h kpresenter_main.h
page.o: /usr/local/kde/include/part_shell_impl.h kpresenter_shell.h
page.o: /usr/local/kde/include/default_shell_impl.h
page.o: /usr/local/kde/include/default_shell.h formats.h
page.o: /usr/lib/qt/include/qimage.h backdia.h afchoose.h
page.o: /usr/local/kde/include/kiconloaderdialog.h
page.o: /usr/local/kde/include/kiconloader.h styledia.h page.moc
page.o: /usr/lib/qt/include/qmetaobj.h /usr/lib/qt/include/qconnect.h
ktextobject.o: ktextobject.h /usr/lib/qt/include/qkeycode.h
ktextobject.o: /usr/lib/qt/include/qglobal.h /usr/lib/qt/include/qtablevw.h
ktextobject.o: /usr/lib/qt/include/qframe.h /usr/lib/qt/include/qwidget.h
ktextobject.o: /usr/lib/qt/include/qwindefs.h /usr/lib/qt/include/qobjdefs.h
ktextobject.o: /usr/lib/qt/include/qobject.h /usr/lib/qt/include/qstring.h
ktextobject.o: /usr/lib/qt/include/qarray.h /usr/lib/qt/include/qgarray.h
ktextobject.o: /usr/lib/qt/include/qshared.h /usr/lib/qt/include/qgeneric.h
ktextobject.o: /usr/include/string.h /usr/include/features.h
ktextobject.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
ktextobject.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
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
afchoose.o: /usr/include/gnu/stubs.h
afchoose.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
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
afchoose.o: /usr/include/_G_config.h /usr/include/gnu/types.h
afchoose.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stdarg.h
afchoose.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/va-alpha.h
afchoose.o: /usr/include/stdio_lim.h /usr/lib/qt/include/qdatetm.h
afchoose.o: /usr/local/kde/include/kiconloaderdialog.h
afchoose.o: /usr/lib/qt/include/qapp.h /usr/lib/qt/include/qstrlist.h
afchoose.o: /usr/lib/qt/include/qdstream.h /usr/lib/qt/include/qlined.h
afchoose.o: /usr/lib/qt/include/qcombo.h /usr/local/kde/include/kapp.h
afchoose.o: /usr/local/kde/include/kconfig.h
afchoose.o: /usr/local/kde/include/kconfigbase.h
afchoose.o: /usr/local/kde/include/kconfigdata.h /usr/lib/qt/include/qdict.h
afchoose.o: /usr/lib/qt/include/qgdict.h /usr/lib/qt/include/qtstream.h
afchoose.o: /usr/local/kde/include/kdebug.h /usr/local/kde/include/klocale.h
afchoose.o: /usr/lib/qt/include/qintdict.h /usr/local/kde/include/drag.h
afchoose.o: /usr/include/X11/X.h /usr/lib/qt/include/qpopmenu.h
afchoose.o: /usr/lib/qt/include/qmenudta.h
afchoose.o: /usr/local/kde/include/kiconloader.h afchoose.moc
afchoose.o: /usr/lib/qt/include/qmetaobj.h /usr/lib/qt/include/qconnect.h
styledia.o: styledia.h /usr/include/stdlib.h /usr/include/features.h
styledia.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
styledia.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
styledia.o: /usr/include/sys/types.h /usr/include/gnu/types.h
styledia.o: /usr/include/time.h /usr/include/endian.h /usr/include/bytesex.h
styledia.o: /usr/include/sys/select.h /usr/include/selectbits.h
styledia.o: /usr/include/alloca.h /usr/lib/qt/include/qdialog.h
styledia.o: /usr/lib/qt/include/qwidget.h /usr/lib/qt/include/qwindefs.h
styledia.o: /usr/lib/qt/include/qobjdefs.h /usr/lib/qt/include/qglobal.h
styledia.o: /usr/lib/qt/include/qobject.h /usr/lib/qt/include/qstring.h
styledia.o: /usr/lib/qt/include/qarray.h /usr/lib/qt/include/qgarray.h
styledia.o: /usr/lib/qt/include/qshared.h /usr/lib/qt/include/qgeneric.h
styledia.o: /usr/include/string.h /usr/lib/qt/include/qevent.h
styledia.o: /usr/lib/qt/include/qrect.h /usr/lib/qt/include/qsize.h
styledia.o: /usr/lib/qt/include/qpoint.h /usr/lib/qt/include/qpaintd.h
styledia.o: /usr/lib/qt/include/qpalette.h /usr/lib/qt/include/qcolor.h
styledia.o: /usr/lib/qt/include/qcursor.h /usr/lib/qt/include/qfont.h
styledia.o: /usr/lib/qt/include/qfontmet.h /usr/lib/qt/include/qfontinf.h
styledia.o: /usr/lib/qt/include/qlabel.h /usr/lib/qt/include/qframe.h
styledia.o: /usr/lib/qt/include/qpushbt.h /usr/lib/qt/include/qbutton.h
styledia.o: /usr/lib/qt/include/qbttngrp.h /usr/lib/qt/include/qgrpbox.h
styledia.o: /usr/lib/qt/include/qcombo.h /usr/lib/qt/include/qpen.h
styledia.o: /usr/lib/qt/include/qbrush.h /usr/lib/qt/include/qpainter.h
styledia.o: /usr/lib/qt/include/qregion.h /usr/lib/qt/include/qpntarry.h
styledia.o: /usr/lib/qt/include/qwmatrix.h /usr/local/kde/include/kcolordlg.h
styledia.o: /usr/lib/qt/include/qtablevw.h /usr/lib/qt/include/qrangect.h
styledia.o: /usr/lib/qt/include/qlined.h /usr/lib/qt/include/qpixmap.h
styledia.o: /usr/local/kde/include/kselect.h styledia.moc
styledia.o: /usr/lib/qt/include/qmetaobj.h /usr/lib/qt/include/qconnect.h
jpeg.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/features.h
jpeg.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
jpeg.o: /usr/include/_G_config.h /usr/include/gnu/types.h
jpeg.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
jpeg.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stdarg.h
jpeg.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/va-alpha.h
jpeg.o: /usr/include/stdio_lim.h /usr/include/assert.h
jpeg.o: /usr/lib/qt/include/qimage.h /usr/lib/qt/include/qpixmap.h
jpeg.o: /usr/lib/qt/include/qpaintd.h /usr/lib/qt/include/qwindefs.h
jpeg.o: /usr/lib/qt/include/qobjdefs.h /usr/lib/qt/include/qglobal.h
jpeg.o: /usr/lib/qt/include/qrect.h /usr/lib/qt/include/qsize.h
jpeg.o: /usr/lib/qt/include/qpoint.h /usr/lib/qt/include/qcolor.h
jpeg.o: /usr/lib/qt/include/qshared.h /usr/lib/qt/include/qstrlist.h
jpeg.o: /usr/lib/qt/include/qstring.h /usr/lib/qt/include/qarray.h
jpeg.o: /usr/lib/qt/include/qgarray.h /usr/lib/qt/include/qgeneric.h
jpeg.o: /usr/include/string.h /usr/lib/qt/include/qlist.h
jpeg.o: /usr/lib/qt/include/qglist.h /usr/lib/qt/include/qcollect.h
jpeg.o: /usr/lib/qt/include/qdstream.h /usr/lib/qt/include/qiodev.h jpeg.h
jpeg.o: jpeginc.h /usr/local/kde/include/jpeglib.h
jpeg.o: /usr/local/kde/include/jconfig.h /usr/local/kde/include/config.h
jpeg.o: /usr/local/kde/include/jmorecfg.h
formats.o: /usr/lib/qt/include/qimage.h /usr/lib/qt/include/qpixmap.h
formats.o: /usr/lib/qt/include/qpaintd.h /usr/lib/qt/include/qwindefs.h
formats.o: /usr/lib/qt/include/qobjdefs.h /usr/lib/qt/include/qglobal.h
formats.o: /usr/lib/qt/include/qrect.h /usr/lib/qt/include/qsize.h
formats.o: /usr/lib/qt/include/qpoint.h /usr/lib/qt/include/qcolor.h
formats.o: /usr/lib/qt/include/qshared.h /usr/lib/qt/include/qstrlist.h
formats.o: /usr/lib/qt/include/qstring.h /usr/lib/qt/include/qarray.h
formats.o: /usr/lib/qt/include/qgarray.h /usr/lib/qt/include/qgeneric.h
formats.o: /usr/include/string.h /usr/include/features.h
formats.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
formats.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
formats.o: /usr/lib/qt/include/qlist.h /usr/lib/qt/include/qglist.h
formats.o: /usr/lib/qt/include/qcollect.h /usr/lib/qt/include/qdstream.h
formats.o: /usr/lib/qt/include/qiodev.h formats.h
qwmf.o: /usr/lib/qt/include/qfile.h /usr/lib/qt/include/qiodev.h
qwmf.o: /usr/lib/qt/include/qglobal.h /usr/lib/qt/include/qstring.h
qwmf.o: /usr/lib/qt/include/qarray.h /usr/lib/qt/include/qgarray.h
qwmf.o: /usr/lib/qt/include/qshared.h /usr/lib/qt/include/qgeneric.h
qwmf.o: /usr/include/string.h /usr/include/features.h
qwmf.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
qwmf.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
qwmf.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
qwmf.o: /usr/include/gnu/types.h
qwmf.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stdarg.h
qwmf.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/va-alpha.h
qwmf.o: /usr/include/stdio_lim.h /usr/lib/qt/include/qfileinf.h
qwmf.o: /usr/lib/qt/include/qdatetm.h /usr/lib/qt/include/qpainter.h
qwmf.o: /usr/lib/qt/include/qpaintd.h /usr/lib/qt/include/qwindefs.h
qwmf.o: /usr/lib/qt/include/qobjdefs.h /usr/lib/qt/include/qrect.h
qwmf.o: /usr/lib/qt/include/qsize.h /usr/lib/qt/include/qpoint.h
qwmf.o: /usr/lib/qt/include/qcolor.h /usr/lib/qt/include/qfontmet.h
qwmf.o: /usr/lib/qt/include/qfont.h /usr/lib/qt/include/qfontinf.h
qwmf.o: /usr/lib/qt/include/qregion.h /usr/lib/qt/include/qpen.h
qwmf.o: /usr/lib/qt/include/qbrush.h /usr/lib/qt/include/qpntarry.h
qwmf.o: /usr/lib/qt/include/qwmatrix.h /usr/lib/qt/include/qdstream.h
qwmf.o: /usr/include/assert.h /usr/lib/qt/include/qapp.h
qwmf.o: /usr/lib/qt/include/qwidget.h /usr/lib/qt/include/qobject.h
qwmf.o: /usr/lib/qt/include/qevent.h /usr/lib/qt/include/qpalette.h
qwmf.o: /usr/lib/qt/include/qcursor.h /usr/include/endian.h
qwmf.o: /usr/include/bytesex.h qwmf.h /usr/lib/qt/include/qpicture.h
qwmf.o: /usr/lib/qt/include/qbuffer.h wmfstruct.h metafuncs.h
kenumlistdia.o: kenumlistdia.h /usr/include/stdio.h /usr/include/libio.h
kenumlistdia.o: /usr/include/features.h /usr/include/sys/cdefs.h
kenumlistdia.o: /usr/include/gnu/stubs.h /usr/include/_G_config.h
kenumlistdia.o: /usr/include/gnu/types.h
kenumlistdia.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
kenumlistdia.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stdarg.h
kenumlistdia.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/va-alpha.h
kenumlistdia.o: /usr/include/stdio_lim.h /usr/lib/qt/include/qdialog.h
kenumlistdia.o: /usr/lib/qt/include/qwidget.h /usr/lib/qt/include/qwindefs.h
kenumlistdia.o: /usr/lib/qt/include/qobjdefs.h /usr/lib/qt/include/qglobal.h
kenumlistdia.o: /usr/lib/qt/include/qobject.h /usr/lib/qt/include/qstring.h
kenumlistdia.o: /usr/lib/qt/include/qarray.h /usr/lib/qt/include/qgarray.h
kenumlistdia.o: /usr/lib/qt/include/qshared.h /usr/lib/qt/include/qgeneric.h
kenumlistdia.o: /usr/include/string.h /usr/lib/qt/include/qevent.h
kenumlistdia.o: /usr/lib/qt/include/qrect.h /usr/lib/qt/include/qsize.h
kenumlistdia.o: /usr/lib/qt/include/qpoint.h /usr/lib/qt/include/qpaintd.h
kenumlistdia.o: /usr/lib/qt/include/qpalette.h /usr/lib/qt/include/qcolor.h
kenumlistdia.o: /usr/lib/qt/include/qcursor.h /usr/lib/qt/include/qfont.h
kenumlistdia.o: /usr/lib/qt/include/qfontmet.h /usr/lib/qt/include/qfontinf.h
kenumlistdia.o: /usr/lib/qt/include/qlabel.h /usr/lib/qt/include/qframe.h
kenumlistdia.o: /usr/lib/qt/include/qpushbt.h /usr/lib/qt/include/qbutton.h
kenumlistdia.o: /usr/lib/qt/include/qlayout.h /usr/lib/qt/include/qgmanagr.h
kenumlistdia.o: /usr/lib/qt/include/qintdict.h /usr/lib/qt/include/qgdict.h
kenumlistdia.o: /usr/lib/qt/include/qcollect.h /usr/lib/qt/include/qlist.h
kenumlistdia.o: /usr/lib/qt/include/qglist.h /usr/lib/qt/include/qcombo.h
kenumlistdia.o: /usr/lib/qt/include/qstrlist.h /usr/lib/qt/include/qdstream.h
kenumlistdia.o: /usr/lib/qt/include/qiodev.h /usr/lib/qt/include/qchkbox.h
kenumlistdia.o: /usr/lib/qt/include/qlined.h /usr/lib/qt/include/qradiobt.h
kenumlistdia.o: /usr/local/kde/include/kapp.h
kenumlistdia.o: /usr/local/kde/include/kconfig.h
kenumlistdia.o: /usr/local/kde/include/kconfigbase.h
kenumlistdia.o: /usr/local/kde/include/kconfigdata.h
kenumlistdia.o: /usr/lib/qt/include/qdict.h /usr/lib/qt/include/qtstream.h
kenumlistdia.o: /usr/lib/qt/include/qfile.h /usr/local/kde/include/kdebug.h
kenumlistdia.o: /usr/local/kde/include/klocale.h
kenumlistdia.o: /usr/local/kde/include/drag.h /usr/lib/qt/include/qapp.h
kenumlistdia.o: /usr/lib/qt/include/qpixmap.h /usr/include/X11/X.h
kenumlistdia.o: /usr/lib/qt/include/qpopmenu.h /usr/lib/qt/include/qtablevw.h
kenumlistdia.o: /usr/lib/qt/include/qmenudta.h
kenumlistdia.o: /usr/local/kde/include/kbuttonbox.h
kenumlistdia.o: /usr/local/kde/include/kcolorbtn.h /usr/include/X11/Xlib.h
kenumlistdia.o: /usr/include/sys/types.h /usr/include/time.h
kenumlistdia.o: /usr/include/endian.h /usr/include/bytesex.h
kenumlistdia.o: /usr/include/sys/select.h /usr/include/selectbits.h
kenumlistdia.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
kenumlistdia.o: kenumlistdia.moc /usr/lib/qt/include/qmetaobj.h
kenumlistdia.o: /usr/lib/qt/include/qconnect.h
kcharselect.o: kcharselect.h /usr/include/stdio.h /usr/include/libio.h
kcharselect.o: /usr/include/features.h /usr/include/sys/cdefs.h
kcharselect.o: /usr/include/gnu/stubs.h /usr/include/_G_config.h
kcharselect.o: /usr/include/gnu/types.h
kcharselect.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
kcharselect.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stdarg.h
kcharselect.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/va-alpha.h
kcharselect.o: /usr/include/stdio_lim.h /usr/lib/qt/include/qwidget.h
kcharselect.o: /usr/lib/qt/include/qwindefs.h /usr/lib/qt/include/qobjdefs.h
kcharselect.o: /usr/lib/qt/include/qglobal.h /usr/lib/qt/include/qobject.h
kcharselect.o: /usr/lib/qt/include/qstring.h /usr/lib/qt/include/qarray.h
kcharselect.o: /usr/lib/qt/include/qgarray.h /usr/lib/qt/include/qshared.h
kcharselect.o: /usr/lib/qt/include/qgeneric.h /usr/include/string.h
kcharselect.o: /usr/lib/qt/include/qevent.h /usr/lib/qt/include/qrect.h
kcharselect.o: /usr/lib/qt/include/qsize.h /usr/lib/qt/include/qpoint.h
kcharselect.o: /usr/lib/qt/include/qpaintd.h /usr/lib/qt/include/qpalette.h
kcharselect.o: /usr/lib/qt/include/qcolor.h /usr/lib/qt/include/qcursor.h
kcharselect.o: /usr/lib/qt/include/qfont.h /usr/lib/qt/include/qfontmet.h
kcharselect.o: /usr/lib/qt/include/qfontinf.h /usr/lib/qt/include/qtablevw.h
kcharselect.o: /usr/lib/qt/include/qframe.h /usr/lib/qt/include/qpainter.h
kcharselect.o: /usr/lib/qt/include/qregion.h /usr/lib/qt/include/qpen.h
kcharselect.o: /usr/lib/qt/include/qbrush.h /usr/lib/qt/include/qpntarry.h
kcharselect.o: /usr/lib/qt/include/qwmatrix.h /usr/local/kde/include/kapp.h
kcharselect.o: /usr/local/kde/include/kconfig.h
kcharselect.o: /usr/local/kde/include/kconfigbase.h
kcharselect.o: /usr/lib/qt/include/qstrlist.h /usr/lib/qt/include/qlist.h
kcharselect.o: /usr/lib/qt/include/qglist.h /usr/lib/qt/include/qcollect.h
kcharselect.o: /usr/lib/qt/include/qdstream.h /usr/lib/qt/include/qiodev.h
kcharselect.o: /usr/local/kde/include/kconfigdata.h
kcharselect.o: /usr/lib/qt/include/qdict.h /usr/lib/qt/include/qgdict.h
kcharselect.o: /usr/lib/qt/include/qtstream.h /usr/lib/qt/include/qfile.h
kcharselect.o: /usr/local/kde/include/kdebug.h
kcharselect.o: /usr/local/kde/include/klocale.h
kcharselect.o: /usr/lib/qt/include/qintdict.h /usr/local/kde/include/drag.h
kcharselect.o: /usr/lib/qt/include/qapp.h /usr/lib/qt/include/qpixmap.h
kcharselect.o: /usr/include/X11/X.h /usr/lib/qt/include/qpopmenu.h
kcharselect.o: /usr/lib/qt/include/qmenudta.h kcharselect.moc
kcharselect.o: /usr/lib/qt/include/qmetaobj.h /usr/lib/qt/include/qconnect.h
kcharselectdia.o: kcharselectdia.h /usr/include/stdio.h /usr/include/libio.h
kcharselectdia.o: /usr/include/features.h /usr/include/sys/cdefs.h
kcharselectdia.o: /usr/include/gnu/stubs.h /usr/include/_G_config.h
kcharselectdia.o: /usr/include/gnu/types.h
kcharselectdia.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
kcharselectdia.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stdarg.h
kcharselectdia.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/va-alpha.h
kcharselectdia.o: /usr/include/stdio_lim.h /usr/lib/qt/include/qdialog.h
kcharselectdia.o: /usr/lib/qt/include/qwidget.h
kcharselectdia.o: /usr/lib/qt/include/qwindefs.h
kcharselectdia.o: /usr/lib/qt/include/qobjdefs.h
kcharselectdia.o: /usr/lib/qt/include/qglobal.h /usr/lib/qt/include/qobject.h
kcharselectdia.o: /usr/lib/qt/include/qstring.h /usr/lib/qt/include/qarray.h
kcharselectdia.o: /usr/lib/qt/include/qgarray.h /usr/lib/qt/include/qshared.h
kcharselectdia.o: /usr/lib/qt/include/qgeneric.h /usr/include/string.h
kcharselectdia.o: /usr/lib/qt/include/qevent.h /usr/lib/qt/include/qrect.h
kcharselectdia.o: /usr/lib/qt/include/qsize.h /usr/lib/qt/include/qpoint.h
kcharselectdia.o: /usr/lib/qt/include/qpaintd.h
kcharselectdia.o: /usr/lib/qt/include/qpalette.h /usr/lib/qt/include/qcolor.h
kcharselectdia.o: /usr/lib/qt/include/qcursor.h /usr/lib/qt/include/qfont.h
kcharselectdia.o: /usr/lib/qt/include/qfontmet.h
kcharselectdia.o: /usr/lib/qt/include/qfontinf.h /usr/lib/qt/include/qlabel.h
kcharselectdia.o: /usr/lib/qt/include/qframe.h /usr/lib/qt/include/qpushbt.h
kcharselectdia.o: /usr/lib/qt/include/qbutton.h /usr/lib/qt/include/qlayout.h
kcharselectdia.o: /usr/lib/qt/include/qgmanagr.h
kcharselectdia.o: /usr/lib/qt/include/qintdict.h /usr/lib/qt/include/qgdict.h
kcharselectdia.o: /usr/lib/qt/include/qcollect.h /usr/lib/qt/include/qlist.h
kcharselectdia.o: /usr/lib/qt/include/qglist.h /usr/lib/qt/include/qcombo.h
kcharselectdia.o: /usr/lib/qt/include/qstrlist.h
kcharselectdia.o: /usr/lib/qt/include/qdstream.h /usr/lib/qt/include/qiodev.h
kcharselectdia.o: /usr/lib/qt/include/qchkbox.h
kcharselectdia.o: /usr/local/kde/include/kbuttonbox.h
kcharselectdia.o: /usr/local/kde/include/kapp.h
kcharselectdia.o: /usr/local/kde/include/kconfig.h
kcharselectdia.o: /usr/local/kde/include/kconfigbase.h
kcharselectdia.o: /usr/local/kde/include/kconfigdata.h
kcharselectdia.o: /usr/lib/qt/include/qdict.h /usr/lib/qt/include/qtstream.h
kcharselectdia.o: /usr/lib/qt/include/qfile.h /usr/local/kde/include/kdebug.h
kcharselectdia.o: /usr/local/kde/include/klocale.h
kcharselectdia.o: /usr/local/kde/include/drag.h /usr/lib/qt/include/qapp.h
kcharselectdia.o: /usr/lib/qt/include/qpixmap.h /usr/include/X11/X.h
kcharselectdia.o: /usr/lib/qt/include/qpopmenu.h
kcharselectdia.o: /usr/lib/qt/include/qtablevw.h
kcharselectdia.o: /usr/lib/qt/include/qmenudta.h
kcharselectdia.o: /usr/local/kde/include/kcolorbtn.h /usr/include/X11/Xlib.h
kcharselectdia.o: /usr/include/sys/types.h /usr/include/time.h
kcharselectdia.o: /usr/include/endian.h /usr/include/bytesex.h
kcharselectdia.o: /usr/include/sys/select.h /usr/include/selectbits.h
kcharselectdia.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
kcharselectdia.o: kcharselect.h /usr/lib/qt/include/qpainter.h
kcharselectdia.o: /usr/lib/qt/include/qregion.h /usr/lib/qt/include/qpen.h
kcharselectdia.o: /usr/lib/qt/include/qbrush.h /usr/lib/qt/include/qpntarry.h
kcharselectdia.o: /usr/lib/qt/include/qwmatrix.h kcharselectdia.moc
kcharselectdia.o: /usr/lib/qt/include/qmetaobj.h
kcharselectdia.o: /usr/lib/qt/include/qconnect.h
