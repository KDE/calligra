####### General stuff

INCDIR 	= -I/usr/X11R6/include -I$(QTDIR)/include -I$(KDEDIR)/include -I../../lib/kofficeui
LDFLAGS = -s -L/usr/X11R6/lib -L/usr/lib -L$(KDEDIR)/lib -L$(QTDIR)/lib -L../../lib/kofficeui -lkdecore -lkdeui -lqt -lm -lXext -lX11 -lkofficeui -lkofficecore -lop -lkom -lkfile -lkfm
CFLAGS	=  

MOC	= moc
CC 	= gcc

####### Files

SOURCES = 	main.cc autoformEdit.cc editWidget.cc drawWidget.cc atfinterpreter.cc\
		pntInsDia.cc afchoose.cc saveAsDia.cc

HEADERS	=	autoformEdit.h editWidget.h drawWidget.h atfinterpreter.h\
		pntInsDia.h afchoose.h saveAsDia.h 

SRCMETA =	autoformEdit.moc editWidget.moc drawWidget.moc atfinterpreter.moc\
		pntInsDia.moc afchoose.moc saveAsDia.moc

OBJECTS =	main.o autoformEdit.o editWidget.o drawWidget.o atfinterpreter.o\
		pntInsDia.o afchoose.o saveAsDia.o

TARGET	=	kautoformEdit

####### Implicit rules

.SUFFIXES: .cc

.cc.o:
	$(CC) -g -c $(CFLAGS) $(DEFS) $(INCDIR) $(HELP) $<
.c.o:
	$(CC) -g -c $(CFLAGS) $(DEFS) $(INCDIR) $(HELP) $<

####### Build rules

install: all

all: symlinks $(TARGET)

symlinks:	
	ln -sf ../atfinterpreter.cc atfinterpreter.cc
	ln -sf ../atfinterpreter.h atfinterpreter.h
	ln -sf ../afchoose.cc afchoose.cc
	ln -sf ../afchoose.h afchoose.h

$(TARGET): $(SOURCES) $(SRCMETA) $(OBJECTS) $(HEADER)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

depend:
	@makedepend -I$(INCDIR) $(SOURCES) 2> /dev/null


install-exec-local:
	$(mkinstalldirs) $(bindir)
	mv autoformEdit kautoformEdit
	$(INSTALL_SCRIPT) kautoformEdit $(bindir)

clean:	
	rm -f *.moc *.o
	rm -f kautoformEdit

uninstall:
	rm -f $(bindir)/kautoformEdit


######## Meta objects 

autoformEdit.moc: autoformEdit.h
	$(MOC) autoformEdit.h -o autoformEdit.moc

editWidget.moc: editWidget.h
	$(MOC) editWidget.h -o editWidget.moc

drawWidget.moc: drawWidget.h
	$(MOC) drawWidget.h -o drawWidget.moc

atfinterpreter.moc: atfinterpreter.h
	$(MOC) atfinterpreter.h -o atfinterpreter.moc

pntInsDia.moc: pntInsDia.h
	$(MOC) pntInsDia.h -o pntInsDia.moc

ainfodia.moc: ainfodia.h
	$(MOC) ainfodia.h -o ainfodia.moc

afchoose.moc: afchoose.h
	$(MOC) afchoose.h -o afchoose.moc

saveAsDia.moc: saveAsDia.h
	$(MOC) saveAsDia.h -o saveAsDia.moc

# DO NOT DELETE

main.o: autoformEdit.h /opt/kde/include/kapp.h /opt/kde/include/kconfig.h
main.o: /opt/kde/include/kconfigbase.h /usr/lib/qt/include/qcolor.h
main.o: /usr/lib/qt/include/qwindefs.h /usr/lib/qt/include/qobjdefs.h
main.o: /usr/lib/qt/include/qglobal.h /usr/lib/qt/include/qfont.h
main.o: /usr/lib/qt/include/qstring.h /usr/lib/qt/include/qarray.h
main.o: /usr/lib/qt/include/qgarray.h /usr/lib/qt/include/qshared.h
main.o: /usr/lib/qt/include/qgeneric.h /usr/include/string.h
main.o: /usr/include/features.h /usr/include/sys/cdefs.h
main.o: /usr/lib/gcc-lib/i486-linux/2.7.2.1/include/stddef.h
main.o: /usr/lib/qt/include/qstrlist.h /usr/lib/qt/include/qlist.h
main.o: /usr/lib/qt/include/qglist.h /usr/lib/qt/include/qcollect.h
main.o: /usr/lib/qt/include/qdstream.h /usr/lib/qt/include/qiodev.h
main.o: /opt/kde/include/kconfigdata.h /usr/lib/qt/include/qdict.h
main.o: /usr/lib/qt/include/qgdict.h /usr/lib/qt/include/qtstream.h
main.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
main.o: /usr/lib/qt/include/qfile.h /usr/lib/qt/include/qobject.h
main.o: /usr/lib/qt/include/qevent.h /usr/lib/qt/include/qrect.h
main.o: /usr/lib/qt/include/qsize.h /usr/lib/qt/include/qpoint.h
main.o: /opt/kde/include/kdebug.h /opt/kde/include/klocale.h
main.o: /usr/lib/qt/include/qintdict.h /opt/kde/include/drag.h
main.o: /usr/lib/qt/include/qapp.h /usr/lib/qt/include/qwidget.h
main.o: /usr/lib/qt/include/qpaintd.h /usr/lib/qt/include/qpalette.h
main.o: /usr/lib/qt/include/qcursor.h /usr/lib/qt/include/qfontmet.h
main.o: /usr/lib/qt/include/qfontinf.h /usr/lib/qt/include/qpixmap.h
main.o: /usr/include/X11/X.h /usr/lib/qt/include/qpopmenu.h
main.o: /usr/lib/qt/include/qtablevw.h /usr/lib/qt/include/qframe.h
main.o: /usr/lib/qt/include/qmenudta.h /opt/kde/include/kmenubar.h
main.o: /usr/lib/qt/include/qmenubar.h /opt/kde/include/ktoolbar.h
main.o: /usr/lib/qt/include/qbutton.h /opt/kde/include/kcombo.h
main.o: /usr/lib/qt/include/qcombo.h /opt/kde/include/klined.h
main.o: /usr/lib/qt/include/qlined.h /opt/kde/include/kstatusbar.h
main.o: /usr/lib/qt/include/qlabel.h /opt/kde/include/ktopwidget.h
main.o: /usr/include/stdlib.h /usr/include/errno.h /usr/include/linux/errno.h
main.o: /usr/include/asm/errno.h /usr/include/alloca.h
main.o: /opt/kde/include/knewpanner.h /opt/kde/include/koAboutDia.h
main.o: /usr/lib/qt/include/qdialog.h /usr/lib/qt/include/qpushbt.h
main.o: /usr/lib/qt/include/qmsgbox.h /usr/lib/qt/include/qlayout.h
main.o: /usr/lib/qt/include/qgmanagr.h /opt/kde/include/kbuttonbox.h
main.o: /usr/lib/qt/include/qtimer.h /usr/lib/qt/include/qkeycode.h
main.o: drawWidget.h /usr/lib/qt/include/qpainter.h
main.o: /usr/lib/qt/include/qregion.h /usr/lib/qt/include/qpen.h
main.o: /usr/lib/qt/include/qbrush.h /usr/lib/qt/include/qpntarry.h
main.o: /usr/lib/qt/include/qwmatrix.h atfinterpreter.h
main.o: /usr/lib/qt/include/qstack.h editWidget.h
main.o: /usr/lib/qt/include/qtooltip.h /opt/kde/include/ktreelist.h
main.o: /opt/kde/include/kbutton.h pntInsDia.h /usr/lib/qt/include/qradiobt.h
main.o: /usr/lib/qt/include/qbttngrp.h /usr/lib/qt/include/qgrpbox.h
main.o: /opt/kde/include/kspinbox.h afchoose.h /usr/lib/qt/include/qtabdlg.h
main.o: /usr/lib/qt/include/qlistbox.h /usr/lib/qt/include/qfileinf.h
main.o: /usr/lib/qt/include/qdatetm.h /opt/kde/include/kiconloaderdialog.h
main.o: /opt/kde/include/kiconloader.h saveAsDia.h
autoformEdit.o: autoformEdit.h /opt/kde/include/kapp.h
autoformEdit.o: /opt/kde/include/kconfig.h /opt/kde/include/kconfigbase.h
autoformEdit.o: /usr/lib/qt/include/qcolor.h /usr/lib/qt/include/qwindefs.h
autoformEdit.o: /usr/lib/qt/include/qobjdefs.h /usr/lib/qt/include/qglobal.h
autoformEdit.o: /usr/lib/qt/include/qfont.h /usr/lib/qt/include/qstring.h
autoformEdit.o: /usr/lib/qt/include/qarray.h /usr/lib/qt/include/qgarray.h
autoformEdit.o: /usr/lib/qt/include/qshared.h /usr/lib/qt/include/qgeneric.h
autoformEdit.o: /usr/include/string.h /usr/include/features.h
autoformEdit.o: /usr/include/sys/cdefs.h
autoformEdit.o: /usr/lib/gcc-lib/i486-linux/2.7.2.1/include/stddef.h
autoformEdit.o: /usr/lib/qt/include/qstrlist.h /usr/lib/qt/include/qlist.h
autoformEdit.o: /usr/lib/qt/include/qglist.h /usr/lib/qt/include/qcollect.h
autoformEdit.o: /usr/lib/qt/include/qdstream.h /usr/lib/qt/include/qiodev.h
autoformEdit.o: /opt/kde/include/kconfigdata.h /usr/lib/qt/include/qdict.h
autoformEdit.o: /usr/lib/qt/include/qgdict.h /usr/lib/qt/include/qtstream.h
autoformEdit.o: /usr/include/stdio.h /usr/include/libio.h
autoformEdit.o: /usr/include/_G_config.h /usr/lib/qt/include/qfile.h
autoformEdit.o: /usr/lib/qt/include/qobject.h /usr/lib/qt/include/qevent.h
autoformEdit.o: /usr/lib/qt/include/qrect.h /usr/lib/qt/include/qsize.h
autoformEdit.o: /usr/lib/qt/include/qpoint.h /opt/kde/include/kdebug.h
autoformEdit.o: /opt/kde/include/klocale.h /usr/lib/qt/include/qintdict.h
autoformEdit.o: /opt/kde/include/drag.h /usr/lib/qt/include/qapp.h
autoformEdit.o: /usr/lib/qt/include/qwidget.h /usr/lib/qt/include/qpaintd.h
autoformEdit.o: /usr/lib/qt/include/qpalette.h /usr/lib/qt/include/qcursor.h
autoformEdit.o: /usr/lib/qt/include/qfontmet.h /usr/lib/qt/include/qfontinf.h
autoformEdit.o: /usr/lib/qt/include/qpixmap.h /usr/include/X11/X.h
autoformEdit.o: /usr/lib/qt/include/qpopmenu.h /usr/lib/qt/include/qtablevw.h
autoformEdit.o: /usr/lib/qt/include/qframe.h /usr/lib/qt/include/qmenudta.h
autoformEdit.o: /opt/kde/include/kmenubar.h /usr/lib/qt/include/qmenubar.h
autoformEdit.o: /opt/kde/include/ktoolbar.h /usr/lib/qt/include/qbutton.h
autoformEdit.o: /opt/kde/include/kcombo.h /usr/lib/qt/include/qcombo.h
autoformEdit.o: /opt/kde/include/klined.h /usr/lib/qt/include/qlined.h
autoformEdit.o: /opt/kde/include/kstatusbar.h /usr/lib/qt/include/qlabel.h
autoformEdit.o: /opt/kde/include/ktopwidget.h /usr/include/stdlib.h
autoformEdit.o: /usr/include/errno.h /usr/include/linux/errno.h
autoformEdit.o: /usr/include/asm/errno.h /usr/include/alloca.h
autoformEdit.o: /opt/kde/include/knewpanner.h /opt/kde/include/koAboutDia.h
autoformEdit.o: /usr/lib/qt/include/qdialog.h /usr/lib/qt/include/qpushbt.h
autoformEdit.o: /usr/lib/qt/include/qmsgbox.h /usr/lib/qt/include/qlayout.h
autoformEdit.o: /usr/lib/qt/include/qgmanagr.h /opt/kde/include/kbuttonbox.h
autoformEdit.o: /usr/lib/qt/include/qtimer.h /usr/lib/qt/include/qkeycode.h
autoformEdit.o: drawWidget.h /usr/lib/qt/include/qpainter.h
autoformEdit.o: /usr/lib/qt/include/qregion.h /usr/lib/qt/include/qpen.h
autoformEdit.o: /usr/lib/qt/include/qbrush.h /usr/lib/qt/include/qpntarry.h
autoformEdit.o: /usr/lib/qt/include/qwmatrix.h atfinterpreter.h
autoformEdit.o: /usr/lib/qt/include/qstack.h editWidget.h
autoformEdit.o: /usr/lib/qt/include/qtooltip.h /opt/kde/include/ktreelist.h
autoformEdit.o: /opt/kde/include/kbutton.h pntInsDia.h
autoformEdit.o: /usr/lib/qt/include/qradiobt.h /usr/lib/qt/include/qbttngrp.h
autoformEdit.o: /usr/lib/qt/include/qgrpbox.h /opt/kde/include/kspinbox.h
autoformEdit.o: afchoose.h /usr/lib/qt/include/qtabdlg.h
autoformEdit.o: /usr/lib/qt/include/qlistbox.h /usr/lib/qt/include/qfileinf.h
autoformEdit.o: /usr/lib/qt/include/qdatetm.h
autoformEdit.o: /opt/kde/include/kiconloaderdialog.h
autoformEdit.o: /opt/kde/include/kiconloader.h saveAsDia.h autoformEdit.moc
autoformEdit.o: /usr/lib/qt/include/qmetaobj.h /usr/lib/qt/include/qconnect.h
editWidget.o: editWidget.h /usr/lib/qt/include/qwidget.h
editWidget.o: /usr/lib/qt/include/qwindefs.h /usr/lib/qt/include/qobjdefs.h
editWidget.o: /usr/lib/qt/include/qglobal.h /usr/lib/qt/include/qobject.h
editWidget.o: /usr/lib/qt/include/qstring.h /usr/lib/qt/include/qarray.h
editWidget.o: /usr/lib/qt/include/qgarray.h /usr/lib/qt/include/qshared.h
editWidget.o: /usr/lib/qt/include/qgeneric.h /usr/include/string.h
editWidget.o: /usr/include/features.h /usr/include/sys/cdefs.h
editWidget.o: /usr/lib/gcc-lib/i486-linux/2.7.2.1/include/stddef.h
editWidget.o: /usr/lib/qt/include/qevent.h /usr/lib/qt/include/qrect.h
editWidget.o: /usr/lib/qt/include/qsize.h /usr/lib/qt/include/qpoint.h
editWidget.o: /usr/lib/qt/include/qpaintd.h /usr/lib/qt/include/qpalette.h
editWidget.o: /usr/lib/qt/include/qcolor.h /usr/lib/qt/include/qcursor.h
editWidget.o: /usr/lib/qt/include/qfont.h /usr/lib/qt/include/qfontmet.h
editWidget.o: /usr/lib/qt/include/qfontinf.h /usr/lib/qt/include/qlined.h
editWidget.o: /usr/lib/qt/include/qtooltip.h /usr/lib/qt/include/qtimer.h
editWidget.o: /usr/lib/qt/include/qlist.h /usr/lib/qt/include/qglist.h
editWidget.o: /usr/lib/qt/include/qcollect.h /usr/lib/qt/include/qmsgbox.h
editWidget.o: /usr/lib/qt/include/qdialog.h /opt/kde/include/ktreelist.h
editWidget.o: /usr/lib/qt/include/qapp.h /usr/lib/qt/include/qkeycode.h
editWidget.o: /usr/lib/qt/include/qpainter.h /usr/lib/qt/include/qregion.h
editWidget.o: /usr/lib/qt/include/qpen.h /usr/lib/qt/include/qbrush.h
editWidget.o: /usr/lib/qt/include/qpntarry.h /usr/lib/qt/include/qwmatrix.h
editWidget.o: /usr/lib/qt/include/qpixmap.h /usr/lib/qt/include/qstack.h
editWidget.o: /usr/lib/qt/include/qtablevw.h /usr/lib/qt/include/qframe.h
editWidget.o: /opt/kde/include/kapp.h /opt/kde/include/kconfig.h
editWidget.o: /opt/kde/include/kconfigbase.h /usr/lib/qt/include/qstrlist.h
editWidget.o: /usr/lib/qt/include/qdstream.h /usr/lib/qt/include/qiodev.h
editWidget.o: /opt/kde/include/kconfigdata.h /usr/lib/qt/include/qdict.h
editWidget.o: /usr/lib/qt/include/qgdict.h /usr/lib/qt/include/qtstream.h
editWidget.o: /usr/include/stdio.h /usr/include/libio.h
editWidget.o: /usr/include/_G_config.h /usr/lib/qt/include/qfile.h
editWidget.o: /opt/kde/include/kdebug.h /opt/kde/include/klocale.h
editWidget.o: /usr/lib/qt/include/qintdict.h /opt/kde/include/drag.h
editWidget.o: /usr/include/X11/X.h /usr/lib/qt/include/qpopmenu.h
editWidget.o: /usr/lib/qt/include/qmenudta.h /opt/kde/include/ktoolbar.h
editWidget.o: /usr/lib/qt/include/qbutton.h /opt/kde/include/kcombo.h
editWidget.o: /usr/lib/qt/include/qcombo.h /opt/kde/include/klined.h
editWidget.o: /opt/kde/include/kbutton.h atfinterpreter.h editWidget.moc
editWidget.o: /usr/lib/qt/include/qmetaobj.h /usr/lib/qt/include/qconnect.h
drawWidget.o: drawWidget.h /usr/lib/qt/include/qwidget.h
drawWidget.o: /usr/lib/qt/include/qwindefs.h /usr/lib/qt/include/qobjdefs.h
drawWidget.o: /usr/lib/qt/include/qglobal.h /usr/lib/qt/include/qobject.h
drawWidget.o: /usr/lib/qt/include/qstring.h /usr/lib/qt/include/qarray.h
drawWidget.o: /usr/lib/qt/include/qgarray.h /usr/lib/qt/include/qshared.h
drawWidget.o: /usr/lib/qt/include/qgeneric.h /usr/include/string.h
drawWidget.o: /usr/include/features.h /usr/include/sys/cdefs.h
drawWidget.o: /usr/lib/gcc-lib/i486-linux/2.7.2.1/include/stddef.h
drawWidget.o: /usr/lib/qt/include/qevent.h /usr/lib/qt/include/qrect.h
drawWidget.o: /usr/lib/qt/include/qsize.h /usr/lib/qt/include/qpoint.h
drawWidget.o: /usr/lib/qt/include/qpaintd.h /usr/lib/qt/include/qpalette.h
drawWidget.o: /usr/lib/qt/include/qcolor.h /usr/lib/qt/include/qcursor.h
drawWidget.o: /usr/lib/qt/include/qfont.h /usr/lib/qt/include/qfontmet.h
drawWidget.o: /usr/lib/qt/include/qfontinf.h /usr/lib/qt/include/qpainter.h
drawWidget.o: /usr/lib/qt/include/qregion.h /usr/lib/qt/include/qpen.h
drawWidget.o: /usr/lib/qt/include/qbrush.h /usr/lib/qt/include/qpntarry.h
drawWidget.o: /usr/lib/qt/include/qwmatrix.h /usr/lib/qt/include/qpixmap.h
drawWidget.o: atfinterpreter.h /usr/lib/qt/include/qstack.h
drawWidget.o: /usr/lib/qt/include/qglist.h /usr/lib/qt/include/qcollect.h
drawWidget.o: /usr/lib/qt/include/qlist.h /usr/lib/qt/include/qfile.h
drawWidget.o: /usr/lib/qt/include/qiodev.h /usr/include/stdio.h
drawWidget.o: /usr/include/libio.h /usr/include/_G_config.h
drawWidget.o: /usr/lib/qt/include/qstrlist.h /usr/lib/qt/include/qdstream.h
drawWidget.o: drawWidget.moc /usr/lib/qt/include/qmetaobj.h
drawWidget.o: /usr/lib/qt/include/qconnect.h
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
pntInsDia.o: pntInsDia.h /usr/lib/qt/include/qdialog.h
pntInsDia.o: /usr/lib/qt/include/qwidget.h /usr/lib/qt/include/qwindefs.h
pntInsDia.o: /usr/lib/qt/include/qobjdefs.h /usr/lib/qt/include/qglobal.h
pntInsDia.o: /usr/lib/qt/include/qobject.h /usr/lib/qt/include/qstring.h
pntInsDia.o: /usr/lib/qt/include/qarray.h /usr/lib/qt/include/qgarray.h
pntInsDia.o: /usr/lib/qt/include/qshared.h /usr/lib/qt/include/qgeneric.h
pntInsDia.o: /usr/include/string.h /usr/include/features.h
pntInsDia.o: /usr/include/sys/cdefs.h
pntInsDia.o: /usr/lib/gcc-lib/i486-linux/2.7.2.1/include/stddef.h
pntInsDia.o: /usr/lib/qt/include/qevent.h /usr/lib/qt/include/qrect.h
pntInsDia.o: /usr/lib/qt/include/qsize.h /usr/lib/qt/include/qpoint.h
pntInsDia.o: /usr/lib/qt/include/qpaintd.h /usr/lib/qt/include/qpalette.h
pntInsDia.o: /usr/lib/qt/include/qcolor.h /usr/lib/qt/include/qcursor.h
pntInsDia.o: /usr/lib/qt/include/qfont.h /usr/lib/qt/include/qfontmet.h
pntInsDia.o: /usr/lib/qt/include/qfontinf.h /usr/lib/qt/include/qlabel.h
pntInsDia.o: /usr/lib/qt/include/qframe.h /usr/lib/qt/include/qpushbt.h
pntInsDia.o: /usr/lib/qt/include/qbutton.h /usr/lib/qt/include/qradiobt.h
pntInsDia.o: /usr/lib/qt/include/qbttngrp.h /usr/lib/qt/include/qgrpbox.h
pntInsDia.o: /opt/kde/include/kspinbox.h /usr/lib/qt/include/qlined.h
pntInsDia.o: /usr/lib/qt/include/qtimer.h /usr/lib/qt/include/qstrlist.h
pntInsDia.o: /usr/lib/qt/include/qlist.h /usr/lib/qt/include/qglist.h
pntInsDia.o: /usr/lib/qt/include/qcollect.h /usr/lib/qt/include/qdstream.h
pntInsDia.o: /usr/lib/qt/include/qiodev.h pntInsDia.moc
pntInsDia.o: /usr/lib/qt/include/qmetaobj.h /usr/lib/qt/include/qconnect.h
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
saveAsDia.o: saveAsDia.h /usr/include/stdlib.h /usr/include/features.h
saveAsDia.o: /usr/include/sys/cdefs.h
saveAsDia.o: /usr/lib/gcc-lib/i486-linux/2.7.2.1/include/stddef.h
saveAsDia.o: /usr/include/errno.h /usr/include/linux/errno.h
saveAsDia.o: /usr/include/asm/errno.h /usr/include/alloca.h
saveAsDia.o: /usr/lib/qt/include/qdialog.h /usr/lib/qt/include/qwidget.h
saveAsDia.o: /usr/lib/qt/include/qwindefs.h /usr/lib/qt/include/qobjdefs.h
saveAsDia.o: /usr/lib/qt/include/qglobal.h /usr/lib/qt/include/qobject.h
saveAsDia.o: /usr/lib/qt/include/qstring.h /usr/lib/qt/include/qarray.h
saveAsDia.o: /usr/lib/qt/include/qgarray.h /usr/lib/qt/include/qshared.h
saveAsDia.o: /usr/lib/qt/include/qgeneric.h /usr/include/string.h
saveAsDia.o: /usr/lib/qt/include/qevent.h /usr/lib/qt/include/qrect.h
saveAsDia.o: /usr/lib/qt/include/qsize.h /usr/lib/qt/include/qpoint.h
saveAsDia.o: /usr/lib/qt/include/qpaintd.h /usr/lib/qt/include/qpalette.h
saveAsDia.o: /usr/lib/qt/include/qcolor.h /usr/lib/qt/include/qcursor.h
saveAsDia.o: /usr/lib/qt/include/qfont.h /usr/lib/qt/include/qfontmet.h
saveAsDia.o: /usr/lib/qt/include/qfontinf.h /usr/lib/qt/include/qlabel.h
saveAsDia.o: /usr/lib/qt/include/qframe.h /usr/lib/qt/include/qpushbt.h
saveAsDia.o: /usr/lib/qt/include/qbutton.h /usr/lib/qt/include/qstrlist.h
saveAsDia.o: /usr/lib/qt/include/qlist.h /usr/lib/qt/include/qglist.h
saveAsDia.o: /usr/lib/qt/include/qcollect.h /usr/lib/qt/include/qdstream.h
saveAsDia.o: /usr/lib/qt/include/qiodev.h /usr/lib/qt/include/qlistbox.h
saveAsDia.o: /usr/lib/qt/include/qtablevw.h /usr/lib/qt/include/qpixmap.h
saveAsDia.o: /usr/lib/qt/include/qlined.h /usr/lib/qt/include/qfile.h
saveAsDia.o: /usr/include/stdio.h /usr/include/libio.h
saveAsDia.o: /usr/include/_G_config.h /opt/kde/include/kapp.h
saveAsDia.o: /opt/kde/include/kconfig.h /opt/kde/include/kconfigbase.h
saveAsDia.o: /opt/kde/include/kconfigdata.h /usr/lib/qt/include/qdict.h
saveAsDia.o: /usr/lib/qt/include/qgdict.h /usr/lib/qt/include/qtstream.h
saveAsDia.o: /opt/kde/include/kdebug.h /opt/kde/include/klocale.h
saveAsDia.o: /usr/lib/qt/include/qintdict.h /opt/kde/include/drag.h
saveAsDia.o: /usr/lib/qt/include/qapp.h /usr/include/X11/X.h
saveAsDia.o: /usr/lib/qt/include/qpopmenu.h /usr/lib/qt/include/qmenudta.h
saveAsDia.o: saveAsDia.moc /usr/lib/qt/include/qmetaobj.h
saveAsDia.o: /usr/lib/qt/include/qconnect.h
