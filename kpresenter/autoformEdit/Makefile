####### General stuff

INCDIR 	= -I/usr/X11R6/include -I$(QTDIR)/include -I$(KDEDIR)/include -I../../lib/kofficeui
LDFLAGS = -s -L/usr/X11R6/lib -L/usr/lib -L$(KDEDIR)/lib -L$(QTDIR)/lib -L../../lib/kofficeui -lkdecore -lkdeui -lqt -lm -lXext -lX11 -lkofficeui
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

main.o: autoformEdit.h /usr/local/kde/include/kapp.h
main.o: /usr/local/kde/include/kconfig.h /usr/local/kde/include/kconfigbase.h
main.o: /usr/lib/qt/include/qcolor.h /usr/lib/qt/include/qwindefs.h
main.o: /usr/lib/qt/include/qobjdefs.h /usr/lib/qt/include/qglobal.h
main.o: /usr/lib/qt/include/qfont.h /usr/lib/qt/include/qstring.h
main.o: /usr/lib/qt/include/qarray.h /usr/lib/qt/include/qgarray.h
main.o: /usr/lib/qt/include/qshared.h /usr/lib/qt/include/qgeneric.h
main.o: /usr/include/string.h /usr/include/features.h
main.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
main.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
main.o: /usr/lib/qt/include/qstrlist.h /usr/lib/qt/include/qlist.h
main.o: /usr/lib/qt/include/qglist.h /usr/lib/qt/include/qcollect.h
main.o: /usr/lib/qt/include/qdstream.h /usr/lib/qt/include/qiodev.h
main.o: /usr/local/kde/include/kconfigdata.h /usr/lib/qt/include/qdict.h
main.o: /usr/lib/qt/include/qgdict.h /usr/lib/qt/include/qtstream.h
main.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
main.o: /usr/include/gnu/types.h
main.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stdarg.h
main.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/va-alpha.h
main.o: /usr/include/stdio_lim.h /usr/lib/qt/include/qfile.h
main.o: /usr/lib/qt/include/qobject.h /usr/lib/qt/include/qevent.h
main.o: /usr/lib/qt/include/qrect.h /usr/lib/qt/include/qsize.h
main.o: /usr/lib/qt/include/qpoint.h /usr/local/kde/include/kdebug.h
main.o: /usr/local/kde/include/klocale.h /usr/lib/qt/include/qintdict.h
main.o: /usr/local/kde/include/drag.h /usr/lib/qt/include/qapp.h
main.o: /usr/lib/qt/include/qwidget.h /usr/lib/qt/include/qpaintd.h
main.o: /usr/lib/qt/include/qpalette.h /usr/lib/qt/include/qcursor.h
main.o: /usr/lib/qt/include/qfontmet.h /usr/lib/qt/include/qfontinf.h
main.o: /usr/lib/qt/include/qpixmap.h /usr/include/X11/X.h
main.o: /usr/lib/qt/include/qpopmenu.h /usr/lib/qt/include/qtablevw.h
main.o: /usr/lib/qt/include/qframe.h /usr/lib/qt/include/qmenudta.h
main.o: /usr/local/kde/include/kmenubar.h /usr/lib/qt/include/qmenubar.h
main.o: /usr/local/kde/include/ktoolbar.h /usr/lib/qt/include/qbutton.h
main.o: /usr/local/kde/include/kcombo.h /usr/lib/qt/include/qcombo.h
main.o: /usr/local/kde/include/klined.h /usr/lib/qt/include/qlined.h
main.o: /usr/local/kde/include/kstatusbar.h /usr/lib/qt/include/qlabel.h
main.o: /usr/local/kde/include/ktopwidget.h /usr/include/stdlib.h
main.o: /usr/include/sys/types.h /usr/include/time.h /usr/include/endian.h
main.o: /usr/include/bytesex.h /usr/include/sys/select.h
main.o: /usr/include/selectbits.h /usr/include/alloca.h
main.o: /usr/local/kde/include/knewpanner.h
main.o: /usr/local/kde/include/koAboutDia.h /usr/lib/qt/include/qdialog.h
main.o: /usr/lib/qt/include/qpushbt.h /usr/lib/qt/include/qmsgbox.h
main.o: /usr/lib/qt/include/qlayout.h /usr/lib/qt/include/qgmanagr.h
main.o: /usr/local/kde/include/kbuttonbox.h /usr/lib/qt/include/qtimer.h
main.o: /usr/lib/qt/include/qkeycode.h drawWidget.h
main.o: /usr/lib/qt/include/qpainter.h /usr/lib/qt/include/qregion.h
main.o: /usr/lib/qt/include/qpen.h /usr/lib/qt/include/qbrush.h
main.o: /usr/lib/qt/include/qpntarry.h /usr/lib/qt/include/qwmatrix.h
main.o: editWidget.h /usr/lib/qt/include/qtooltip.h
main.o: /usr/local/kde/include/ktreelist.h /usr/lib/qt/include/qstack.h
main.o: /usr/local/kde/include/kbutton.h pntInsDia.h
main.o: /usr/lib/qt/include/qradiobt.h /usr/lib/qt/include/qbttngrp.h
main.o: /usr/lib/qt/include/qgrpbox.h /usr/local/kde/include/kspinbox.h
main.o: saveAsDia.h /usr/lib/qt/include/qlistbox.h
autoformEdit.o: autoformEdit.h /usr/local/kde/include/kapp.h
autoformEdit.o: /usr/local/kde/include/kconfig.h
autoformEdit.o: /usr/local/kde/include/kconfigbase.h
autoformEdit.o: /usr/lib/qt/include/qcolor.h /usr/lib/qt/include/qwindefs.h
autoformEdit.o: /usr/lib/qt/include/qobjdefs.h /usr/lib/qt/include/qglobal.h
autoformEdit.o: /usr/lib/qt/include/qfont.h /usr/lib/qt/include/qstring.h
autoformEdit.o: /usr/lib/qt/include/qarray.h /usr/lib/qt/include/qgarray.h
autoformEdit.o: /usr/lib/qt/include/qshared.h /usr/lib/qt/include/qgeneric.h
autoformEdit.o: /usr/include/string.h /usr/include/features.h
autoformEdit.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
autoformEdit.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
autoformEdit.o: /usr/lib/qt/include/qstrlist.h /usr/lib/qt/include/qlist.h
autoformEdit.o: /usr/lib/qt/include/qglist.h /usr/lib/qt/include/qcollect.h
autoformEdit.o: /usr/lib/qt/include/qdstream.h /usr/lib/qt/include/qiodev.h
autoformEdit.o: /usr/local/kde/include/kconfigdata.h
autoformEdit.o: /usr/lib/qt/include/qdict.h /usr/lib/qt/include/qgdict.h
autoformEdit.o: /usr/lib/qt/include/qtstream.h /usr/include/stdio.h
autoformEdit.o: /usr/include/libio.h /usr/include/_G_config.h
autoformEdit.o: /usr/include/gnu/types.h
autoformEdit.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stdarg.h
autoformEdit.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/va-alpha.h
autoformEdit.o: /usr/include/stdio_lim.h /usr/lib/qt/include/qfile.h
autoformEdit.o: /usr/lib/qt/include/qobject.h /usr/lib/qt/include/qevent.h
autoformEdit.o: /usr/lib/qt/include/qrect.h /usr/lib/qt/include/qsize.h
autoformEdit.o: /usr/lib/qt/include/qpoint.h /usr/local/kde/include/kdebug.h
autoformEdit.o: /usr/local/kde/include/klocale.h
autoformEdit.o: /usr/lib/qt/include/qintdict.h /usr/local/kde/include/drag.h
autoformEdit.o: /usr/lib/qt/include/qapp.h /usr/lib/qt/include/qwidget.h
autoformEdit.o: /usr/lib/qt/include/qpaintd.h /usr/lib/qt/include/qpalette.h
autoformEdit.o: /usr/lib/qt/include/qcursor.h /usr/lib/qt/include/qfontmet.h
autoformEdit.o: /usr/lib/qt/include/qfontinf.h /usr/lib/qt/include/qpixmap.h
autoformEdit.o: /usr/include/X11/X.h /usr/lib/qt/include/qpopmenu.h
autoformEdit.o: /usr/lib/qt/include/qtablevw.h /usr/lib/qt/include/qframe.h
autoformEdit.o: /usr/lib/qt/include/qmenudta.h
autoformEdit.o: /usr/local/kde/include/kmenubar.h
autoformEdit.o: /usr/lib/qt/include/qmenubar.h
autoformEdit.o: /usr/local/kde/include/ktoolbar.h
autoformEdit.o: /usr/lib/qt/include/qbutton.h /usr/local/kde/include/kcombo.h
autoformEdit.o: /usr/lib/qt/include/qcombo.h /usr/local/kde/include/klined.h
autoformEdit.o: /usr/lib/qt/include/qlined.h
autoformEdit.o: /usr/local/kde/include/kstatusbar.h
autoformEdit.o: /usr/lib/qt/include/qlabel.h
autoformEdit.o: /usr/local/kde/include/ktopwidget.h /usr/include/stdlib.h
autoformEdit.o: /usr/include/sys/types.h /usr/include/time.h
autoformEdit.o: /usr/include/endian.h /usr/include/bytesex.h
autoformEdit.o: /usr/include/sys/select.h /usr/include/selectbits.h
autoformEdit.o: /usr/include/alloca.h /usr/local/kde/include/knewpanner.h
autoformEdit.o: /usr/local/kde/include/koAboutDia.h
autoformEdit.o: /usr/lib/qt/include/qdialog.h /usr/lib/qt/include/qpushbt.h
autoformEdit.o: /usr/lib/qt/include/qmsgbox.h /usr/lib/qt/include/qlayout.h
autoformEdit.o: /usr/lib/qt/include/qgmanagr.h
autoformEdit.o: /usr/local/kde/include/kbuttonbox.h
autoformEdit.o: /usr/lib/qt/include/qtimer.h /usr/lib/qt/include/qkeycode.h
autoformEdit.o: drawWidget.h /usr/lib/qt/include/qpainter.h
autoformEdit.o: /usr/lib/qt/include/qregion.h /usr/lib/qt/include/qpen.h
autoformEdit.o: /usr/lib/qt/include/qbrush.h /usr/lib/qt/include/qpntarry.h
autoformEdit.o: /usr/lib/qt/include/qwmatrix.h editWidget.h
autoformEdit.o: /usr/lib/qt/include/qtooltip.h
autoformEdit.o: /usr/local/kde/include/ktreelist.h
autoformEdit.o: /usr/lib/qt/include/qstack.h /usr/local/kde/include/kbutton.h
autoformEdit.o: pntInsDia.h /usr/lib/qt/include/qradiobt.h
autoformEdit.o: /usr/lib/qt/include/qbttngrp.h /usr/lib/qt/include/qgrpbox.h
autoformEdit.o: /usr/local/kde/include/kspinbox.h saveAsDia.h
autoformEdit.o: /usr/lib/qt/include/qlistbox.h
editWidget.o: editWidget.h /usr/lib/qt/include/qwidget.h
editWidget.o: /usr/lib/qt/include/qwindefs.h /usr/lib/qt/include/qobjdefs.h
editWidget.o: /usr/lib/qt/include/qglobal.h /usr/lib/qt/include/qobject.h
editWidget.o: /usr/lib/qt/include/qstring.h /usr/lib/qt/include/qarray.h
editWidget.o: /usr/lib/qt/include/qgarray.h /usr/lib/qt/include/qshared.h
editWidget.o: /usr/lib/qt/include/qgeneric.h /usr/include/string.h
editWidget.o: /usr/include/features.h /usr/include/sys/cdefs.h
editWidget.o: /usr/include/gnu/stubs.h
editWidget.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
editWidget.o: /usr/lib/qt/include/qevent.h /usr/lib/qt/include/qrect.h
editWidget.o: /usr/lib/qt/include/qsize.h /usr/lib/qt/include/qpoint.h
editWidget.o: /usr/lib/qt/include/qpaintd.h /usr/lib/qt/include/qpalette.h
editWidget.o: /usr/lib/qt/include/qcolor.h /usr/lib/qt/include/qcursor.h
editWidget.o: /usr/lib/qt/include/qfont.h /usr/lib/qt/include/qfontmet.h
editWidget.o: /usr/lib/qt/include/qfontinf.h /usr/lib/qt/include/qlined.h
editWidget.o: /usr/lib/qt/include/qtooltip.h /usr/lib/qt/include/qtimer.h
editWidget.o: /usr/lib/qt/include/qlist.h /usr/lib/qt/include/qglist.h
editWidget.o: /usr/lib/qt/include/qcollect.h /usr/lib/qt/include/qmsgbox.h
editWidget.o: /usr/lib/qt/include/qdialog.h
editWidget.o: /usr/local/kde/include/ktreelist.h /usr/lib/qt/include/qapp.h
editWidget.o: /usr/lib/qt/include/qkeycode.h /usr/lib/qt/include/qpainter.h
editWidget.o: /usr/lib/qt/include/qregion.h /usr/lib/qt/include/qpen.h
editWidget.o: /usr/lib/qt/include/qbrush.h /usr/lib/qt/include/qpntarry.h
editWidget.o: /usr/lib/qt/include/qwmatrix.h /usr/lib/qt/include/qpixmap.h
editWidget.o: /usr/lib/qt/include/qstack.h /usr/lib/qt/include/qtablevw.h
editWidget.o: /usr/lib/qt/include/qframe.h /usr/local/kde/include/kapp.h
editWidget.o: /usr/local/kde/include/kconfig.h
editWidget.o: /usr/local/kde/include/kconfigbase.h
editWidget.o: /usr/lib/qt/include/qstrlist.h /usr/lib/qt/include/qdstream.h
editWidget.o: /usr/lib/qt/include/qiodev.h
editWidget.o: /usr/local/kde/include/kconfigdata.h
editWidget.o: /usr/lib/qt/include/qdict.h /usr/lib/qt/include/qgdict.h
editWidget.o: /usr/lib/qt/include/qtstream.h /usr/include/stdio.h
editWidget.o: /usr/include/libio.h /usr/include/_G_config.h
editWidget.o: /usr/include/gnu/types.h
editWidget.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stdarg.h
editWidget.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/va-alpha.h
editWidget.o: /usr/include/stdio_lim.h /usr/lib/qt/include/qfile.h
editWidget.o: /usr/local/kde/include/kdebug.h
editWidget.o: /usr/local/kde/include/klocale.h /usr/lib/qt/include/qintdict.h
editWidget.o: /usr/local/kde/include/drag.h /usr/include/X11/X.h
editWidget.o: /usr/lib/qt/include/qpopmenu.h /usr/lib/qt/include/qmenudta.h
editWidget.o: /usr/local/kde/include/ktoolbar.h /usr/lib/qt/include/qbutton.h
editWidget.o: /usr/local/kde/include/kcombo.h /usr/lib/qt/include/qcombo.h
editWidget.o: /usr/local/kde/include/klined.h
editWidget.o: /usr/local/kde/include/kbutton.h
drawWidget.o: drawWidget.h /usr/lib/qt/include/qwidget.h
drawWidget.o: /usr/lib/qt/include/qwindefs.h /usr/lib/qt/include/qobjdefs.h
drawWidget.o: /usr/lib/qt/include/qglobal.h /usr/lib/qt/include/qobject.h
drawWidget.o: /usr/lib/qt/include/qstring.h /usr/lib/qt/include/qarray.h
drawWidget.o: /usr/lib/qt/include/qgarray.h /usr/lib/qt/include/qshared.h
drawWidget.o: /usr/lib/qt/include/qgeneric.h /usr/include/string.h
drawWidget.o: /usr/include/features.h /usr/include/sys/cdefs.h
drawWidget.o: /usr/include/gnu/stubs.h
drawWidget.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
drawWidget.o: /usr/lib/qt/include/qevent.h /usr/lib/qt/include/qrect.h
drawWidget.o: /usr/lib/qt/include/qsize.h /usr/lib/qt/include/qpoint.h
drawWidget.o: /usr/lib/qt/include/qpaintd.h /usr/lib/qt/include/qpalette.h
drawWidget.o: /usr/lib/qt/include/qcolor.h /usr/lib/qt/include/qcursor.h
drawWidget.o: /usr/lib/qt/include/qfont.h /usr/lib/qt/include/qfontmet.h
drawWidget.o: /usr/lib/qt/include/qfontinf.h /usr/lib/qt/include/qpainter.h
drawWidget.o: /usr/lib/qt/include/qregion.h /usr/lib/qt/include/qpen.h
drawWidget.o: /usr/lib/qt/include/qbrush.h /usr/lib/qt/include/qpntarry.h
drawWidget.o: /usr/lib/qt/include/qwmatrix.h /usr/lib/qt/include/qpixmap.h
pntInsDia.o: pntInsDia.h /usr/lib/qt/include/qdialog.h
pntInsDia.o: /usr/lib/qt/include/qwidget.h /usr/lib/qt/include/qwindefs.h
pntInsDia.o: /usr/lib/qt/include/qobjdefs.h /usr/lib/qt/include/qglobal.h
pntInsDia.o: /usr/lib/qt/include/qobject.h /usr/lib/qt/include/qstring.h
pntInsDia.o: /usr/lib/qt/include/qarray.h /usr/lib/qt/include/qgarray.h
pntInsDia.o: /usr/lib/qt/include/qshared.h /usr/lib/qt/include/qgeneric.h
pntInsDia.o: /usr/include/string.h /usr/include/features.h
pntInsDia.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
pntInsDia.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
pntInsDia.o: /usr/lib/qt/include/qevent.h /usr/lib/qt/include/qrect.h
pntInsDia.o: /usr/lib/qt/include/qsize.h /usr/lib/qt/include/qpoint.h
pntInsDia.o: /usr/lib/qt/include/qpaintd.h /usr/lib/qt/include/qpalette.h
pntInsDia.o: /usr/lib/qt/include/qcolor.h /usr/lib/qt/include/qcursor.h
pntInsDia.o: /usr/lib/qt/include/qfont.h /usr/lib/qt/include/qfontmet.h
pntInsDia.o: /usr/lib/qt/include/qfontinf.h /usr/lib/qt/include/qlabel.h
pntInsDia.o: /usr/lib/qt/include/qframe.h /usr/lib/qt/include/qpushbt.h
pntInsDia.o: /usr/lib/qt/include/qbutton.h /usr/lib/qt/include/qradiobt.h
pntInsDia.o: /usr/lib/qt/include/qbttngrp.h /usr/lib/qt/include/qgrpbox.h
pntInsDia.o: /usr/local/kde/include/kspinbox.h /usr/lib/qt/include/qlined.h
pntInsDia.o: /usr/lib/qt/include/qtimer.h /usr/lib/qt/include/qstrlist.h
pntInsDia.o: /usr/lib/qt/include/qlist.h /usr/lib/qt/include/qglist.h
pntInsDia.o: /usr/lib/qt/include/qcollect.h /usr/lib/qt/include/qdstream.h
pntInsDia.o: /usr/lib/qt/include/qiodev.h
saveAsDia.o: saveAsDia.h /usr/include/stdlib.h /usr/include/features.h
saveAsDia.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
saveAsDia.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
saveAsDia.o: /usr/include/sys/types.h /usr/include/gnu/types.h
saveAsDia.o: /usr/include/time.h /usr/include/endian.h /usr/include/bytesex.h
saveAsDia.o: /usr/include/sys/select.h /usr/include/selectbits.h
saveAsDia.o: /usr/include/alloca.h /usr/lib/qt/include/qdialog.h
saveAsDia.o: /usr/lib/qt/include/qwidget.h /usr/lib/qt/include/qwindefs.h
saveAsDia.o: /usr/lib/qt/include/qobjdefs.h /usr/lib/qt/include/qglobal.h
saveAsDia.o: /usr/lib/qt/include/qobject.h /usr/lib/qt/include/qstring.h
saveAsDia.o: /usr/lib/qt/include/qarray.h /usr/lib/qt/include/qgarray.h
saveAsDia.o: /usr/lib/qt/include/qshared.h /usr/lib/qt/include/qgeneric.h
saveAsDia.o: /usr/include/string.h /usr/lib/qt/include/qevent.h
saveAsDia.o: /usr/lib/qt/include/qrect.h /usr/lib/qt/include/qsize.h
saveAsDia.o: /usr/lib/qt/include/qpoint.h /usr/lib/qt/include/qpaintd.h
saveAsDia.o: /usr/lib/qt/include/qpalette.h /usr/lib/qt/include/qcolor.h
saveAsDia.o: /usr/lib/qt/include/qcursor.h /usr/lib/qt/include/qfont.h
saveAsDia.o: /usr/lib/qt/include/qfontmet.h /usr/lib/qt/include/qfontinf.h
saveAsDia.o: /usr/lib/qt/include/qlabel.h /usr/lib/qt/include/qframe.h
saveAsDia.o: /usr/lib/qt/include/qpushbt.h /usr/lib/qt/include/qbutton.h
saveAsDia.o: /usr/lib/qt/include/qstrlist.h /usr/lib/qt/include/qlist.h
saveAsDia.o: /usr/lib/qt/include/qglist.h /usr/lib/qt/include/qcollect.h
saveAsDia.o: /usr/lib/qt/include/qdstream.h /usr/lib/qt/include/qiodev.h
saveAsDia.o: /usr/lib/qt/include/qlistbox.h /usr/lib/qt/include/qtablevw.h
saveAsDia.o: /usr/lib/qt/include/qpixmap.h /usr/lib/qt/include/qlined.h
saveAsDia.o: /usr/lib/qt/include/qfile.h /usr/include/stdio.h
saveAsDia.o: /usr/include/libio.h /usr/include/_G_config.h
saveAsDia.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stdarg.h
saveAsDia.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/va-alpha.h
saveAsDia.o: /usr/include/stdio_lim.h /usr/local/kde/include/kapp.h
saveAsDia.o: /usr/local/kde/include/kconfig.h
saveAsDia.o: /usr/local/kde/include/kconfigbase.h
saveAsDia.o: /usr/local/kde/include/kconfigdata.h /usr/lib/qt/include/qdict.h
saveAsDia.o: /usr/lib/qt/include/qgdict.h /usr/lib/qt/include/qtstream.h
saveAsDia.o: /usr/local/kde/include/kdebug.h /usr/local/kde/include/klocale.h
saveAsDia.o: /usr/lib/qt/include/qintdict.h /usr/local/kde/include/drag.h
saveAsDia.o: /usr/lib/qt/include/qapp.h /usr/include/X11/X.h
saveAsDia.o: /usr/lib/qt/include/qpopmenu.h /usr/lib/qt/include/qmenudta.h
