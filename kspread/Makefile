# Generated automatically from Makefile.in by configure.

####### Python

PYTHONINC =     -I./Python-1.4/Include -I./Python-1.4
PYTHON    =     ./Python-1.4

# Libraries (must be in this order!)
PYTHONLIBS =    -L$(PYTHON)/Modules \
                -L$(PYTHON)/Python \
                -L$(PYTHON)/Objects \
                -L$(PYTHON)/Parser     
#PYTHONLIBS = -L/usr/lib/python-1.4/config

INCDIR  =	-I$(QTDIR)/include -I$(KDEDIR)/include -I/usr/local/include $(PYTHONINC) -I$(KDEDIR)/idl
CXXFLAGS =	$(INCDIR) -g -fexceptions -O0
LDFLAGS =        -L$(KDEDIR)/lib -L/usr/X11/lib -lqt -lkdecore -lkdeui -lmico2.0.5 -lX11 -lXext -lkom -lop $(PYTHONLIBS) -lModules -lPython -lObjects -lParser -lkfm -lkfile -lkoml -lkofficecore -lkofficeui -lcrypt
CXX     =       c++
MOC     =       /usr/bin/moc

####### Files

SOURCES =	kmetaedit.cc kspread_emacs.cc kspread_main.cc kspread_shell.cc kspread_doc.cc kspread_dlg_paper.cc kspread_dlg_paper_headings.cc kspread_dlg_paper_border.cc kspread.cc kspread_undo.cc kspread_view.cc kspread_tabbar.cc kspread_python.cc kspread_dlg_print.cc kspread_autofill.cc kspread_layout.cc kspread_map.cc kspread_dlg_layout.cc kspread_dlg_scripts.cc kspread_cell.cc kspread_table.cc
HEADERS =	kmetaedit.h kspread_emacs.h kspread_main.cc kspread_shell.h kspread_doc.h kspread_dlg_paper.h kspread_dlg_paper_headings.h kspread_dlg_paper_border.h kspread.h kspread_undo.h kspread_view.h kspread_tabbar.h kspread_python.h kspread_dlg_print.h kspread_autofill.h kspread_layout.h kspread_map.h kspread_dlg_layout.h kspread_dlg_scripts.h kspread_table.h kspread_cell.h
OBJECTS =	kmetaedit.o kspread_emacs.o kspread_main.o kspread_shell.o kspread_doc.o kspread_dlg_paper.o kspread_dlg_paper_headings.o kspread_dlg_paper_border.o kspread.o kspread_undo.o kspread_view.o kspread_tabbar.o kspread_python.o $(PYTHON)/Modules/config.o $(PYTHON)/Modules/getpath.o kspread_dlg_print.o kspread_autofill.o kspread_layout.o kspread_map.o kspread_dlg_layout.o kspread_dlg_scripts.o kspread_table.o kspread_cell.o
SRCMETA =	kspread_main.moc kspread_shell.moc kspread_doc.moc kspread_dlg_paper.moc kspread_dlg_paper_headings.moc kspread_dlg_paper_border.moc kspread_view.moc kspread_tabbar.moc kspread_dlg_print.moc kspread_table.moc kspread_dlg_layout.moc kspread_dlg_scripts.moc kspread_part.moc
IDL	=	kspread.idl
IDLH	=	kspread.h
TARGET	=	kspread

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

$(TARGET): $(SRCMETA) $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

depend:
	@makedepend $(INCDIR) $(SOURCES) 2> /dev/null

showfiles:
	@echo $(SOURCES) $(HEADERS) Makefile

clean:
	-rm -f *.o *.bak *~ *% #*
	-rm -f $(SRCMETA) $(TARGET)

idl: kspread.idl
	idl --query-server-for-narrow -I$(KDEDIR)/idl kspread.idl

install:
	$(INSTALL_DIR) $(KDEDIR)/share/apps/koffice
	$(INSTALL_DIR) $(KDEDIR)/share/apps/koffice/partlnk
	$(INSTALL_DATA) kspread_part.kdelnk $(KDEDIR)/share/apps/koffice/partlnk/kspread.kdelnk
	$(INSTALL_DIR) $(KDEDIR)/idl
	$(INSTALL_DATA) kspread.idl $(KDEDIR)/idl
	( cd pics; make install; cd .. )
	( cd scripts; make install; cd .. )

####### Meta objects

# DO NOT DELETE THIS LINE -- make depend depends on it.

kmetaedit.o: kmetaedit.h /usr/lib/qt/include/qstring.h
kmetaedit.o: /usr/lib/qt/include/qarray.h /usr/lib/qt/include/qgarray.h
kmetaedit.o: /usr/lib/qt/include/qshared.h /usr/lib/qt/include/qglobal.h
kmetaedit.o: /usr/lib/qt/include/qgeneric.h /usr/include/string.h
kmetaedit.o: /usr/include/features.h /usr/include/sys/cdefs.h
kmetaedit.o: /usr/include/gnu/stubs.h
kmetaedit.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
kmetaedit.o: kspread_emacs.h
kspread_emacs.o: kspread_emacs.h kmetaedit.h /usr/lib/qt/include/qstring.h
kspread_emacs.o: /usr/lib/qt/include/qarray.h /usr/lib/qt/include/qgarray.h
kspread_emacs.o: /usr/lib/qt/include/qshared.h /usr/lib/qt/include/qglobal.h
kspread_emacs.o: /usr/lib/qt/include/qgeneric.h /usr/include/string.h
kspread_emacs.o: /usr/include/features.h /usr/include/sys/cdefs.h
kspread_emacs.o: /usr/include/gnu/stubs.h
kspread_emacs.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
kspread_emacs.o: /usr/include/unistd.h /usr/include/posix_opt.h
kspread_emacs.o: /usr/include/gnu/types.h /usr/include/confname.h
kspread_emacs.o: /usr/include/signal.h /usr/include/sigset.h
kspread_emacs.o: /usr/include/signum.h /usr/include/sigaction.h
kspread_emacs.o: /usr/include/sigcontext.h /usr/include/asm/sigcontext.h
kspread_emacs.o: /usr/include/linux/namecompat.h /usr/include/sys/types.h
kspread_emacs.o: /usr/include/time.h /usr/include/endian.h
kspread_emacs.o: /usr/include/bytesex.h /usr/include/sys/select.h
kspread_emacs.o: /usr/include/selectbits.h /usr/include/stdio.h
kspread_emacs.o: /usr/include/libio.h /usr/include/_G_config.h
kspread_emacs.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stdarg.h
kspread_emacs.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/va-alpha.h
kspread_emacs.o: /usr/include/stdio_lim.h /usr/include/stdlib.h
kspread_emacs.o: /usr/include/alloca.h
kspread_main.o: kspread_main.h /usr/local/kde/include/op_app.h
kspread_main.o: /usr/lib/qt/include/qapp.h /usr/lib/qt/include/qwidget.h
kspread_main.o: /usr/lib/qt/include/qwindefs.h /usr/lib/qt/include/qobjdefs.h
kspread_main.o: /usr/lib/qt/include/qglobal.h /usr/lib/qt/include/qobject.h
kspread_main.o: /usr/lib/qt/include/qstring.h /usr/lib/qt/include/qarray.h
kspread_main.o: /usr/lib/qt/include/qgarray.h /usr/lib/qt/include/qshared.h
kspread_main.o: /usr/lib/qt/include/qgeneric.h /usr/include/string.h
kspread_main.o: /usr/include/features.h /usr/include/sys/cdefs.h
kspread_main.o: /usr/include/gnu/stubs.h
kspread_main.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
kspread_main.o: /usr/lib/qt/include/qevent.h /usr/lib/qt/include/qrect.h
kspread_main.o: /usr/lib/qt/include/qsize.h /usr/lib/qt/include/qpoint.h
kspread_main.o: /usr/lib/qt/include/qpaintd.h /usr/lib/qt/include/qpalette.h
kspread_main.o: /usr/lib/qt/include/qcolor.h /usr/lib/qt/include/qcursor.h
kspread_main.o: /usr/lib/qt/include/qfont.h /usr/lib/qt/include/qfontmet.h
kspread_main.o: /usr/lib/qt/include/qfontinf.h /usr/local/kde/include/kom.h
kspread_main.o: /usr/local/include/CORBA.h /usr/local/include/mico/rtti.h
kspread_main.o: /usr/local/include/mico/config.h
kspread_main.o: /usr/local/include/mico/assert.h /usr/include/stdlib.h
kspread_main.o: /usr/include/sys/types.h /usr/include/gnu/types.h
kspread_main.o: /usr/include/time.h /usr/include/endian.h
kspread_main.o: /usr/include/bytesex.h /usr/include/sys/select.h
kspread_main.o: /usr/include/selectbits.h /usr/include/alloca.h
kspread_main.o: /usr/include/stdio.h /usr/include/libio.h
kspread_main.o: /usr/include/_G_config.h
kspread_main.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stdarg.h
kspread_main.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/va-alpha.h
kspread_main.o: /usr/include/stdio_lim.h ./Python-1.4/Include/assert.h
kspread_main.o: /usr/include/sys/time.h /usr/include/timebits.h
kspread_main.o: /usr/include/sys/socket.h /usr/include/socketbits.h
kspread_main.o: /usr/include/sockaddrcom.h /usr/include/asm/socket.h
kspread_main.o: /usr/include/asm/sockios.h /usr/local/include/ministl/string
kspread_main.o: /usr/local/include/ministl/bstring.h /usr/include/ctype.h
kspread_main.o: /usr/local/include/ministl/bool.h
kspread_main.o: /usr/local/include/ministl/vector
kspread_main.o: /usr/local/include/ministl/vector.h
kspread_main.o: /usr/local/include/ministl/ministl.h
kspread_main.o: /usr/local/include/ministl/defalloc.h
kspread_main.o: /usr/local/include/ministl/map
kspread_main.o: /usr/local/include/ministl/map.h
kspread_main.o: /usr/local/include/ministl/function
kspread_main.o: /usr/local/include/ministl/function.h
kspread_main.o: /usr/local/include/ministl/pair
kspread_main.o: /usr/local/include/ministl/pair.h
kspread_main.o: /usr/local/include/ministl/set
kspread_main.o: /usr/local/include/ministl/set.h
kspread_main.o: /usr/local/include/ministl/list
kspread_main.o: /usr/local/include/ministl/list.h
kspread_main.o: /usr/local/include/mico/version.h
kspread_main.o: /usr/local/include/mico/types.h
kspread_main.o: /usr/local/include/mico/sequence.h
kspread_main.o: /usr/local/include/mico/array.h /usr/local/include/mico/var.h
kspread_main.o: /usr/local/include/mico/basic.h
kspread_main.o: /usr/local/include/mico/address.h
kspread_main.o: /usr/local/include/mico/ior.h
kspread_main.o: /usr/local/include/mico/transport.h
kspread_main.o: /usr/local/include/mico/shlib.h
kspread_main.o: /usr/local/include/mico/process.h
kspread_main.o: /usr/local/include/mico/buffer.h
kspread_main.o: /usr/local/include/mico/codec.h
kspread_main.o: /usr/local/include/mico/string.h
kspread_main.o: /usr/local/include/mico/except.h
kspread_main.o: /usr/local/include/mico/any.h
kspread_main.o: /usr/local/include/mico/object.h
kspread_main.o: /usr/local/include/mico/boa.h
kspread_main.o: /usr/local/include/mico/orb_fwd.h
kspread_main.o: /usr/local/include/mico/dii.h /usr/local/include/mico/dsi.h
kspread_main.o: /usr/local/include/mico/dispatch.h
kspread_main.o: /usr/local/include/mico/ir.h
kspread_main.o: /usr/local/include/mico/typecode.h
kspread_main.o: /usr/local/include/mico/orb.h
kspread_main.o: /usr/local/include/mico/ioptypes.h
kspread_main.o: /usr/local/include/mico/imr.h /usr/local/include/mico/throw.h
kspread_main.o: /usr/local/include/mico/template_impl.h
kspread_main.o: /usr/local/kde/include/kom_app.h
kspread_main.o: /usr/lib/qt/include/qsocknot.h /usr/lib/qt/include/qtimer.h
kspread_main.o: /usr/lib/qt/include/qlist.h /usr/lib/qt/include/qglist.h
kspread_main.o: /usr/lib/qt/include/qcollect.h /usr/lib/qt/include/qintdict.h
kspread_main.o: /usr/lib/qt/include/qgdict.h /usr/local/kde/include/kapp.h
kspread_main.o: /usr/local/kde/include/kconfig.h
kspread_main.o: /usr/local/kde/include/kconfigbase.h
kspread_main.o: /usr/lib/qt/include/qstrlist.h /usr/lib/qt/include/qdstream.h
kspread_main.o: /usr/lib/qt/include/qiodev.h
kspread_main.o: /usr/local/kde/include/kconfigdata.h
kspread_main.o: /usr/lib/qt/include/qdict.h /usr/lib/qt/include/qtstream.h
kspread_main.o: /usr/lib/qt/include/qfile.h /usr/local/kde/include/kdebug.h
kspread_main.o: /usr/local/kde/include/klocale.h
kspread_main.o: /usr/local/kde/include/drag.h /usr/lib/qt/include/qpixmap.h
kspread_main.o: /usr/include/X11/X.h /usr/lib/qt/include/qpopmenu.h
kspread_main.o: /usr/lib/qt/include/qtablevw.h /usr/lib/qt/include/qframe.h
kspread_main.o: /usr/lib/qt/include/qmenudta.h
kspread_main.o: /usr/local/kde/include/topwidget_impl.h
kspread_main.o: /usr/local/kde/include/controls.h /opt/kde/idl/kom.h
kspread_main.o: /usr/local/kde/include/component_impl.h
kspread_main.o: /usr/local/kde/include/base_impl.h
kspread_main.o: /usr/local/kde/include/kom_var.h
kspread_main.o: /usr/local/kde/include/control_frame.h
kspread_main.o: /usr/local/kde/include/control_impl.h
kspread_main.o: /usr/local/kde/include/kwm.h /usr/include/X11/Xlib.h
kspread_main.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
kspread_main.o: /usr/local/kde/include/part_impl.h
kspread_main.o: /usr/local/kde/include/parts.h
kspread_main.o: /usr/local/kde/include/kmenubar.h
kspread_main.o: /usr/lib/qt/include/qmenubar.h
kspread_main.o: /usr/local/kde/include/ktoolbar.h
kspread_main.o: /usr/lib/qt/include/qbutton.h /usr/local/kde/include/kcombo.h
kspread_main.o: /usr/lib/qt/include/qcombo.h /usr/local/kde/include/klined.h
kspread_main.o: /usr/lib/qt/include/qlined.h
kspread_main.o: /usr/local/kde/include/part_shell_impl.h
kspread_main.o: /usr/local/kde/include/ktopwidget.h
kspread_main.o: /usr/local/kde/include/kstatusbar.h
kspread_main.o: /usr/lib/qt/include/qlabel.h
kspread_main.o: /usr/local/kde/include/toolbar_impl.h
kspread_main.o: /usr/lib/qt/include/qpushbt.h
kspread_main.o: /usr/local/kde/include/menu_impl.h kspread_doc.h
kspread_main.o: /usr/local/kde/include/part_frame_impl.h
kspread_main.o: /usr/local/kde/include/document_impl.h
kspread_main.o: /usr/local/kde/include/view_impl.h
kspread_main.o: /usr/lib/qt/include/qpainter.h /usr/lib/qt/include/qregion.h
kspread_main.o: /usr/lib/qt/include/qpen.h /usr/lib/qt/include/qbrush.h
kspread_main.o: /usr/lib/qt/include/qpntarry.h /usr/lib/qt/include/qwmatrix.h
kspread_main.o: /usr/lib/qt/include/qprinter.h kmetaedit.h kspread.h
kspread_main.o: /opt/kde/idl/controls.h /opt/kde/idl/parts.h kspread_python.h
kspread_main.o: ./Python-1.4/Include/Python.h
kspread_main.o: ./Python-1.4/Include/allobjects.h
kspread_main.o: /usr/local/kde/include/config.h /usr/include/errno.h
kspread_main.o: /usr/include/errnos.h /usr/include/linux/errno.h
kspread_main.o: /usr/include/asm/errno.h ./Python-1.4/Include/myproto.h
kspread_main.o: ./Python-1.4/Include/object.h ./Python-1.4/Include/objimpl.h
kspread_main.o: ./Python-1.4/Include/pydebug.h
kspread_main.o: ./Python-1.4/Include/accessobject.h
kspread_main.o: ./Python-1.4/Include/intobject.h
kspread_main.o: ./Python-1.4/Include/longobject.h
kspread_main.o: ./Python-1.4/Include/floatobject.h
kspread_main.o: ./Python-1.4/Include/complexobject.h
kspread_main.o: ./Python-1.4/Include/rangeobject.h
kspread_main.o: ./Python-1.4/Include/stringobject.h
kspread_main.o: ./Python-1.4/Include/tupleobject.h
kspread_main.o: ./Python-1.4/Include/listobject.h
kspread_main.o: ./Python-1.4/Include/mappingobject.h
kspread_main.o: ./Python-1.4/Include/methodobject.h
kspread_main.o: ./Python-1.4/Include/moduleobject.h
kspread_main.o: ./Python-1.4/Include/funcobject.h
kspread_main.o: ./Python-1.4/Include/classobject.h
kspread_main.o: ./Python-1.4/Include/fileobject.h
kspread_main.o: ./Python-1.4/Include/cobject.h
kspread_main.o: ./Python-1.4/Include/traceback.h
kspread_main.o: ./Python-1.4/Include/sliceobject.h
kspread_main.o: ./Python-1.4/Include/pyerrors.h
kspread_main.o: ./Python-1.4/Include/mymalloc.h
kspread_main.o: ./Python-1.4/Include/modsupport.h
kspread_main.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/varargs.h
kspread_main.o: ./Python-1.4/Include/ceval.h ./Python-1.4/Include/pythonrun.h
kspread_main.o: ./Python-1.4/Include/sysmodule.h
kspread_main.o: ./Python-1.4/Include/intrcheck.h
kspread_main.o: ./Python-1.4/Include/import.h
kspread_main.o: ./Python-1.4/Include/bltinmodule.h
kspread_main.o: ./Python-1.4/Include/abstract.h
kspread_main.o: ./Python-1.4/Include/graminit.h kspread_undo.h
kspread_main.o: kspread_layout.h /usr/local/kde/include/komlParser.h
kspread_main.o: /usr/local/kde/include/torben.h
kspread_main.o: /usr/local/kde/include/komlFeed.h kspread_cell.h
kspread_main.o: kspread_table.h kspread_autofill.h kspread_dlg_layout.h
kspread_main.o: /usr/lib/qt/include/qlistbox.h /usr/lib/qt/include/qgrpbox.h
kspread_main.o: /usr/lib/qt/include/qtabdlg.h /usr/lib/qt/include/qdialog.h
kspread_main.o: /usr/lib/qt/include/qmsgbox.h /usr/lib/qt/include/qbttngrp.h
kspread_main.o: /usr/lib/qt/include/qchkbox.h /usr/lib/qt/include/qlcdnum.h
kspread_main.o: /usr/lib/qt/include/qbitarry.h /usr/lib/qt/include/qradiobt.h
kspread_main.o: /usr/lib/qt/include/qscrbar.h /usr/lib/qt/include/qrangect.h
kspread_main.o: /usr/lib/qt/include/qdrawutl.h /usr/lib/qt/include/qtooltip.h
kspread_main.o: /usr/local/kde/include/kcolordlg.h
kspread_main.o: /usr/local/kde/include/kselect.h /usr/lib/qt/include/qstack.h
kspread_main.o: kspread_view.h kspread_tabbar.h kspread_map.h kspread_shell.h
kspread_main.o: /usr/local/kde/include/default_shell_impl.h
kspread_main.o: /usr/local/kde/include/default_shell.h
kspread_main.o: /usr/local/kde/include/koScanParts.h
kspread_main.o: /usr/local/kde/include/factory_impl.h
kspread_shell.o: kspread_shell.h /usr/local/kde/include/default_shell_impl.h
kspread_shell.o: /usr/local/kde/include/default_shell.h
kspread_shell.o: /usr/local/include/CORBA.h /usr/local/include/mico/rtti.h
kspread_shell.o: /usr/local/include/mico/config.h
kspread_shell.o: /usr/local/include/mico/assert.h /usr/include/stdlib.h
kspread_shell.o: /usr/include/features.h /usr/include/sys/cdefs.h
kspread_shell.o: /usr/include/gnu/stubs.h
kspread_shell.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
kspread_shell.o: /usr/include/sys/types.h /usr/include/gnu/types.h
kspread_shell.o: /usr/include/time.h /usr/include/endian.h
kspread_shell.o: /usr/include/bytesex.h /usr/include/sys/select.h
kspread_shell.o: /usr/include/selectbits.h /usr/include/alloca.h
kspread_shell.o: /usr/include/stdio.h /usr/include/libio.h
kspread_shell.o: /usr/include/_G_config.h
kspread_shell.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stdarg.h
kspread_shell.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/va-alpha.h
kspread_shell.o: /usr/include/stdio_lim.h ./Python-1.4/Include/assert.h
kspread_shell.o: /usr/include/sys/time.h /usr/include/timebits.h
kspread_shell.o: /usr/include/sys/socket.h /usr/include/socketbits.h
kspread_shell.o: /usr/include/sockaddrcom.h /usr/include/asm/socket.h
kspread_shell.o: /usr/include/asm/sockios.h /usr/local/include/ministl/string
kspread_shell.o: /usr/local/include/ministl/bstring.h /usr/include/ctype.h
kspread_shell.o: /usr/include/string.h /usr/local/include/ministl/bool.h
kspread_shell.o: /usr/local/include/ministl/vector
kspread_shell.o: /usr/local/include/ministl/vector.h
kspread_shell.o: /usr/local/include/ministl/ministl.h
kspread_shell.o: /usr/local/include/ministl/defalloc.h
kspread_shell.o: /usr/local/include/ministl/map
kspread_shell.o: /usr/local/include/ministl/map.h
kspread_shell.o: /usr/local/include/ministl/function
kspread_shell.o: /usr/local/include/ministl/function.h
kspread_shell.o: /usr/local/include/ministl/pair
kspread_shell.o: /usr/local/include/ministl/pair.h
kspread_shell.o: /usr/local/include/ministl/set
kspread_shell.o: /usr/local/include/ministl/set.h
kspread_shell.o: /usr/local/include/ministl/list
kspread_shell.o: /usr/local/include/ministl/list.h
kspread_shell.o: /usr/local/include/mico/version.h
kspread_shell.o: /usr/local/include/mico/types.h
kspread_shell.o: /usr/local/include/mico/sequence.h
kspread_shell.o: /usr/local/include/mico/array.h
kspread_shell.o: /usr/local/include/mico/var.h
kspread_shell.o: /usr/local/include/mico/basic.h
kspread_shell.o: /usr/local/include/mico/address.h
kspread_shell.o: /usr/local/include/mico/ior.h
kspread_shell.o: /usr/local/include/mico/transport.h
kspread_shell.o: /usr/local/include/mico/shlib.h
kspread_shell.o: /usr/local/include/mico/process.h
kspread_shell.o: /usr/local/include/mico/buffer.h
kspread_shell.o: /usr/local/include/mico/codec.h
kspread_shell.o: /usr/local/include/mico/string.h
kspread_shell.o: /usr/local/include/mico/except.h
kspread_shell.o: /usr/local/include/mico/any.h
kspread_shell.o: /usr/local/include/mico/object.h
kspread_shell.o: /usr/local/include/mico/boa.h
kspread_shell.o: /usr/local/include/mico/orb_fwd.h
kspread_shell.o: /usr/local/include/mico/dii.h /usr/local/include/mico/dsi.h
kspread_shell.o: /usr/local/include/mico/dispatch.h
kspread_shell.o: /usr/local/include/mico/ir.h
kspread_shell.o: /usr/local/include/mico/typecode.h
kspread_shell.o: /usr/local/include/mico/orb.h
kspread_shell.o: /usr/local/include/mico/ioptypes.h
kspread_shell.o: /usr/local/include/mico/imr.h
kspread_shell.o: /usr/local/include/mico/throw.h /opt/kde/idl/kom.h
kspread_shell.o: /usr/local/kde/include/controls.h
kspread_shell.o: /usr/local/include/mico/template_impl.h
kspread_shell.o: /usr/local/kde/include/parts.h
kspread_shell.o: /usr/local/kde/include/part_shell_impl.h
kspread_shell.o: /usr/local/kde/include/ktopwidget.h
kspread_shell.o: /usr/lib/qt/include/qwidget.h /usr/lib/qt/include/qwindefs.h
kspread_shell.o: /usr/lib/qt/include/qobjdefs.h /usr/lib/qt/include/qglobal.h
kspread_shell.o: /usr/lib/qt/include/qobject.h /usr/lib/qt/include/qstring.h
kspread_shell.o: /usr/lib/qt/include/qarray.h /usr/lib/qt/include/qgarray.h
kspread_shell.o: /usr/lib/qt/include/qshared.h /usr/lib/qt/include/qgeneric.h
kspread_shell.o: /usr/lib/qt/include/qevent.h /usr/lib/qt/include/qrect.h
kspread_shell.o: /usr/lib/qt/include/qsize.h /usr/lib/qt/include/qpoint.h
kspread_shell.o: /usr/lib/qt/include/qpaintd.h /usr/lib/qt/include/qpalette.h
kspread_shell.o: /usr/lib/qt/include/qcolor.h /usr/lib/qt/include/qcursor.h
kspread_shell.o: /usr/lib/qt/include/qfont.h /usr/lib/qt/include/qfontmet.h
kspread_shell.o: /usr/lib/qt/include/qfontinf.h /usr/lib/qt/include/qlist.h
kspread_shell.o: /usr/lib/qt/include/qglist.h /usr/lib/qt/include/qcollect.h
kspread_shell.o: /usr/local/kde/include/ktoolbar.h
kspread_shell.o: /usr/lib/qt/include/qframe.h /usr/lib/qt/include/qpixmap.h
kspread_shell.o: /usr/lib/qt/include/qpopmenu.h
kspread_shell.o: /usr/lib/qt/include/qtablevw.h
kspread_shell.o: /usr/lib/qt/include/qmenudta.h /usr/lib/qt/include/qbutton.h
kspread_shell.o: /usr/local/kde/include/kcombo.h /usr/lib/qt/include/qcombo.h
kspread_shell.o: /usr/local/kde/include/klined.h /usr/lib/qt/include/qlined.h
kspread_shell.o: /usr/local/kde/include/kmenubar.h
kspread_shell.o: /usr/lib/qt/include/qmenubar.h
kspread_shell.o: /usr/local/kde/include/kstatusbar.h
kspread_shell.o: /usr/lib/qt/include/qlabel.h
kspread_shell.o: /usr/local/kde/include/kconfig.h
kspread_shell.o: /usr/local/kde/include/kconfigbase.h
kspread_shell.o: /usr/lib/qt/include/qstrlist.h
kspread_shell.o: /usr/lib/qt/include/qdstream.h /usr/lib/qt/include/qiodev.h
kspread_shell.o: /usr/local/kde/include/kconfigdata.h
kspread_shell.o: /usr/lib/qt/include/qdict.h /usr/lib/qt/include/qgdict.h
kspread_shell.o: /usr/lib/qt/include/qtstream.h /usr/lib/qt/include/qfile.h
kspread_shell.o: /usr/local/kde/include/kdebug.h
kspread_shell.o: /usr/local/kde/include/part_impl.h
kspread_shell.o: /usr/local/kde/include/control_impl.h
kspread_shell.o: /usr/local/kde/include/component_impl.h
kspread_shell.o: /usr/local/kde/include/kom.h
kspread_shell.o: /usr/local/kde/include/base_impl.h
kspread_shell.o: /usr/local/kde/include/kom_var.h
kspread_shell.o: /usr/local/kde/include/toolbar_impl.h
kspread_shell.o: /usr/lib/qt/include/qpushbt.h /usr/lib/qt/include/qintdict.h
kspread_shell.o: /usr/local/kde/include/menu_impl.h
kspread_shell.o: /usr/local/kde/include/control_frame.h
kspread_shell.o: /usr/local/kde/include/kwm.h /usr/lib/qt/include/qapp.h
kspread_shell.o: /usr/include/X11/Xlib.h /usr/include/X11/X.h
kspread_shell.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
kspread_shell.o: /usr/local/kde/include/kapp.h
kspread_shell.o: /usr/local/kde/include/klocale.h
kspread_shell.o: /usr/local/kde/include/drag.h
kspread_shell.o: /usr/local/kde/include/topwidget_impl.h
kspread_shell.o: /usr/local/kde/include/document_impl.h
kspread_shell.o: /usr/local/kde/include/view_impl.h kspread_doc.h
kspread_shell.o: /usr/local/kde/include/part_frame_impl.h
kspread_shell.o: /usr/lib/qt/include/qpainter.h /usr/lib/qt/include/qregion.h
kspread_shell.o: /usr/lib/qt/include/qpen.h /usr/lib/qt/include/qbrush.h
kspread_shell.o: /usr/lib/qt/include/qpntarry.h
kspread_shell.o: /usr/lib/qt/include/qwmatrix.h
kspread_shell.o: /usr/lib/qt/include/qprinter.h kmetaedit.h kspread.h
kspread_shell.o: /opt/kde/idl/controls.h /opt/kde/idl/parts.h
kspread_shell.o: kspread_python.h ./Python-1.4/Include/Python.h
kspread_shell.o: ./Python-1.4/Include/allobjects.h
kspread_shell.o: /usr/local/kde/include/config.h /usr/include/errno.h
kspread_shell.o: /usr/include/errnos.h /usr/include/linux/errno.h
kspread_shell.o: /usr/include/asm/errno.h ./Python-1.4/Include/myproto.h
kspread_shell.o: ./Python-1.4/Include/object.h ./Python-1.4/Include/objimpl.h
kspread_shell.o: ./Python-1.4/Include/pydebug.h
kspread_shell.o: ./Python-1.4/Include/accessobject.h
kspread_shell.o: ./Python-1.4/Include/intobject.h
kspread_shell.o: ./Python-1.4/Include/longobject.h
kspread_shell.o: ./Python-1.4/Include/floatobject.h
kspread_shell.o: ./Python-1.4/Include/complexobject.h
kspread_shell.o: ./Python-1.4/Include/rangeobject.h
kspread_shell.o: ./Python-1.4/Include/stringobject.h
kspread_shell.o: ./Python-1.4/Include/tupleobject.h
kspread_shell.o: ./Python-1.4/Include/listobject.h
kspread_shell.o: ./Python-1.4/Include/mappingobject.h
kspread_shell.o: ./Python-1.4/Include/methodobject.h
kspread_shell.o: ./Python-1.4/Include/moduleobject.h
kspread_shell.o: ./Python-1.4/Include/funcobject.h
kspread_shell.o: ./Python-1.4/Include/classobject.h
kspread_shell.o: ./Python-1.4/Include/fileobject.h
kspread_shell.o: ./Python-1.4/Include/cobject.h
kspread_shell.o: ./Python-1.4/Include/traceback.h
kspread_shell.o: ./Python-1.4/Include/sliceobject.h
kspread_shell.o: ./Python-1.4/Include/pyerrors.h
kspread_shell.o: ./Python-1.4/Include/mymalloc.h
kspread_shell.o: ./Python-1.4/Include/modsupport.h
kspread_shell.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/varargs.h
kspread_shell.o: ./Python-1.4/Include/ceval.h
kspread_shell.o: ./Python-1.4/Include/pythonrun.h
kspread_shell.o: ./Python-1.4/Include/sysmodule.h
kspread_shell.o: ./Python-1.4/Include/intrcheck.h
kspread_shell.o: ./Python-1.4/Include/import.h
kspread_shell.o: ./Python-1.4/Include/bltinmodule.h
kspread_shell.o: ./Python-1.4/Include/abstract.h
kspread_shell.o: ./Python-1.4/Include/graminit.h kspread_undo.h
kspread_shell.o: kspread_layout.h /usr/local/kde/include/komlParser.h
kspread_shell.o: /usr/local/kde/include/torben.h
kspread_shell.o: /usr/local/kde/include/komlFeed.h kspread_cell.h
kspread_shell.o: kspread_table.h kspread_autofill.h kspread_dlg_layout.h
kspread_shell.o: /usr/lib/qt/include/qlistbox.h /usr/lib/qt/include/qgrpbox.h
kspread_shell.o: /usr/lib/qt/include/qtabdlg.h /usr/lib/qt/include/qdialog.h
kspread_shell.o: /usr/lib/qt/include/qmsgbox.h /usr/lib/qt/include/qbttngrp.h
kspread_shell.o: /usr/lib/qt/include/qchkbox.h /usr/lib/qt/include/qlcdnum.h
kspread_shell.o: /usr/lib/qt/include/qbitarry.h
kspread_shell.o: /usr/lib/qt/include/qradiobt.h /usr/lib/qt/include/qscrbar.h
kspread_shell.o: /usr/lib/qt/include/qrangect.h
kspread_shell.o: /usr/lib/qt/include/qdrawutl.h
kspread_shell.o: /usr/lib/qt/include/qtooltip.h /usr/lib/qt/include/qtimer.h
kspread_shell.o: /usr/local/kde/include/kcolordlg.h
kspread_shell.o: /usr/local/kde/include/kselect.h
kspread_shell.o: /usr/lib/qt/include/qstack.h kspread_view.h kspread_tabbar.h
kspread_shell.o: kspread_map.h
kspread_doc.o: kspread_doc.h /usr/local/kde/include/part_frame_impl.h
kspread_doc.o: /usr/local/kde/include/control_frame.h
kspread_doc.o: /usr/local/kde/include/kom.h /usr/local/include/CORBA.h
kspread_doc.o: /usr/local/include/mico/rtti.h
kspread_doc.o: /usr/local/include/mico/config.h
kspread_doc.o: /usr/local/include/mico/assert.h /usr/include/stdlib.h
kspread_doc.o: /usr/include/features.h /usr/include/sys/cdefs.h
kspread_doc.o: /usr/include/gnu/stubs.h
kspread_doc.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
kspread_doc.o: /usr/include/sys/types.h /usr/include/gnu/types.h
kspread_doc.o: /usr/include/time.h /usr/include/endian.h
kspread_doc.o: /usr/include/bytesex.h /usr/include/sys/select.h
kspread_doc.o: /usr/include/selectbits.h /usr/include/alloca.h
kspread_doc.o: /usr/include/stdio.h /usr/include/libio.h
kspread_doc.o: /usr/include/_G_config.h
kspread_doc.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stdarg.h
kspread_doc.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/va-alpha.h
kspread_doc.o: /usr/include/stdio_lim.h ./Python-1.4/Include/assert.h
kspread_doc.o: /usr/include/sys/time.h /usr/include/timebits.h
kspread_doc.o: /usr/include/sys/socket.h /usr/include/socketbits.h
kspread_doc.o: /usr/include/sockaddrcom.h /usr/include/asm/socket.h
kspread_doc.o: /usr/include/asm/sockios.h /usr/local/include/ministl/string
kspread_doc.o: /usr/local/include/ministl/bstring.h /usr/include/ctype.h
kspread_doc.o: /usr/include/string.h /usr/local/include/ministl/bool.h
kspread_doc.o: /usr/local/include/ministl/vector
kspread_doc.o: /usr/local/include/ministl/vector.h
kspread_doc.o: /usr/local/include/ministl/ministl.h
kspread_doc.o: /usr/local/include/ministl/defalloc.h
kspread_doc.o: /usr/local/include/ministl/map
kspread_doc.o: /usr/local/include/ministl/map.h
kspread_doc.o: /usr/local/include/ministl/function
kspread_doc.o: /usr/local/include/ministl/function.h
kspread_doc.o: /usr/local/include/ministl/pair
kspread_doc.o: /usr/local/include/ministl/pair.h
kspread_doc.o: /usr/local/include/ministl/set
kspread_doc.o: /usr/local/include/ministl/set.h
kspread_doc.o: /usr/local/include/ministl/list
kspread_doc.o: /usr/local/include/ministl/list.h
kspread_doc.o: /usr/local/include/mico/version.h
kspread_doc.o: /usr/local/include/mico/types.h
kspread_doc.o: /usr/local/include/mico/sequence.h
kspread_doc.o: /usr/local/include/mico/array.h /usr/local/include/mico/var.h
kspread_doc.o: /usr/local/include/mico/basic.h
kspread_doc.o: /usr/local/include/mico/address.h
kspread_doc.o: /usr/local/include/mico/ior.h
kspread_doc.o: /usr/local/include/mico/transport.h
kspread_doc.o: /usr/local/include/mico/shlib.h
kspread_doc.o: /usr/local/include/mico/process.h
kspread_doc.o: /usr/local/include/mico/buffer.h
kspread_doc.o: /usr/local/include/mico/codec.h
kspread_doc.o: /usr/local/include/mico/string.h
kspread_doc.o: /usr/local/include/mico/except.h /usr/local/include/mico/any.h
kspread_doc.o: /usr/local/include/mico/object.h /usr/local/include/mico/boa.h
kspread_doc.o: /usr/local/include/mico/orb_fwd.h
kspread_doc.o: /usr/local/include/mico/dii.h /usr/local/include/mico/dsi.h
kspread_doc.o: /usr/local/include/mico/dispatch.h
kspread_doc.o: /usr/local/include/mico/ir.h
kspread_doc.o: /usr/local/include/mico/typecode.h
kspread_doc.o: /usr/local/include/mico/orb.h
kspread_doc.o: /usr/local/include/mico/ioptypes.h
kspread_doc.o: /usr/local/include/mico/imr.h /usr/local/include/mico/throw.h
kspread_doc.o: /usr/local/include/mico/template_impl.h
kspread_doc.o: /usr/local/kde/include/controls.h /opt/kde/idl/kom.h
kspread_doc.o: /usr/local/kde/include/control_impl.h
kspread_doc.o: /usr/local/kde/include/component_impl.h
kspread_doc.o: /usr/local/kde/include/base_impl.h
kspread_doc.o: /usr/lib/qt/include/qstring.h /usr/lib/qt/include/qarray.h
kspread_doc.o: /usr/lib/qt/include/qgarray.h /usr/lib/qt/include/qshared.h
kspread_doc.o: /usr/lib/qt/include/qglobal.h /usr/lib/qt/include/qgeneric.h
kspread_doc.o: /usr/lib/qt/include/qlist.h /usr/lib/qt/include/qglist.h
kspread_doc.o: /usr/lib/qt/include/qcollect.h /usr/lib/qt/include/qdict.h
kspread_doc.o: /usr/lib/qt/include/qgdict.h /usr/local/kde/include/kom_var.h
kspread_doc.o: /usr/lib/qt/include/qwidget.h /usr/lib/qt/include/qwindefs.h
kspread_doc.o: /usr/lib/qt/include/qobjdefs.h /usr/lib/qt/include/qobject.h
kspread_doc.o: /usr/lib/qt/include/qevent.h /usr/lib/qt/include/qrect.h
kspread_doc.o: /usr/lib/qt/include/qsize.h /usr/lib/qt/include/qpoint.h
kspread_doc.o: /usr/lib/qt/include/qpaintd.h /usr/lib/qt/include/qpalette.h
kspread_doc.o: /usr/lib/qt/include/qcolor.h /usr/lib/qt/include/qcursor.h
kspread_doc.o: /usr/lib/qt/include/qfont.h /usr/lib/qt/include/qfontmet.h
kspread_doc.o: /usr/lib/qt/include/qfontinf.h /usr/local/kde/include/kwm.h
kspread_doc.o: /usr/lib/qt/include/qapp.h /usr/lib/qt/include/qpixmap.h
kspread_doc.o: /usr/include/X11/Xlib.h /usr/include/X11/X.h
kspread_doc.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
kspread_doc.o: /usr/local/kde/include/kapp.h /usr/local/kde/include/kconfig.h
kspread_doc.o: /usr/local/kde/include/kconfigbase.h
kspread_doc.o: /usr/lib/qt/include/qstrlist.h /usr/lib/qt/include/qdstream.h
kspread_doc.o: /usr/lib/qt/include/qiodev.h
kspread_doc.o: /usr/local/kde/include/kconfigdata.h
kspread_doc.o: /usr/lib/qt/include/qtstream.h /usr/lib/qt/include/qfile.h
kspread_doc.o: /usr/local/kde/include/kdebug.h
kspread_doc.o: /usr/local/kde/include/klocale.h
kspread_doc.o: /usr/lib/qt/include/qintdict.h /usr/local/kde/include/drag.h
kspread_doc.o: /usr/lib/qt/include/qpopmenu.h /usr/lib/qt/include/qtablevw.h
kspread_doc.o: /usr/lib/qt/include/qframe.h /usr/lib/qt/include/qmenudta.h
kspread_doc.o: /usr/local/kde/include/parts.h
kspread_doc.o: /usr/local/kde/include/part_impl.h
kspread_doc.o: /usr/local/kde/include/document_impl.h
kspread_doc.o: /usr/local/kde/include/view_impl.h
kspread_doc.o: /usr/lib/qt/include/qpainter.h /usr/lib/qt/include/qregion.h
kspread_doc.o: /usr/lib/qt/include/qpen.h /usr/lib/qt/include/qbrush.h
kspread_doc.o: /usr/lib/qt/include/qpntarry.h /usr/lib/qt/include/qwmatrix.h
kspread_doc.o: /usr/lib/qt/include/qprinter.h kmetaedit.h kspread.h
kspread_doc.o: /opt/kde/idl/controls.h /opt/kde/idl/parts.h kspread_python.h
kspread_doc.o: ./Python-1.4/Include/Python.h
kspread_doc.o: ./Python-1.4/Include/allobjects.h
kspread_doc.o: /usr/local/kde/include/config.h /usr/include/errno.h
kspread_doc.o: /usr/include/errnos.h /usr/include/linux/errno.h
kspread_doc.o: /usr/include/asm/errno.h ./Python-1.4/Include/myproto.h
kspread_doc.o: ./Python-1.4/Include/object.h ./Python-1.4/Include/objimpl.h
kspread_doc.o: ./Python-1.4/Include/pydebug.h
kspread_doc.o: ./Python-1.4/Include/accessobject.h
kspread_doc.o: ./Python-1.4/Include/intobject.h
kspread_doc.o: ./Python-1.4/Include/longobject.h
kspread_doc.o: ./Python-1.4/Include/floatobject.h
kspread_doc.o: ./Python-1.4/Include/complexobject.h
kspread_doc.o: ./Python-1.4/Include/rangeobject.h
kspread_doc.o: ./Python-1.4/Include/stringobject.h
kspread_doc.o: ./Python-1.4/Include/tupleobject.h
kspread_doc.o: ./Python-1.4/Include/listobject.h
kspread_doc.o: ./Python-1.4/Include/mappingobject.h
kspread_doc.o: ./Python-1.4/Include/methodobject.h
kspread_doc.o: ./Python-1.4/Include/moduleobject.h
kspread_doc.o: ./Python-1.4/Include/funcobject.h
kspread_doc.o: ./Python-1.4/Include/classobject.h
kspread_doc.o: ./Python-1.4/Include/fileobject.h
kspread_doc.o: ./Python-1.4/Include/cobject.h
kspread_doc.o: ./Python-1.4/Include/traceback.h
kspread_doc.o: ./Python-1.4/Include/sliceobject.h
kspread_doc.o: ./Python-1.4/Include/pyerrors.h
kspread_doc.o: ./Python-1.4/Include/mymalloc.h
kspread_doc.o: ./Python-1.4/Include/modsupport.h
kspread_doc.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/varargs.h
kspread_doc.o: ./Python-1.4/Include/ceval.h ./Python-1.4/Include/pythonrun.h
kspread_doc.o: ./Python-1.4/Include/sysmodule.h
kspread_doc.o: ./Python-1.4/Include/intrcheck.h ./Python-1.4/Include/import.h
kspread_doc.o: ./Python-1.4/Include/bltinmodule.h
kspread_doc.o: ./Python-1.4/Include/abstract.h
kspread_doc.o: ./Python-1.4/Include/graminit.h kspread_undo.h
kspread_doc.o: kspread_layout.h /usr/local/kde/include/komlParser.h
kspread_doc.o: /usr/local/kde/include/torben.h
kspread_doc.o: /usr/local/kde/include/komlFeed.h kspread_cell.h
kspread_doc.o: kspread_table.h kspread_autofill.h kspread_dlg_layout.h
kspread_doc.o: /usr/lib/qt/include/qlistbox.h /usr/lib/qt/include/qlined.h
kspread_doc.o: /usr/lib/qt/include/qcombo.h /usr/lib/qt/include/qlabel.h
kspread_doc.o: /usr/lib/qt/include/qgrpbox.h /usr/lib/qt/include/qtabdlg.h
kspread_doc.o: /usr/lib/qt/include/qdialog.h /usr/lib/qt/include/qmsgbox.h
kspread_doc.o: /usr/lib/qt/include/qbttngrp.h /usr/lib/qt/include/qchkbox.h
kspread_doc.o: /usr/lib/qt/include/qbutton.h /usr/lib/qt/include/qlcdnum.h
kspread_doc.o: /usr/lib/qt/include/qbitarry.h /usr/lib/qt/include/qpushbt.h
kspread_doc.o: /usr/lib/qt/include/qradiobt.h /usr/lib/qt/include/qscrbar.h
kspread_doc.o: /usr/lib/qt/include/qrangect.h /usr/lib/qt/include/qdrawutl.h
kspread_doc.o: /usr/lib/qt/include/qtooltip.h /usr/lib/qt/include/qtimer.h
kspread_doc.o: /usr/local/kde/include/kcolordlg.h
kspread_doc.o: /usr/local/kde/include/kselect.h /usr/lib/qt/include/qstack.h
kspread_doc.o: kspread_view.h /usr/local/kde/include/menu_impl.h
kspread_doc.o: /usr/local/kde/include/kmenubar.h
kspread_doc.o: /usr/lib/qt/include/qmenubar.h
kspread_doc.o: /usr/local/kde/include/ktopwidget.h
kspread_doc.o: /usr/local/kde/include/ktoolbar.h
kspread_doc.o: /usr/local/kde/include/kcombo.h
kspread_doc.o: /usr/local/kde/include/klined.h
kspread_doc.o: /usr/local/kde/include/kstatusbar.h
kspread_doc.o: /usr/local/kde/include/toolbar_impl.h kspread_tabbar.h
kspread_doc.o: kspread_map.h /usr/local/kde/include/komlStreamFeed.h
kspread_doc.o: /usr/local/kde/include/komlWriter.h /usr/include/unistd.h
kspread_doc.o: /usr/include/posix_opt.h /usr/include/confname.h
kspread_doc.o: /usr/local/kde/include/kurl.h /usr/lib/qt/include/qdatetm.h
kspread_dlg_paper.o: kspread_dlg_paper.h /usr/include/stdlib.h
kspread_dlg_paper.o: /usr/include/features.h /usr/include/sys/cdefs.h
kspread_dlg_paper.o: /usr/include/gnu/stubs.h
kspread_dlg_paper.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
kspread_dlg_paper.o: /usr/include/sys/types.h /usr/include/gnu/types.h
kspread_dlg_paper.o: /usr/include/time.h /usr/include/endian.h
kspread_dlg_paper.o: /usr/include/bytesex.h /usr/include/sys/select.h
kspread_dlg_paper.o: /usr/include/selectbits.h /usr/include/alloca.h
kspread_dlg_paper.o: /usr/include/stdio.h /usr/include/libio.h
kspread_dlg_paper.o: /usr/include/_G_config.h
kspread_dlg_paper.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stdarg.h
kspread_dlg_paper.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/va-alpha.h
kspread_dlg_paper.o: /usr/include/stdio_lim.h /usr/lib/qt/include/qtabdlg.h
kspread_dlg_paper.o: /usr/lib/qt/include/qdialog.h
kspread_dlg_paper.o: /usr/lib/qt/include/qwidget.h
kspread_dlg_paper.o: /usr/lib/qt/include/qwindefs.h
kspread_dlg_paper.o: /usr/lib/qt/include/qobjdefs.h
kspread_dlg_paper.o: /usr/lib/qt/include/qglobal.h
kspread_dlg_paper.o: /usr/lib/qt/include/qobject.h
kspread_dlg_paper.o: /usr/lib/qt/include/qstring.h
kspread_dlg_paper.o: /usr/lib/qt/include/qarray.h
kspread_dlg_paper.o: /usr/lib/qt/include/qgarray.h
kspread_dlg_paper.o: /usr/lib/qt/include/qshared.h
kspread_dlg_paper.o: /usr/lib/qt/include/qgeneric.h /usr/include/string.h
kspread_dlg_paper.o: /usr/lib/qt/include/qevent.h /usr/lib/qt/include/qrect.h
kspread_dlg_paper.o: /usr/lib/qt/include/qsize.h /usr/lib/qt/include/qpoint.h
kspread_dlg_paper.o: /usr/lib/qt/include/qpaintd.h
kspread_dlg_paper.o: /usr/lib/qt/include/qpalette.h
kspread_dlg_paper.o: /usr/lib/qt/include/qcolor.h
kspread_dlg_paper.o: /usr/lib/qt/include/qcursor.h
kspread_dlg_paper.o: /usr/lib/qt/include/qfont.h
kspread_dlg_paper.o: /usr/lib/qt/include/qfontmet.h
kspread_dlg_paper.o: /usr/lib/qt/include/qfontinf.h
kspread_dlg_paper.o: kspread_dlg_paper_border.h /usr/lib/qt/include/qlined.h
kspread_dlg_paper.o: /usr/lib/qt/include/qradiobt.h
kspread_dlg_paper.o: /usr/lib/qt/include/qbutton.h
kspread_dlg_paper.o: /usr/lib/qt/include/qbttngrp.h
kspread_dlg_paper.o: /usr/lib/qt/include/qgrpbox.h
kspread_dlg_paper.o: /usr/lib/qt/include/qframe.h
kspread_dlg_paper.o: /usr/lib/qt/include/qlabel.h
kspread_dlg_paper.o: kspread_dlg_paper_headings.h
kspread_dlg_paper_headings.o: kspread_dlg_paper_headings.h
kspread_dlg_paper_headings.o: /usr/lib/qt/include/qwidget.h
kspread_dlg_paper_headings.o: /usr/lib/qt/include/qwindefs.h
kspread_dlg_paper_headings.o: /usr/lib/qt/include/qobjdefs.h
kspread_dlg_paper_headings.o: /usr/lib/qt/include/qglobal.h
kspread_dlg_paper_headings.o: /usr/lib/qt/include/qobject.h
kspread_dlg_paper_headings.o: /usr/lib/qt/include/qstring.h
kspread_dlg_paper_headings.o: /usr/lib/qt/include/qarray.h
kspread_dlg_paper_headings.o: /usr/lib/qt/include/qgarray.h
kspread_dlg_paper_headings.o: /usr/lib/qt/include/qshared.h
kspread_dlg_paper_headings.o: /usr/lib/qt/include/qgeneric.h
kspread_dlg_paper_headings.o: /usr/include/string.h /usr/include/features.h
kspread_dlg_paper_headings.o: /usr/include/sys/cdefs.h
kspread_dlg_paper_headings.o: /usr/include/gnu/stubs.h
kspread_dlg_paper_headings.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
kspread_dlg_paper_headings.o: /usr/lib/qt/include/qevent.h
kspread_dlg_paper_headings.o: /usr/lib/qt/include/qrect.h
kspread_dlg_paper_headings.o: /usr/lib/qt/include/qsize.h
kspread_dlg_paper_headings.o: /usr/lib/qt/include/qpoint.h
kspread_dlg_paper_headings.o: /usr/lib/qt/include/qpaintd.h
kspread_dlg_paper_headings.o: /usr/lib/qt/include/qpalette.h
kspread_dlg_paper_headings.o: /usr/lib/qt/include/qcolor.h
kspread_dlg_paper_headings.o: /usr/lib/qt/include/qcursor.h
kspread_dlg_paper_headings.o: /usr/lib/qt/include/qfont.h
kspread_dlg_paper_headings.o: /usr/lib/qt/include/qfontmet.h
kspread_dlg_paper_headings.o: /usr/lib/qt/include/qfontinf.h
kspread_dlg_paper_headings.o: /usr/lib/qt/include/qlined.h
kspread_dlg_paper_headings.o: /usr/lib/qt/include/qlabel.h
kspread_dlg_paper_headings.o: /usr/lib/qt/include/qframe.h
kspread_dlg_paper_border.o: kspread_dlg_paper_border.h
kspread_dlg_paper_border.o: /usr/lib/qt/include/qwidget.h
kspread_dlg_paper_border.o: /usr/lib/qt/include/qwindefs.h
kspread_dlg_paper_border.o: /usr/lib/qt/include/qobjdefs.h
kspread_dlg_paper_border.o: /usr/lib/qt/include/qglobal.h
kspread_dlg_paper_border.o: /usr/lib/qt/include/qobject.h
kspread_dlg_paper_border.o: /usr/lib/qt/include/qstring.h
kspread_dlg_paper_border.o: /usr/lib/qt/include/qarray.h
kspread_dlg_paper_border.o: /usr/lib/qt/include/qgarray.h
kspread_dlg_paper_border.o: /usr/lib/qt/include/qshared.h
kspread_dlg_paper_border.o: /usr/lib/qt/include/qgeneric.h
kspread_dlg_paper_border.o: /usr/include/string.h /usr/include/features.h
kspread_dlg_paper_border.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
kspread_dlg_paper_border.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
kspread_dlg_paper_border.o: /usr/lib/qt/include/qevent.h
kspread_dlg_paper_border.o: /usr/lib/qt/include/qrect.h
kspread_dlg_paper_border.o: /usr/lib/qt/include/qsize.h
kspread_dlg_paper_border.o: /usr/lib/qt/include/qpoint.h
kspread_dlg_paper_border.o: /usr/lib/qt/include/qpaintd.h
kspread_dlg_paper_border.o: /usr/lib/qt/include/qpalette.h
kspread_dlg_paper_border.o: /usr/lib/qt/include/qcolor.h
kspread_dlg_paper_border.o: /usr/lib/qt/include/qcursor.h
kspread_dlg_paper_border.o: /usr/lib/qt/include/qfont.h
kspread_dlg_paper_border.o: /usr/lib/qt/include/qfontmet.h
kspread_dlg_paper_border.o: /usr/lib/qt/include/qfontinf.h
kspread_dlg_paper_border.o: /usr/lib/qt/include/qlined.h
kspread_dlg_paper_border.o: /usr/lib/qt/include/qradiobt.h
kspread_dlg_paper_border.o: /usr/lib/qt/include/qbutton.h
kspread_dlg_paper_border.o: /usr/lib/qt/include/qbttngrp.h
kspread_dlg_paper_border.o: /usr/lib/qt/include/qgrpbox.h
kspread_dlg_paper_border.o: /usr/lib/qt/include/qframe.h
kspread_dlg_paper_border.o: /usr/lib/qt/include/qlabel.h
kspread.o: kspread.h /usr/local/include/CORBA.h
kspread.o: /usr/local/include/mico/rtti.h /usr/local/include/mico/config.h
kspread.o: /usr/local/include/mico/assert.h /usr/include/stdlib.h
kspread.o: /usr/include/features.h /usr/include/sys/cdefs.h
kspread.o: /usr/include/gnu/stubs.h
kspread.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
kspread.o: /usr/include/sys/types.h /usr/include/gnu/types.h
kspread.o: /usr/include/time.h /usr/include/endian.h /usr/include/bytesex.h
kspread.o: /usr/include/sys/select.h /usr/include/selectbits.h
kspread.o: /usr/include/alloca.h /usr/include/stdio.h /usr/include/libio.h
kspread.o: /usr/include/_G_config.h
kspread.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stdarg.h
kspread.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/va-alpha.h
kspread.o: /usr/include/stdio_lim.h ./Python-1.4/Include/assert.h
kspread.o: /usr/include/sys/time.h /usr/include/timebits.h
kspread.o: /usr/include/sys/socket.h /usr/include/socketbits.h
kspread.o: /usr/include/sockaddrcom.h /usr/include/asm/socket.h
kspread.o: /usr/include/asm/sockios.h /usr/local/include/ministl/string
kspread.o: /usr/local/include/ministl/bstring.h /usr/include/ctype.h
kspread.o: /usr/include/string.h /usr/local/include/ministl/bool.h
kspread.o: /usr/local/include/ministl/vector
kspread.o: /usr/local/include/ministl/vector.h
kspread.o: /usr/local/include/ministl/ministl.h
kspread.o: /usr/local/include/ministl/defalloc.h
kspread.o: /usr/local/include/ministl/map /usr/local/include/ministl/map.h
kspread.o: /usr/local/include/ministl/function
kspread.o: /usr/local/include/ministl/function.h
kspread.o: /usr/local/include/ministl/pair /usr/local/include/ministl/pair.h
kspread.o: /usr/local/include/ministl/set /usr/local/include/ministl/set.h
kspread.o: /usr/local/include/ministl/list /usr/local/include/ministl/list.h
kspread.o: /usr/local/include/mico/version.h /usr/local/include/mico/types.h
kspread.o: /usr/local/include/mico/sequence.h /usr/local/include/mico/array.h
kspread.o: /usr/local/include/mico/var.h /usr/local/include/mico/basic.h
kspread.o: /usr/local/include/mico/address.h /usr/local/include/mico/ior.h
kspread.o: /usr/local/include/mico/transport.h
kspread.o: /usr/local/include/mico/shlib.h /usr/local/include/mico/process.h
kspread.o: /usr/local/include/mico/buffer.h /usr/local/include/mico/codec.h
kspread.o: /usr/local/include/mico/string.h /usr/local/include/mico/except.h
kspread.o: /usr/local/include/mico/any.h /usr/local/include/mico/object.h
kspread.o: /usr/local/include/mico/boa.h /usr/local/include/mico/orb_fwd.h
kspread.o: /usr/local/include/mico/dii.h /usr/local/include/mico/dsi.h
kspread.o: /usr/local/include/mico/dispatch.h /usr/local/include/mico/ir.h
kspread.o: /usr/local/include/mico/typecode.h /usr/local/include/mico/orb.h
kspread.o: /usr/local/include/mico/ioptypes.h /usr/local/include/mico/imr.h
kspread.o: /usr/local/include/mico/throw.h /opt/kde/idl/kom.h
kspread.o: /opt/kde/idl/controls.h /opt/kde/idl/parts.h
kspread.o: /usr/local/include/mico/template_impl.h
kspread_undo.o: kspread_undo.h kspread_doc.h
kspread_undo.o: /usr/local/kde/include/part_frame_impl.h
kspread_undo.o: /usr/local/kde/include/control_frame.h
kspread_undo.o: /usr/local/kde/include/kom.h /usr/local/include/CORBA.h
kspread_undo.o: /usr/local/include/mico/rtti.h
kspread_undo.o: /usr/local/include/mico/config.h
kspread_undo.o: /usr/local/include/mico/assert.h /usr/include/stdlib.h
kspread_undo.o: /usr/include/features.h /usr/include/sys/cdefs.h
kspread_undo.o: /usr/include/gnu/stubs.h
kspread_undo.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
kspread_undo.o: /usr/include/sys/types.h /usr/include/gnu/types.h
kspread_undo.o: /usr/include/time.h /usr/include/endian.h
kspread_undo.o: /usr/include/bytesex.h /usr/include/sys/select.h
kspread_undo.o: /usr/include/selectbits.h /usr/include/alloca.h
kspread_undo.o: /usr/include/stdio.h /usr/include/libio.h
kspread_undo.o: /usr/include/_G_config.h
kspread_undo.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stdarg.h
kspread_undo.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/va-alpha.h
kspread_undo.o: /usr/include/stdio_lim.h ./Python-1.4/Include/assert.h
kspread_undo.o: /usr/include/sys/time.h /usr/include/timebits.h
kspread_undo.o: /usr/include/sys/socket.h /usr/include/socketbits.h
kspread_undo.o: /usr/include/sockaddrcom.h /usr/include/asm/socket.h
kspread_undo.o: /usr/include/asm/sockios.h /usr/local/include/ministl/string
kspread_undo.o: /usr/local/include/ministl/bstring.h /usr/include/ctype.h
kspread_undo.o: /usr/include/string.h /usr/local/include/ministl/bool.h
kspread_undo.o: /usr/local/include/ministl/vector
kspread_undo.o: /usr/local/include/ministl/vector.h
kspread_undo.o: /usr/local/include/ministl/ministl.h
kspread_undo.o: /usr/local/include/ministl/defalloc.h
kspread_undo.o: /usr/local/include/ministl/map
kspread_undo.o: /usr/local/include/ministl/map.h
kspread_undo.o: /usr/local/include/ministl/function
kspread_undo.o: /usr/local/include/ministl/function.h
kspread_undo.o: /usr/local/include/ministl/pair
kspread_undo.o: /usr/local/include/ministl/pair.h
kspread_undo.o: /usr/local/include/ministl/set
kspread_undo.o: /usr/local/include/ministl/set.h
kspread_undo.o: /usr/local/include/ministl/list
kspread_undo.o: /usr/local/include/ministl/list.h
kspread_undo.o: /usr/local/include/mico/version.h
kspread_undo.o: /usr/local/include/mico/types.h
kspread_undo.o: /usr/local/include/mico/sequence.h
kspread_undo.o: /usr/local/include/mico/array.h /usr/local/include/mico/var.h
kspread_undo.o: /usr/local/include/mico/basic.h
kspread_undo.o: /usr/local/include/mico/address.h
kspread_undo.o: /usr/local/include/mico/ior.h
kspread_undo.o: /usr/local/include/mico/transport.h
kspread_undo.o: /usr/local/include/mico/shlib.h
kspread_undo.o: /usr/local/include/mico/process.h
kspread_undo.o: /usr/local/include/mico/buffer.h
kspread_undo.o: /usr/local/include/mico/codec.h
kspread_undo.o: /usr/local/include/mico/string.h
kspread_undo.o: /usr/local/include/mico/except.h
kspread_undo.o: /usr/local/include/mico/any.h
kspread_undo.o: /usr/local/include/mico/object.h
kspread_undo.o: /usr/local/include/mico/boa.h
kspread_undo.o: /usr/local/include/mico/orb_fwd.h
kspread_undo.o: /usr/local/include/mico/dii.h /usr/local/include/mico/dsi.h
kspread_undo.o: /usr/local/include/mico/dispatch.h
kspread_undo.o: /usr/local/include/mico/ir.h
kspread_undo.o: /usr/local/include/mico/typecode.h
kspread_undo.o: /usr/local/include/mico/orb.h
kspread_undo.o: /usr/local/include/mico/ioptypes.h
kspread_undo.o: /usr/local/include/mico/imr.h /usr/local/include/mico/throw.h
kspread_undo.o: /usr/local/include/mico/template_impl.h
kspread_undo.o: /usr/local/kde/include/controls.h /opt/kde/idl/kom.h
kspread_undo.o: /usr/local/kde/include/control_impl.h
kspread_undo.o: /usr/local/kde/include/component_impl.h
kspread_undo.o: /usr/local/kde/include/base_impl.h
kspread_undo.o: /usr/lib/qt/include/qstring.h /usr/lib/qt/include/qarray.h
kspread_undo.o: /usr/lib/qt/include/qgarray.h /usr/lib/qt/include/qshared.h
kspread_undo.o: /usr/lib/qt/include/qglobal.h /usr/lib/qt/include/qgeneric.h
kspread_undo.o: /usr/lib/qt/include/qlist.h /usr/lib/qt/include/qglist.h
kspread_undo.o: /usr/lib/qt/include/qcollect.h /usr/lib/qt/include/qdict.h
kspread_undo.o: /usr/lib/qt/include/qgdict.h /usr/local/kde/include/kom_var.h
kspread_undo.o: /usr/lib/qt/include/qwidget.h /usr/lib/qt/include/qwindefs.h
kspread_undo.o: /usr/lib/qt/include/qobjdefs.h /usr/lib/qt/include/qobject.h
kspread_undo.o: /usr/lib/qt/include/qevent.h /usr/lib/qt/include/qrect.h
kspread_undo.o: /usr/lib/qt/include/qsize.h /usr/lib/qt/include/qpoint.h
kspread_undo.o: /usr/lib/qt/include/qpaintd.h /usr/lib/qt/include/qpalette.h
kspread_undo.o: /usr/lib/qt/include/qcolor.h /usr/lib/qt/include/qcursor.h
kspread_undo.o: /usr/lib/qt/include/qfont.h /usr/lib/qt/include/qfontmet.h
kspread_undo.o: /usr/lib/qt/include/qfontinf.h /usr/local/kde/include/kwm.h
kspread_undo.o: /usr/lib/qt/include/qapp.h /usr/lib/qt/include/qpixmap.h
kspread_undo.o: /usr/include/X11/Xlib.h /usr/include/X11/X.h
kspread_undo.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
kspread_undo.o: /usr/local/kde/include/kapp.h
kspread_undo.o: /usr/local/kde/include/kconfig.h
kspread_undo.o: /usr/local/kde/include/kconfigbase.h
kspread_undo.o: /usr/lib/qt/include/qstrlist.h /usr/lib/qt/include/qdstream.h
kspread_undo.o: /usr/lib/qt/include/qiodev.h
kspread_undo.o: /usr/local/kde/include/kconfigdata.h
kspread_undo.o: /usr/lib/qt/include/qtstream.h /usr/lib/qt/include/qfile.h
kspread_undo.o: /usr/local/kde/include/kdebug.h
kspread_undo.o: /usr/local/kde/include/klocale.h
kspread_undo.o: /usr/lib/qt/include/qintdict.h /usr/local/kde/include/drag.h
kspread_undo.o: /usr/lib/qt/include/qpopmenu.h /usr/lib/qt/include/qtablevw.h
kspread_undo.o: /usr/lib/qt/include/qframe.h /usr/lib/qt/include/qmenudta.h
kspread_undo.o: /usr/local/kde/include/parts.h
kspread_undo.o: /usr/local/kde/include/part_impl.h
kspread_undo.o: /usr/local/kde/include/document_impl.h
kspread_undo.o: /usr/local/kde/include/view_impl.h
kspread_undo.o: /usr/lib/qt/include/qpainter.h /usr/lib/qt/include/qregion.h
kspread_undo.o: /usr/lib/qt/include/qpen.h /usr/lib/qt/include/qbrush.h
kspread_undo.o: /usr/lib/qt/include/qpntarry.h /usr/lib/qt/include/qwmatrix.h
kspread_undo.o: /usr/lib/qt/include/qprinter.h kmetaedit.h kspread.h
kspread_undo.o: /opt/kde/idl/controls.h /opt/kde/idl/parts.h kspread_python.h
kspread_undo.o: ./Python-1.4/Include/Python.h
kspread_undo.o: ./Python-1.4/Include/allobjects.h
kspread_undo.o: /usr/local/kde/include/config.h /usr/include/errno.h
kspread_undo.o: /usr/include/errnos.h /usr/include/linux/errno.h
kspread_undo.o: /usr/include/asm/errno.h ./Python-1.4/Include/myproto.h
kspread_undo.o: ./Python-1.4/Include/object.h ./Python-1.4/Include/objimpl.h
kspread_undo.o: ./Python-1.4/Include/pydebug.h
kspread_undo.o: ./Python-1.4/Include/accessobject.h
kspread_undo.o: ./Python-1.4/Include/intobject.h
kspread_undo.o: ./Python-1.4/Include/longobject.h
kspread_undo.o: ./Python-1.4/Include/floatobject.h
kspread_undo.o: ./Python-1.4/Include/complexobject.h
kspread_undo.o: ./Python-1.4/Include/rangeobject.h
kspread_undo.o: ./Python-1.4/Include/stringobject.h
kspread_undo.o: ./Python-1.4/Include/tupleobject.h
kspread_undo.o: ./Python-1.4/Include/listobject.h
kspread_undo.o: ./Python-1.4/Include/mappingobject.h
kspread_undo.o: ./Python-1.4/Include/methodobject.h
kspread_undo.o: ./Python-1.4/Include/moduleobject.h
kspread_undo.o: ./Python-1.4/Include/funcobject.h
kspread_undo.o: ./Python-1.4/Include/classobject.h
kspread_undo.o: ./Python-1.4/Include/fileobject.h
kspread_undo.o: ./Python-1.4/Include/cobject.h
kspread_undo.o: ./Python-1.4/Include/traceback.h
kspread_undo.o: ./Python-1.4/Include/sliceobject.h
kspread_undo.o: ./Python-1.4/Include/pyerrors.h
kspread_undo.o: ./Python-1.4/Include/mymalloc.h
kspread_undo.o: ./Python-1.4/Include/modsupport.h
kspread_undo.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/varargs.h
kspread_undo.o: ./Python-1.4/Include/ceval.h ./Python-1.4/Include/pythonrun.h
kspread_undo.o: ./Python-1.4/Include/sysmodule.h
kspread_undo.o: ./Python-1.4/Include/intrcheck.h
kspread_undo.o: ./Python-1.4/Include/import.h
kspread_undo.o: ./Python-1.4/Include/bltinmodule.h
kspread_undo.o: ./Python-1.4/Include/abstract.h
kspread_undo.o: ./Python-1.4/Include/graminit.h kspread_view.h
kspread_undo.o: /usr/local/kde/include/menu_impl.h
kspread_undo.o: /usr/local/kde/include/kmenubar.h
kspread_undo.o: /usr/lib/qt/include/qmenubar.h
kspread_undo.o: /usr/local/kde/include/ktopwidget.h
kspread_undo.o: /usr/local/kde/include/ktoolbar.h
kspread_undo.o: /usr/lib/qt/include/qbutton.h /usr/local/kde/include/kcombo.h
kspread_undo.o: /usr/lib/qt/include/qcombo.h /usr/local/kde/include/klined.h
kspread_undo.o: /usr/lib/qt/include/qlined.h
kspread_undo.o: /usr/local/kde/include/kstatusbar.h
kspread_undo.o: /usr/lib/qt/include/qlabel.h
kspread_undo.o: /usr/local/kde/include/toolbar_impl.h
kspread_undo.o: /usr/lib/qt/include/qpushbt.h /usr/lib/qt/include/qscrbar.h
kspread_undo.o: /usr/lib/qt/include/qrangect.h /usr/lib/qt/include/qdrawutl.h
kspread_undo.o: kspread_tabbar.h kspread_table.h
kspread_undo.o: /usr/local/kde/include/komlParser.h
kspread_undo.o: /usr/local/kde/include/torben.h
kspread_undo.o: /usr/local/kde/include/komlFeed.h kspread_autofill.h
kspread_undo.o: kspread_layout.h kspread_cell.h kspread_dlg_layout.h
kspread_undo.o: /usr/lib/qt/include/qlistbox.h /usr/lib/qt/include/qgrpbox.h
kspread_undo.o: /usr/lib/qt/include/qtabdlg.h /usr/lib/qt/include/qdialog.h
kspread_undo.o: /usr/lib/qt/include/qmsgbox.h /usr/lib/qt/include/qbttngrp.h
kspread_undo.o: /usr/lib/qt/include/qchkbox.h /usr/lib/qt/include/qlcdnum.h
kspread_undo.o: /usr/lib/qt/include/qbitarry.h /usr/lib/qt/include/qradiobt.h
kspread_undo.o: /usr/lib/qt/include/qtooltip.h /usr/lib/qt/include/qtimer.h
kspread_undo.o: /usr/local/kde/include/kcolordlg.h
kspread_undo.o: /usr/local/kde/include/kselect.h kspread_map.h
kspread_undo.o: /usr/lib/qt/include/qstack.h /usr/lib/qt/include/qbuffer.h
kspread_view.o: kspread_view.h /usr/local/kde/include/part_frame_impl.h
kspread_view.o: /usr/local/kde/include/control_frame.h
kspread_view.o: /usr/local/kde/include/kom.h /usr/local/include/CORBA.h
kspread_view.o: /usr/local/include/mico/rtti.h
kspread_view.o: /usr/local/include/mico/config.h
kspread_view.o: /usr/local/include/mico/assert.h /usr/include/stdlib.h
kspread_view.o: /usr/include/features.h /usr/include/sys/cdefs.h
kspread_view.o: /usr/include/gnu/stubs.h
kspread_view.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
kspread_view.o: /usr/include/sys/types.h /usr/include/gnu/types.h
kspread_view.o: /usr/include/time.h /usr/include/endian.h
kspread_view.o: /usr/include/bytesex.h /usr/include/sys/select.h
kspread_view.o: /usr/include/selectbits.h /usr/include/alloca.h
kspread_view.o: /usr/include/stdio.h /usr/include/libio.h
kspread_view.o: /usr/include/_G_config.h
kspread_view.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stdarg.h
kspread_view.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/va-alpha.h
kspread_view.o: /usr/include/stdio_lim.h ./Python-1.4/Include/assert.h
kspread_view.o: /usr/include/sys/time.h /usr/include/timebits.h
kspread_view.o: /usr/include/sys/socket.h /usr/include/socketbits.h
kspread_view.o: /usr/include/sockaddrcom.h /usr/include/asm/socket.h
kspread_view.o: /usr/include/asm/sockios.h /usr/local/include/ministl/string
kspread_view.o: /usr/local/include/ministl/bstring.h /usr/include/ctype.h
kspread_view.o: /usr/include/string.h /usr/local/include/ministl/bool.h
kspread_view.o: /usr/local/include/ministl/vector
kspread_view.o: /usr/local/include/ministl/vector.h
kspread_view.o: /usr/local/include/ministl/ministl.h
kspread_view.o: /usr/local/include/ministl/defalloc.h
kspread_view.o: /usr/local/include/ministl/map
kspread_view.o: /usr/local/include/ministl/map.h
kspread_view.o: /usr/local/include/ministl/function
kspread_view.o: /usr/local/include/ministl/function.h
kspread_view.o: /usr/local/include/ministl/pair
kspread_view.o: /usr/local/include/ministl/pair.h
kspread_view.o: /usr/local/include/ministl/set
kspread_view.o: /usr/local/include/ministl/set.h
kspread_view.o: /usr/local/include/ministl/list
kspread_view.o: /usr/local/include/ministl/list.h
kspread_view.o: /usr/local/include/mico/version.h
kspread_view.o: /usr/local/include/mico/types.h
kspread_view.o: /usr/local/include/mico/sequence.h
kspread_view.o: /usr/local/include/mico/array.h /usr/local/include/mico/var.h
kspread_view.o: /usr/local/include/mico/basic.h
kspread_view.o: /usr/local/include/mico/address.h
kspread_view.o: /usr/local/include/mico/ior.h
kspread_view.o: /usr/local/include/mico/transport.h
kspread_view.o: /usr/local/include/mico/shlib.h
kspread_view.o: /usr/local/include/mico/process.h
kspread_view.o: /usr/local/include/mico/buffer.h
kspread_view.o: /usr/local/include/mico/codec.h
kspread_view.o: /usr/local/include/mico/string.h
kspread_view.o: /usr/local/include/mico/except.h
kspread_view.o: /usr/local/include/mico/any.h
kspread_view.o: /usr/local/include/mico/object.h
kspread_view.o: /usr/local/include/mico/boa.h
kspread_view.o: /usr/local/include/mico/orb_fwd.h
kspread_view.o: /usr/local/include/mico/dii.h /usr/local/include/mico/dsi.h
kspread_view.o: /usr/local/include/mico/dispatch.h
kspread_view.o: /usr/local/include/mico/ir.h
kspread_view.o: /usr/local/include/mico/typecode.h
kspread_view.o: /usr/local/include/mico/orb.h
kspread_view.o: /usr/local/include/mico/ioptypes.h
kspread_view.o: /usr/local/include/mico/imr.h /usr/local/include/mico/throw.h
kspread_view.o: /usr/local/include/mico/template_impl.h
kspread_view.o: /usr/local/kde/include/controls.h /opt/kde/idl/kom.h
kspread_view.o: /usr/local/kde/include/control_impl.h
kspread_view.o: /usr/local/kde/include/component_impl.h
kspread_view.o: /usr/local/kde/include/base_impl.h
kspread_view.o: /usr/lib/qt/include/qstring.h /usr/lib/qt/include/qarray.h
kspread_view.o: /usr/lib/qt/include/qgarray.h /usr/lib/qt/include/qshared.h
kspread_view.o: /usr/lib/qt/include/qglobal.h /usr/lib/qt/include/qgeneric.h
kspread_view.o: /usr/lib/qt/include/qlist.h /usr/lib/qt/include/qglist.h
kspread_view.o: /usr/lib/qt/include/qcollect.h /usr/lib/qt/include/qdict.h
kspread_view.o: /usr/lib/qt/include/qgdict.h /usr/local/kde/include/kom_var.h
kspread_view.o: /usr/lib/qt/include/qwidget.h /usr/lib/qt/include/qwindefs.h
kspread_view.o: /usr/lib/qt/include/qobjdefs.h /usr/lib/qt/include/qobject.h
kspread_view.o: /usr/lib/qt/include/qevent.h /usr/lib/qt/include/qrect.h
kspread_view.o: /usr/lib/qt/include/qsize.h /usr/lib/qt/include/qpoint.h
kspread_view.o: /usr/lib/qt/include/qpaintd.h /usr/lib/qt/include/qpalette.h
kspread_view.o: /usr/lib/qt/include/qcolor.h /usr/lib/qt/include/qcursor.h
kspread_view.o: /usr/lib/qt/include/qfont.h /usr/lib/qt/include/qfontmet.h
kspread_view.o: /usr/lib/qt/include/qfontinf.h /usr/local/kde/include/kwm.h
kspread_view.o: /usr/lib/qt/include/qapp.h /usr/lib/qt/include/qpixmap.h
kspread_view.o: /usr/include/X11/Xlib.h /usr/include/X11/X.h
kspread_view.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
kspread_view.o: /usr/local/kde/include/kapp.h
kspread_view.o: /usr/local/kde/include/kconfig.h
kspread_view.o: /usr/local/kde/include/kconfigbase.h
kspread_view.o: /usr/lib/qt/include/qstrlist.h /usr/lib/qt/include/qdstream.h
kspread_view.o: /usr/lib/qt/include/qiodev.h
kspread_view.o: /usr/local/kde/include/kconfigdata.h
kspread_view.o: /usr/lib/qt/include/qtstream.h /usr/lib/qt/include/qfile.h
kspread_view.o: /usr/local/kde/include/kdebug.h
kspread_view.o: /usr/local/kde/include/klocale.h
kspread_view.o: /usr/lib/qt/include/qintdict.h /usr/local/kde/include/drag.h
kspread_view.o: /usr/lib/qt/include/qpopmenu.h /usr/lib/qt/include/qtablevw.h
kspread_view.o: /usr/lib/qt/include/qframe.h /usr/lib/qt/include/qmenudta.h
kspread_view.o: /usr/local/kde/include/parts.h
kspread_view.o: /usr/local/kde/include/part_impl.h
kspread_view.o: /usr/local/kde/include/view_impl.h
kspread_view.o: /usr/local/kde/include/document_impl.h
kspread_view.o: /usr/local/kde/include/menu_impl.h
kspread_view.o: /usr/local/kde/include/kmenubar.h
kspread_view.o: /usr/lib/qt/include/qmenubar.h
kspread_view.o: /usr/local/kde/include/ktopwidget.h
kspread_view.o: /usr/local/kde/include/ktoolbar.h
kspread_view.o: /usr/lib/qt/include/qbutton.h /usr/local/kde/include/kcombo.h
kspread_view.o: /usr/lib/qt/include/qcombo.h /usr/local/kde/include/klined.h
kspread_view.o: /usr/lib/qt/include/qlined.h
kspread_view.o: /usr/local/kde/include/kstatusbar.h
kspread_view.o: /usr/lib/qt/include/qlabel.h
kspread_view.o: /usr/local/kde/include/toolbar_impl.h
kspread_view.o: /usr/lib/qt/include/qpushbt.h /usr/lib/qt/include/qscrbar.h
kspread_view.o: /usr/lib/qt/include/qrangect.h /usr/lib/qt/include/qdrawutl.h
kspread_view.o: /usr/lib/qt/include/qpainter.h /usr/lib/qt/include/qregion.h
kspread_view.o: /usr/lib/qt/include/qpen.h /usr/lib/qt/include/qbrush.h
kspread_view.o: /usr/lib/qt/include/qpntarry.h /usr/lib/qt/include/qwmatrix.h
kspread_view.o: kspread_tabbar.h kspread_table.h
kspread_view.o: /usr/local/kde/include/komlParser.h
kspread_view.o: /usr/local/kde/include/torben.h
kspread_view.o: /usr/local/kde/include/komlFeed.h kspread_autofill.h
kspread_view.o: kspread_layout.h kspread_cell.h kspread_dlg_layout.h
kspread_view.o: /usr/lib/qt/include/qlistbox.h /usr/lib/qt/include/qgrpbox.h
kspread_view.o: /usr/lib/qt/include/qtabdlg.h /usr/lib/qt/include/qdialog.h
kspread_view.o: /usr/lib/qt/include/qmsgbox.h /usr/lib/qt/include/qbttngrp.h
kspread_view.o: /usr/lib/qt/include/qchkbox.h /usr/lib/qt/include/qlcdnum.h
kspread_view.o: /usr/lib/qt/include/qbitarry.h /usr/lib/qt/include/qradiobt.h
kspread_view.o: /usr/lib/qt/include/qtooltip.h /usr/lib/qt/include/qtimer.h
kspread_view.o: /usr/local/kde/include/kcolordlg.h
kspread_view.o: /usr/local/kde/include/kselect.h kspread.h
kspread_view.o: /opt/kde/idl/controls.h /opt/kde/idl/parts.h
kspread_view.o: /usr/local/kde/include/utils.h /usr/lib/qt/include/qobjcoll.h
kspread_view.o: /usr/lib/qt/include/qmetaobj.h /usr/lib/qt/include/qconnect.h
kspread_view.o: /usr/lib/qt/include/qkeycode.h
kspread_view.o: /usr/local/kde/include/kbutton.h
kspread_view.o: /usr/local/kde/include/koPartSelectDia.h
kspread_view.o: /usr/local/kde/include/koScanParts.h
kspread_view.o: /usr/local/kde/include/op_app.h
kspread_view.o: /usr/local/kde/include/kom_app.h
kspread_view.o: /usr/lib/qt/include/qsocknot.h
kspread_view.o: /usr/local/kde/include/topwidget_impl.h
kspread_view.o: /usr/local/kde/include/koPartSelectDia_data.h kspread_map.h
kspread_view.o: kspread_dlg_scripts.h kmetaedit.h kspread_dlg_paper.h
kspread_view.o: kspread_dlg_paper_border.h kspread_dlg_paper_headings.h
kspread_view.o: kspread_dlg_print.h /usr/lib/qt/include/qprinter.h
kspread_view.o: kspread_doc.h kspread_python.h ./Python-1.4/Include/Python.h
kspread_view.o: ./Python-1.4/Include/allobjects.h
kspread_view.o: /usr/local/kde/include/config.h /usr/include/errno.h
kspread_view.o: /usr/include/errnos.h /usr/include/linux/errno.h
kspread_view.o: /usr/include/asm/errno.h ./Python-1.4/Include/myproto.h
kspread_view.o: ./Python-1.4/Include/object.h ./Python-1.4/Include/objimpl.h
kspread_view.o: ./Python-1.4/Include/pydebug.h
kspread_view.o: ./Python-1.4/Include/accessobject.h
kspread_view.o: ./Python-1.4/Include/intobject.h
kspread_view.o: ./Python-1.4/Include/longobject.h
kspread_view.o: ./Python-1.4/Include/floatobject.h
kspread_view.o: ./Python-1.4/Include/complexobject.h
kspread_view.o: ./Python-1.4/Include/rangeobject.h
kspread_view.o: ./Python-1.4/Include/stringobject.h
kspread_view.o: ./Python-1.4/Include/tupleobject.h
kspread_view.o: ./Python-1.4/Include/listobject.h
kspread_view.o: ./Python-1.4/Include/mappingobject.h
kspread_view.o: ./Python-1.4/Include/methodobject.h
kspread_view.o: ./Python-1.4/Include/moduleobject.h
kspread_view.o: ./Python-1.4/Include/funcobject.h
kspread_view.o: ./Python-1.4/Include/classobject.h
kspread_view.o: ./Python-1.4/Include/fileobject.h
kspread_view.o: ./Python-1.4/Include/cobject.h
kspread_view.o: ./Python-1.4/Include/traceback.h
kspread_view.o: ./Python-1.4/Include/sliceobject.h
kspread_view.o: ./Python-1.4/Include/pyerrors.h
kspread_view.o: ./Python-1.4/Include/mymalloc.h
kspread_view.o: ./Python-1.4/Include/modsupport.h
kspread_view.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/varargs.h
kspread_view.o: ./Python-1.4/Include/ceval.h ./Python-1.4/Include/pythonrun.h
kspread_view.o: ./Python-1.4/Include/sysmodule.h
kspread_view.o: ./Python-1.4/Include/intrcheck.h
kspread_view.o: ./Python-1.4/Include/import.h
kspread_view.o: ./Python-1.4/Include/bltinmodule.h
kspread_view.o: ./Python-1.4/Include/abstract.h
kspread_view.o: ./Python-1.4/Include/graminit.h kspread_undo.h
kspread_view.o: /usr/lib/qt/include/qstack.h kspread_shell.h
kspread_view.o: /usr/local/kde/include/default_shell_impl.h
kspread_view.o: /usr/local/kde/include/default_shell.h
kspread_view.o: /usr/local/kde/include/part_shell_impl.h
kspread_tabbar.o: /usr/lib/qt/include/qpntarry.h /usr/lib/qt/include/qarray.h
kspread_tabbar.o: /usr/lib/qt/include/qgarray.h /usr/lib/qt/include/qshared.h
kspread_tabbar.o: /usr/lib/qt/include/qglobal.h
kspread_tabbar.o: /usr/lib/qt/include/qgeneric.h /usr/lib/qt/include/qpoint.h
kspread_tabbar.o: /usr/lib/qt/include/qwindefs.h
kspread_tabbar.o: /usr/lib/qt/include/qobjdefs.h /usr/include/stdio.h
kspread_tabbar.o: /usr/include/libio.h /usr/include/features.h
kspread_tabbar.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
kspread_tabbar.o: /usr/include/_G_config.h /usr/include/gnu/types.h
kspread_tabbar.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
kspread_tabbar.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stdarg.h
kspread_tabbar.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/va-alpha.h
kspread_tabbar.o: /usr/include/stdio_lim.h /usr/include/stdlib.h
kspread_tabbar.o: /usr/include/sys/types.h /usr/include/time.h
kspread_tabbar.o: /usr/include/endian.h /usr/include/bytesex.h
kspread_tabbar.o: /usr/include/sys/select.h /usr/include/selectbits.h
kspread_tabbar.o: /usr/include/alloca.h kspread_tabbar.h
kspread_tabbar.o: /usr/lib/qt/include/qwidget.h /usr/lib/qt/include/qobject.h
kspread_tabbar.o: /usr/lib/qt/include/qstring.h /usr/include/string.h
kspread_tabbar.o: /usr/lib/qt/include/qevent.h /usr/lib/qt/include/qrect.h
kspread_tabbar.o: /usr/lib/qt/include/qsize.h /usr/lib/qt/include/qpaintd.h
kspread_tabbar.o: /usr/lib/qt/include/qpalette.h /usr/lib/qt/include/qcolor.h
kspread_tabbar.o: /usr/lib/qt/include/qcursor.h /usr/lib/qt/include/qfont.h
kspread_tabbar.o: /usr/lib/qt/include/qfontmet.h
kspread_tabbar.o: /usr/lib/qt/include/qfontinf.h
kspread_tabbar.o: /usr/lib/qt/include/qpainter.h
kspread_tabbar.o: /usr/lib/qt/include/qregion.h /usr/lib/qt/include/qpen.h
kspread_tabbar.o: /usr/lib/qt/include/qbrush.h /usr/lib/qt/include/qwmatrix.h
kspread_tabbar.o: /usr/lib/qt/include/qstrlist.h /usr/lib/qt/include/qlist.h
kspread_tabbar.o: /usr/lib/qt/include/qglist.h /usr/lib/qt/include/qcollect.h
kspread_tabbar.o: /usr/lib/qt/include/qdstream.h /usr/lib/qt/include/qiodev.h
kspread_python.o: kspread_python.h ./Python-1.4/Include/Python.h
kspread_python.o: ./Python-1.4/Include/allobjects.h
kspread_python.o: /usr/local/kde/include/config.h /usr/include/stdio.h
kspread_python.o: /usr/include/libio.h /usr/include/features.h
kspread_python.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
kspread_python.o: /usr/include/_G_config.h /usr/include/gnu/types.h
kspread_python.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
kspread_python.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stdarg.h
kspread_python.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/va-alpha.h
kspread_python.o: /usr/include/stdio_lim.h /usr/include/string.h
kspread_python.o: /usr/include/errno.h /usr/include/errnos.h
kspread_python.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
kspread_python.o: /usr/include/stdlib.h /usr/include/sys/types.h
kspread_python.o: /usr/include/time.h /usr/include/endian.h
kspread_python.o: /usr/include/bytesex.h /usr/include/sys/select.h
kspread_python.o: /usr/include/selectbits.h /usr/include/alloca.h
kspread_python.o: ./Python-1.4/Include/myproto.h
kspread_python.o: ./Python-1.4/Include/object.h
kspread_python.o: ./Python-1.4/Include/objimpl.h
kspread_python.o: ./Python-1.4/Include/pydebug.h
kspread_python.o: ./Python-1.4/Include/accessobject.h
kspread_python.o: ./Python-1.4/Include/intobject.h
kspread_python.o: ./Python-1.4/Include/longobject.h
kspread_python.o: ./Python-1.4/Include/floatobject.h
kspread_python.o: ./Python-1.4/Include/complexobject.h
kspread_python.o: ./Python-1.4/Include/rangeobject.h
kspread_python.o: ./Python-1.4/Include/stringobject.h
kspread_python.o: ./Python-1.4/Include/tupleobject.h
kspread_python.o: ./Python-1.4/Include/listobject.h
kspread_python.o: ./Python-1.4/Include/mappingobject.h
kspread_python.o: ./Python-1.4/Include/methodobject.h
kspread_python.o: ./Python-1.4/Include/moduleobject.h
kspread_python.o: ./Python-1.4/Include/funcobject.h
kspread_python.o: ./Python-1.4/Include/classobject.h
kspread_python.o: ./Python-1.4/Include/fileobject.h
kspread_python.o: ./Python-1.4/Include/cobject.h
kspread_python.o: ./Python-1.4/Include/traceback.h
kspread_python.o: ./Python-1.4/Include/sliceobject.h
kspread_python.o: ./Python-1.4/Include/pyerrors.h
kspread_python.o: ./Python-1.4/Include/mymalloc.h
kspread_python.o: ./Python-1.4/Include/modsupport.h
kspread_python.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/varargs.h
kspread_python.o: ./Python-1.4/Include/ceval.h
kspread_python.o: ./Python-1.4/Include/pythonrun.h
kspread_python.o: ./Python-1.4/Include/sysmodule.h
kspread_python.o: ./Python-1.4/Include/intrcheck.h
kspread_python.o: ./Python-1.4/Include/import.h
kspread_python.o: ./Python-1.4/Include/bltinmodule.h
kspread_python.o: ./Python-1.4/Include/abstract.h
kspread_python.o: ./Python-1.4/Include/graminit.h
kspread_python.o: /usr/lib/qt/include/qstring.h /usr/lib/qt/include/qarray.h
kspread_python.o: /usr/lib/qt/include/qgarray.h /usr/lib/qt/include/qshared.h
kspread_python.o: /usr/lib/qt/include/qglobal.h
kspread_python.o: /usr/lib/qt/include/qgeneric.h
kspread_python.o: ./Python-1.4/Include/assert.h
kspread_dlg_print.o: kspread_view.h /usr/local/kde/include/part_frame_impl.h
kspread_dlg_print.o: /usr/local/kde/include/control_frame.h
kspread_dlg_print.o: /usr/local/kde/include/kom.h /usr/local/include/CORBA.h
kspread_dlg_print.o: /usr/local/include/mico/rtti.h
kspread_dlg_print.o: /usr/local/include/mico/config.h
kspread_dlg_print.o: /usr/local/include/mico/assert.h /usr/include/stdlib.h
kspread_dlg_print.o: /usr/include/features.h /usr/include/sys/cdefs.h
kspread_dlg_print.o: /usr/include/gnu/stubs.h
kspread_dlg_print.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
kspread_dlg_print.o: /usr/include/sys/types.h /usr/include/gnu/types.h
kspread_dlg_print.o: /usr/include/time.h /usr/include/endian.h
kspread_dlg_print.o: /usr/include/bytesex.h /usr/include/sys/select.h
kspread_dlg_print.o: /usr/include/selectbits.h /usr/include/alloca.h
kspread_dlg_print.o: /usr/include/stdio.h /usr/include/libio.h
kspread_dlg_print.o: /usr/include/_G_config.h
kspread_dlg_print.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stdarg.h
kspread_dlg_print.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/va-alpha.h
kspread_dlg_print.o: /usr/include/stdio_lim.h ./Python-1.4/Include/assert.h
kspread_dlg_print.o: /usr/include/sys/time.h /usr/include/timebits.h
kspread_dlg_print.o: /usr/include/sys/socket.h /usr/include/socketbits.h
kspread_dlg_print.o: /usr/include/sockaddrcom.h /usr/include/asm/socket.h
kspread_dlg_print.o: /usr/include/asm/sockios.h
kspread_dlg_print.o: /usr/local/include/ministl/string
kspread_dlg_print.o: /usr/local/include/ministl/bstring.h
kspread_dlg_print.o: /usr/include/ctype.h /usr/include/string.h
kspread_dlg_print.o: /usr/local/include/ministl/bool.h
kspread_dlg_print.o: /usr/local/include/ministl/vector
kspread_dlg_print.o: /usr/local/include/ministl/vector.h
kspread_dlg_print.o: /usr/local/include/ministl/ministl.h
kspread_dlg_print.o: /usr/local/include/ministl/defalloc.h
kspread_dlg_print.o: /usr/local/include/ministl/map
kspread_dlg_print.o: /usr/local/include/ministl/map.h
kspread_dlg_print.o: /usr/local/include/ministl/function
kspread_dlg_print.o: /usr/local/include/ministl/function.h
kspread_dlg_print.o: /usr/local/include/ministl/pair
kspread_dlg_print.o: /usr/local/include/ministl/pair.h
kspread_dlg_print.o: /usr/local/include/ministl/set
kspread_dlg_print.o: /usr/local/include/ministl/set.h
kspread_dlg_print.o: /usr/local/include/ministl/list
kspread_dlg_print.o: /usr/local/include/ministl/list.h
kspread_dlg_print.o: /usr/local/include/mico/version.h
kspread_dlg_print.o: /usr/local/include/mico/types.h
kspread_dlg_print.o: /usr/local/include/mico/sequence.h
kspread_dlg_print.o: /usr/local/include/mico/array.h
kspread_dlg_print.o: /usr/local/include/mico/var.h
kspread_dlg_print.o: /usr/local/include/mico/basic.h
kspread_dlg_print.o: /usr/local/include/mico/address.h
kspread_dlg_print.o: /usr/local/include/mico/ior.h
kspread_dlg_print.o: /usr/local/include/mico/transport.h
kspread_dlg_print.o: /usr/local/include/mico/shlib.h
kspread_dlg_print.o: /usr/local/include/mico/process.h
kspread_dlg_print.o: /usr/local/include/mico/buffer.h
kspread_dlg_print.o: /usr/local/include/mico/codec.h
kspread_dlg_print.o: /usr/local/include/mico/string.h
kspread_dlg_print.o: /usr/local/include/mico/except.h
kspread_dlg_print.o: /usr/local/include/mico/any.h
kspread_dlg_print.o: /usr/local/include/mico/object.h
kspread_dlg_print.o: /usr/local/include/mico/boa.h
kspread_dlg_print.o: /usr/local/include/mico/orb_fwd.h
kspread_dlg_print.o: /usr/local/include/mico/dii.h
kspread_dlg_print.o: /usr/local/include/mico/dsi.h
kspread_dlg_print.o: /usr/local/include/mico/dispatch.h
kspread_dlg_print.o: /usr/local/include/mico/ir.h
kspread_dlg_print.o: /usr/local/include/mico/typecode.h
kspread_dlg_print.o: /usr/local/include/mico/orb.h
kspread_dlg_print.o: /usr/local/include/mico/ioptypes.h
kspread_dlg_print.o: /usr/local/include/mico/imr.h
kspread_dlg_print.o: /usr/local/include/mico/throw.h
kspread_dlg_print.o: /usr/local/include/mico/template_impl.h
kspread_dlg_print.o: /usr/local/kde/include/controls.h /opt/kde/idl/kom.h
kspread_dlg_print.o: /usr/local/kde/include/control_impl.h
kspread_dlg_print.o: /usr/local/kde/include/component_impl.h
kspread_dlg_print.o: /usr/local/kde/include/base_impl.h
kspread_dlg_print.o: /usr/lib/qt/include/qstring.h
kspread_dlg_print.o: /usr/lib/qt/include/qarray.h
kspread_dlg_print.o: /usr/lib/qt/include/qgarray.h
kspread_dlg_print.o: /usr/lib/qt/include/qshared.h
kspread_dlg_print.o: /usr/lib/qt/include/qglobal.h
kspread_dlg_print.o: /usr/lib/qt/include/qgeneric.h
kspread_dlg_print.o: /usr/lib/qt/include/qlist.h /usr/lib/qt/include/qglist.h
kspread_dlg_print.o: /usr/lib/qt/include/qcollect.h
kspread_dlg_print.o: /usr/lib/qt/include/qdict.h /usr/lib/qt/include/qgdict.h
kspread_dlg_print.o: /usr/local/kde/include/kom_var.h
kspread_dlg_print.o: /usr/lib/qt/include/qwidget.h
kspread_dlg_print.o: /usr/lib/qt/include/qwindefs.h
kspread_dlg_print.o: /usr/lib/qt/include/qobjdefs.h
kspread_dlg_print.o: /usr/lib/qt/include/qobject.h
kspread_dlg_print.o: /usr/lib/qt/include/qevent.h /usr/lib/qt/include/qrect.h
kspread_dlg_print.o: /usr/lib/qt/include/qsize.h /usr/lib/qt/include/qpoint.h
kspread_dlg_print.o: /usr/lib/qt/include/qpaintd.h
kspread_dlg_print.o: /usr/lib/qt/include/qpalette.h
kspread_dlg_print.o: /usr/lib/qt/include/qcolor.h
kspread_dlg_print.o: /usr/lib/qt/include/qcursor.h
kspread_dlg_print.o: /usr/lib/qt/include/qfont.h
kspread_dlg_print.o: /usr/lib/qt/include/qfontmet.h
kspread_dlg_print.o: /usr/lib/qt/include/qfontinf.h
kspread_dlg_print.o: /usr/local/kde/include/kwm.h /usr/lib/qt/include/qapp.h
kspread_dlg_print.o: /usr/lib/qt/include/qpixmap.h /usr/include/X11/Xlib.h
kspread_dlg_print.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
kspread_dlg_print.o: /usr/include/X11/Xosdefs.h /usr/local/kde/include/kapp.h
kspread_dlg_print.o: /usr/local/kde/include/kconfig.h
kspread_dlg_print.o: /usr/local/kde/include/kconfigbase.h
kspread_dlg_print.o: /usr/lib/qt/include/qstrlist.h
kspread_dlg_print.o: /usr/lib/qt/include/qdstream.h
kspread_dlg_print.o: /usr/lib/qt/include/qiodev.h
kspread_dlg_print.o: /usr/local/kde/include/kconfigdata.h
kspread_dlg_print.o: /usr/lib/qt/include/qtstream.h
kspread_dlg_print.o: /usr/lib/qt/include/qfile.h
kspread_dlg_print.o: /usr/local/kde/include/kdebug.h
kspread_dlg_print.o: /usr/local/kde/include/klocale.h
kspread_dlg_print.o: /usr/lib/qt/include/qintdict.h
kspread_dlg_print.o: /usr/local/kde/include/drag.h
kspread_dlg_print.o: /usr/lib/qt/include/qpopmenu.h
kspread_dlg_print.o: /usr/lib/qt/include/qtablevw.h
kspread_dlg_print.o: /usr/lib/qt/include/qframe.h
kspread_dlg_print.o: /usr/lib/qt/include/qmenudta.h
kspread_dlg_print.o: /usr/local/kde/include/parts.h
kspread_dlg_print.o: /usr/local/kde/include/part_impl.h
kspread_dlg_print.o: /usr/local/kde/include/view_impl.h
kspread_dlg_print.o: /usr/local/kde/include/document_impl.h
kspread_dlg_print.o: /usr/local/kde/include/menu_impl.h
kspread_dlg_print.o: /usr/local/kde/include/kmenubar.h
kspread_dlg_print.o: /usr/lib/qt/include/qmenubar.h
kspread_dlg_print.o: /usr/local/kde/include/ktopwidget.h
kspread_dlg_print.o: /usr/local/kde/include/ktoolbar.h
kspread_dlg_print.o: /usr/lib/qt/include/qbutton.h
kspread_dlg_print.o: /usr/local/kde/include/kcombo.h
kspread_dlg_print.o: /usr/lib/qt/include/qcombo.h
kspread_dlg_print.o: /usr/local/kde/include/klined.h
kspread_dlg_print.o: /usr/lib/qt/include/qlined.h
kspread_dlg_print.o: /usr/local/kde/include/kstatusbar.h
kspread_dlg_print.o: /usr/lib/qt/include/qlabel.h
kspread_dlg_print.o: /usr/local/kde/include/toolbar_impl.h
kspread_dlg_print.o: /usr/lib/qt/include/qpushbt.h
kspread_dlg_print.o: /usr/lib/qt/include/qscrbar.h
kspread_dlg_print.o: /usr/lib/qt/include/qrangect.h
kspread_dlg_print.o: /usr/lib/qt/include/qdrawutl.h
kspread_dlg_print.o: /usr/lib/qt/include/qpainter.h
kspread_dlg_print.o: /usr/lib/qt/include/qregion.h /usr/lib/qt/include/qpen.h
kspread_dlg_print.o: /usr/lib/qt/include/qbrush.h
kspread_dlg_print.o: /usr/lib/qt/include/qpntarry.h
kspread_dlg_print.o: /usr/lib/qt/include/qwmatrix.h kspread_tabbar.h
kspread_dlg_print.o: kspread_table.h /usr/local/kde/include/komlParser.h
kspread_dlg_print.o: /usr/local/kde/include/torben.h
kspread_dlg_print.o: /usr/local/kde/include/komlFeed.h kspread_autofill.h
kspread_dlg_print.o: kspread_layout.h kspread_cell.h kspread_dlg_layout.h
kspread_dlg_print.o: /usr/lib/qt/include/qlistbox.h
kspread_dlg_print.o: /usr/lib/qt/include/qgrpbox.h
kspread_dlg_print.o: /usr/lib/qt/include/qtabdlg.h
kspread_dlg_print.o: /usr/lib/qt/include/qdialog.h
kspread_dlg_print.o: /usr/lib/qt/include/qmsgbox.h
kspread_dlg_print.o: /usr/lib/qt/include/qbttngrp.h
kspread_dlg_print.o: /usr/lib/qt/include/qchkbox.h
kspread_dlg_print.o: /usr/lib/qt/include/qlcdnum.h
kspread_dlg_print.o: /usr/lib/qt/include/qbitarry.h
kspread_dlg_print.o: /usr/lib/qt/include/qradiobt.h
kspread_dlg_print.o: /usr/lib/qt/include/qtooltip.h
kspread_dlg_print.o: /usr/lib/qt/include/qtimer.h
kspread_dlg_print.o: /usr/local/kde/include/kcolordlg.h
kspread_dlg_print.o: /usr/local/kde/include/kselect.h kspread.h
kspread_dlg_print.o: /opt/kde/idl/controls.h /opt/kde/idl/parts.h
kspread_dlg_print.o: kspread_dlg_print.h /usr/lib/qt/include/qprinter.h
kspread_dlg_print.o: kspread_doc.h kmetaedit.h kspread_python.h
kspread_dlg_print.o: ./Python-1.4/Include/Python.h
kspread_dlg_print.o: ./Python-1.4/Include/allobjects.h
kspread_dlg_print.o: /usr/local/kde/include/config.h /usr/include/errno.h
kspread_dlg_print.o: /usr/include/errnos.h /usr/include/linux/errno.h
kspread_dlg_print.o: /usr/include/asm/errno.h ./Python-1.4/Include/myproto.h
kspread_dlg_print.o: ./Python-1.4/Include/object.h
kspread_dlg_print.o: ./Python-1.4/Include/objimpl.h
kspread_dlg_print.o: ./Python-1.4/Include/pydebug.h
kspread_dlg_print.o: ./Python-1.4/Include/accessobject.h
kspread_dlg_print.o: ./Python-1.4/Include/intobject.h
kspread_dlg_print.o: ./Python-1.4/Include/longobject.h
kspread_dlg_print.o: ./Python-1.4/Include/floatobject.h
kspread_dlg_print.o: ./Python-1.4/Include/complexobject.h
kspread_dlg_print.o: ./Python-1.4/Include/rangeobject.h
kspread_dlg_print.o: ./Python-1.4/Include/stringobject.h
kspread_dlg_print.o: ./Python-1.4/Include/tupleobject.h
kspread_dlg_print.o: ./Python-1.4/Include/listobject.h
kspread_dlg_print.o: ./Python-1.4/Include/mappingobject.h
kspread_dlg_print.o: ./Python-1.4/Include/methodobject.h
kspread_dlg_print.o: ./Python-1.4/Include/moduleobject.h
kspread_dlg_print.o: ./Python-1.4/Include/funcobject.h
kspread_dlg_print.o: ./Python-1.4/Include/classobject.h
kspread_dlg_print.o: ./Python-1.4/Include/fileobject.h
kspread_dlg_print.o: ./Python-1.4/Include/cobject.h
kspread_dlg_print.o: ./Python-1.4/Include/traceback.h
kspread_dlg_print.o: ./Python-1.4/Include/sliceobject.h
kspread_dlg_print.o: ./Python-1.4/Include/pyerrors.h
kspread_dlg_print.o: ./Python-1.4/Include/mymalloc.h
kspread_dlg_print.o: ./Python-1.4/Include/modsupport.h
kspread_dlg_print.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/varargs.h
kspread_dlg_print.o: ./Python-1.4/Include/ceval.h
kspread_dlg_print.o: ./Python-1.4/Include/pythonrun.h
kspread_dlg_print.o: ./Python-1.4/Include/sysmodule.h
kspread_dlg_print.o: ./Python-1.4/Include/intrcheck.h
kspread_dlg_print.o: ./Python-1.4/Include/import.h
kspread_dlg_print.o: ./Python-1.4/Include/bltinmodule.h
kspread_dlg_print.o: ./Python-1.4/Include/abstract.h
kspread_dlg_print.o: ./Python-1.4/Include/graminit.h kspread_undo.h
kspread_dlg_print.o: /usr/lib/qt/include/qstack.h kspread_map.h
kspread_dlg_print.o: /usr/include/dirent.h /usr/include/direntry.h
kspread_dlg_print.o: /usr/include/posix1_lim.h /usr/include/local_lim.h
kspread_dlg_print.o: /usr/include/linux/limits.h /usr/include/unistd.h
kspread_dlg_print.o: /usr/include/posix_opt.h /usr/include/confname.h
kspread_autofill.o: kspread_autofill.h /usr/lib/qt/include/qarray.h
kspread_autofill.o: /usr/lib/qt/include/qgarray.h
kspread_autofill.o: /usr/lib/qt/include/qshared.h
kspread_autofill.o: /usr/lib/qt/include/qglobal.h
kspread_autofill.o: /usr/lib/qt/include/qgeneric.h
kspread_autofill.o: /usr/lib/qt/include/qstring.h /usr/include/string.h
kspread_autofill.o: /usr/include/features.h /usr/include/sys/cdefs.h
kspread_autofill.o: /usr/include/gnu/stubs.h
kspread_autofill.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
kspread_autofill.o: /usr/lib/qt/include/qlist.h /usr/lib/qt/include/qglist.h
kspread_autofill.o: /usr/lib/qt/include/qcollect.h
kspread_autofill.o: /usr/lib/qt/include/qstrlist.h
kspread_autofill.o: /usr/lib/qt/include/qdstream.h
kspread_autofill.o: /usr/lib/qt/include/qiodev.h kspread_cell.h
kspread_autofill.o: /usr/local/kde/include/komlParser.h
kspread_autofill.o: /usr/local/include/ministl/vector.h
kspread_autofill.o: /usr/local/include/ministl/ministl.h
kspread_autofill.o: /usr/local/include/ministl/bool.h
kspread_autofill.o: /usr/local/include/ministl/defalloc.h
kspread_autofill.o: /usr/local/kde/include/torben.h
kspread_autofill.o: /usr/local/include/ministl/list.h
kspread_autofill.o: /usr/local/include/ministl/string
kspread_autofill.o: /usr/local/include/ministl/bstring.h /usr/include/ctype.h
kspread_autofill.o: /usr/include/endian.h /usr/include/bytesex.h
kspread_autofill.o: /usr/include/stdlib.h /usr/include/sys/types.h
kspread_autofill.o: /usr/include/gnu/types.h /usr/include/time.h
kspread_autofill.o: /usr/include/sys/select.h /usr/include/selectbits.h
kspread_autofill.o: /usr/include/alloca.h /usr/local/include/ministl/vector
kspread_autofill.o: /usr/local/kde/include/komlFeed.h
kspread_autofill.o: /usr/lib/qt/include/qpen.h /usr/lib/qt/include/qcolor.h
kspread_autofill.o: /usr/lib/qt/include/qwindefs.h
kspread_autofill.o: /usr/lib/qt/include/qobjdefs.h
kspread_autofill.o: /usr/lib/qt/include/qpainter.h
kspread_autofill.o: /usr/lib/qt/include/qpaintd.h /usr/lib/qt/include/qrect.h
kspread_autofill.o: /usr/lib/qt/include/qsize.h /usr/lib/qt/include/qpoint.h
kspread_autofill.o: /usr/lib/qt/include/qfontmet.h
kspread_autofill.o: /usr/lib/qt/include/qfont.h
kspread_autofill.o: /usr/lib/qt/include/qfontinf.h
kspread_autofill.o: /usr/lib/qt/include/qregion.h
kspread_autofill.o: /usr/lib/qt/include/qbrush.h
kspread_autofill.o: /usr/lib/qt/include/qpntarry.h
kspread_autofill.o: /usr/lib/qt/include/qwmatrix.h kspread_layout.h
kspread_autofill.o: kspread_table.h /usr/local/kde/include/document_impl.h
kspread_autofill.o: /usr/local/kde/include/parts.h /usr/local/include/CORBA.h
kspread_autofill.o: /usr/local/include/mico/rtti.h
kspread_autofill.o: /usr/local/include/mico/config.h
kspread_autofill.o: /usr/local/include/mico/assert.h /usr/include/stdio.h
kspread_autofill.o: /usr/include/libio.h /usr/include/_G_config.h
kspread_autofill.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stdarg.h
kspread_autofill.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/va-alpha.h
kspread_autofill.o: /usr/include/stdio_lim.h ./Python-1.4/Include/assert.h
kspread_autofill.o: /usr/include/sys/time.h /usr/include/timebits.h
kspread_autofill.o: /usr/include/sys/socket.h /usr/include/socketbits.h
kspread_autofill.o: /usr/include/sockaddrcom.h /usr/include/asm/socket.h
kspread_autofill.o: /usr/include/asm/sockios.h /usr/local/include/ministl/map
kspread_autofill.o: /usr/local/include/ministl/map.h
kspread_autofill.o: /usr/local/include/ministl/function
kspread_autofill.o: /usr/local/include/ministl/function.h
kspread_autofill.o: /usr/local/include/ministl/pair
kspread_autofill.o: /usr/local/include/ministl/pair.h
kspread_autofill.o: /usr/local/include/ministl/set
kspread_autofill.o: /usr/local/include/ministl/set.h
kspread_autofill.o: /usr/local/include/ministl/list
kspread_autofill.o: /usr/local/include/mico/version.h
kspread_autofill.o: /usr/local/include/mico/types.h
kspread_autofill.o: /usr/local/include/mico/sequence.h
kspread_autofill.o: /usr/local/include/mico/array.h
kspread_autofill.o: /usr/local/include/mico/var.h
kspread_autofill.o: /usr/local/include/mico/basic.h
kspread_autofill.o: /usr/local/include/mico/address.h
kspread_autofill.o: /usr/local/include/mico/ior.h
kspread_autofill.o: /usr/local/include/mico/transport.h
kspread_autofill.o: /usr/local/include/mico/shlib.h
kspread_autofill.o: /usr/local/include/mico/process.h
kspread_autofill.o: /usr/local/include/mico/buffer.h
kspread_autofill.o: /usr/local/include/mico/codec.h
kspread_autofill.o: /usr/local/include/mico/string.h
kspread_autofill.o: /usr/local/include/mico/except.h
kspread_autofill.o: /usr/local/include/mico/any.h
kspread_autofill.o: /usr/local/include/mico/object.h
kspread_autofill.o: /usr/local/include/mico/boa.h
kspread_autofill.o: /usr/local/include/mico/orb_fwd.h
kspread_autofill.o: /usr/local/include/mico/dii.h
kspread_autofill.o: /usr/local/include/mico/dsi.h
kspread_autofill.o: /usr/local/include/mico/dispatch.h
kspread_autofill.o: /usr/local/include/mico/ir.h
kspread_autofill.o: /usr/local/include/mico/typecode.h
kspread_autofill.o: /usr/local/include/mico/orb.h
kspread_autofill.o: /usr/local/include/mico/ioptypes.h
kspread_autofill.o: /usr/local/include/mico/imr.h
kspread_autofill.o: /usr/local/include/mico/throw.h /opt/kde/idl/kom.h
kspread_autofill.o: /usr/local/kde/include/controls.h
kspread_autofill.o: /usr/local/include/mico/template_impl.h
kspread_autofill.o: /usr/local/kde/include/component_impl.h
kspread_autofill.o: /usr/local/kde/include/kom.h
kspread_autofill.o: /usr/local/kde/include/base_impl.h
kspread_autofill.o: /usr/lib/qt/include/qdict.h /usr/lib/qt/include/qgdict.h
kspread_autofill.o: /usr/local/kde/include/kom_var.h
kspread_autofill.o: /usr/local/kde/include/view_impl.h
kspread_autofill.o: /usr/local/kde/include/part_impl.h
kspread_autofill.o: /usr/local/kde/include/control_impl.h
kspread_autofill.o: /usr/lib/qt/include/qintdict.h kspread_dlg_layout.h
kspread_autofill.o: /usr/lib/qt/include/qwidget.h
kspread_autofill.o: /usr/lib/qt/include/qobject.h
kspread_autofill.o: /usr/lib/qt/include/qevent.h
kspread_autofill.o: /usr/lib/qt/include/qpalette.h
kspread_autofill.o: /usr/lib/qt/include/qcursor.h
kspread_autofill.o: /usr/lib/qt/include/qlistbox.h
kspread_autofill.o: /usr/lib/qt/include/qtablevw.h
kspread_autofill.o: /usr/lib/qt/include/qframe.h
kspread_autofill.o: /usr/lib/qt/include/qpixmap.h
kspread_autofill.o: /usr/lib/qt/include/qlined.h /usr/lib/qt/include/qcombo.h
kspread_autofill.o: /usr/lib/qt/include/qlabel.h
kspread_autofill.o: /usr/lib/qt/include/qgrpbox.h
kspread_autofill.o: /usr/lib/qt/include/qtabdlg.h
kspread_autofill.o: /usr/lib/qt/include/qdialog.h
kspread_autofill.o: /usr/lib/qt/include/qmsgbox.h /usr/lib/qt/include/qapp.h
kspread_autofill.o: /usr/lib/qt/include/qbttngrp.h
kspread_autofill.o: /usr/lib/qt/include/qchkbox.h
kspread_autofill.o: /usr/lib/qt/include/qbutton.h
kspread_autofill.o: /usr/lib/qt/include/qlcdnum.h
kspread_autofill.o: /usr/lib/qt/include/qbitarry.h
kspread_autofill.o: /usr/lib/qt/include/qpushbt.h
kspread_autofill.o: /usr/lib/qt/include/qradiobt.h
kspread_autofill.o: /usr/lib/qt/include/qscrbar.h
kspread_autofill.o: /usr/lib/qt/include/qrangect.h
kspread_autofill.o: /usr/lib/qt/include/qdrawutl.h
kspread_autofill.o: /usr/lib/qt/include/qtooltip.h
kspread_autofill.o: /usr/lib/qt/include/qtimer.h
kspread_autofill.o: /usr/local/kde/include/kcolordlg.h
kspread_autofill.o: /usr/local/kde/include/kselect.h /usr/include/math.h
kspread_autofill.o: /usr/include/huge_val.h /usr/include/mathcalls.h
kspread_autofill.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/float.h
kspread_layout.o: kspread_view.h /usr/local/kde/include/part_frame_impl.h
kspread_layout.o: /usr/local/kde/include/control_frame.h
kspread_layout.o: /usr/local/kde/include/kom.h /usr/local/include/CORBA.h
kspread_layout.o: /usr/local/include/mico/rtti.h
kspread_layout.o: /usr/local/include/mico/config.h
kspread_layout.o: /usr/local/include/mico/assert.h /usr/include/stdlib.h
kspread_layout.o: /usr/include/features.h /usr/include/sys/cdefs.h
kspread_layout.o: /usr/include/gnu/stubs.h
kspread_layout.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
kspread_layout.o: /usr/include/sys/types.h /usr/include/gnu/types.h
kspread_layout.o: /usr/include/time.h /usr/include/endian.h
kspread_layout.o: /usr/include/bytesex.h /usr/include/sys/select.h
kspread_layout.o: /usr/include/selectbits.h /usr/include/alloca.h
kspread_layout.o: /usr/include/stdio.h /usr/include/libio.h
kspread_layout.o: /usr/include/_G_config.h
kspread_layout.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stdarg.h
kspread_layout.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/va-alpha.h
kspread_layout.o: /usr/include/stdio_lim.h ./Python-1.4/Include/assert.h
kspread_layout.o: /usr/include/sys/time.h /usr/include/timebits.h
kspread_layout.o: /usr/include/sys/socket.h /usr/include/socketbits.h
kspread_layout.o: /usr/include/sockaddrcom.h /usr/include/asm/socket.h
kspread_layout.o: /usr/include/asm/sockios.h
kspread_layout.o: /usr/local/include/ministl/string
kspread_layout.o: /usr/local/include/ministl/bstring.h /usr/include/ctype.h
kspread_layout.o: /usr/include/string.h /usr/local/include/ministl/bool.h
kspread_layout.o: /usr/local/include/ministl/vector
kspread_layout.o: /usr/local/include/ministl/vector.h
kspread_layout.o: /usr/local/include/ministl/ministl.h
kspread_layout.o: /usr/local/include/ministl/defalloc.h
kspread_layout.o: /usr/local/include/ministl/map
kspread_layout.o: /usr/local/include/ministl/map.h
kspread_layout.o: /usr/local/include/ministl/function
kspread_layout.o: /usr/local/include/ministl/function.h
kspread_layout.o: /usr/local/include/ministl/pair
kspread_layout.o: /usr/local/include/ministl/pair.h
kspread_layout.o: /usr/local/include/ministl/set
kspread_layout.o: /usr/local/include/ministl/set.h
kspread_layout.o: /usr/local/include/ministl/list
kspread_layout.o: /usr/local/include/ministl/list.h
kspread_layout.o: /usr/local/include/mico/version.h
kspread_layout.o: /usr/local/include/mico/types.h
kspread_layout.o: /usr/local/include/mico/sequence.h
kspread_layout.o: /usr/local/include/mico/array.h
kspread_layout.o: /usr/local/include/mico/var.h
kspread_layout.o: /usr/local/include/mico/basic.h
kspread_layout.o: /usr/local/include/mico/address.h
kspread_layout.o: /usr/local/include/mico/ior.h
kspread_layout.o: /usr/local/include/mico/transport.h
kspread_layout.o: /usr/local/include/mico/shlib.h
kspread_layout.o: /usr/local/include/mico/process.h
kspread_layout.o: /usr/local/include/mico/buffer.h
kspread_layout.o: /usr/local/include/mico/codec.h
kspread_layout.o: /usr/local/include/mico/string.h
kspread_layout.o: /usr/local/include/mico/except.h
kspread_layout.o: /usr/local/include/mico/any.h
kspread_layout.o: /usr/local/include/mico/object.h
kspread_layout.o: /usr/local/include/mico/boa.h
kspread_layout.o: /usr/local/include/mico/orb_fwd.h
kspread_layout.o: /usr/local/include/mico/dii.h /usr/local/include/mico/dsi.h
kspread_layout.o: /usr/local/include/mico/dispatch.h
kspread_layout.o: /usr/local/include/mico/ir.h
kspread_layout.o: /usr/local/include/mico/typecode.h
kspread_layout.o: /usr/local/include/mico/orb.h
kspread_layout.o: /usr/local/include/mico/ioptypes.h
kspread_layout.o: /usr/local/include/mico/imr.h
kspread_layout.o: /usr/local/include/mico/throw.h
kspread_layout.o: /usr/local/include/mico/template_impl.h
kspread_layout.o: /usr/local/kde/include/controls.h /opt/kde/idl/kom.h
kspread_layout.o: /usr/local/kde/include/control_impl.h
kspread_layout.o: /usr/local/kde/include/component_impl.h
kspread_layout.o: /usr/local/kde/include/base_impl.h
kspread_layout.o: /usr/lib/qt/include/qstring.h /usr/lib/qt/include/qarray.h
kspread_layout.o: /usr/lib/qt/include/qgarray.h /usr/lib/qt/include/qshared.h
kspread_layout.o: /usr/lib/qt/include/qglobal.h
kspread_layout.o: /usr/lib/qt/include/qgeneric.h /usr/lib/qt/include/qlist.h
kspread_layout.o: /usr/lib/qt/include/qglist.h /usr/lib/qt/include/qcollect.h
kspread_layout.o: /usr/lib/qt/include/qdict.h /usr/lib/qt/include/qgdict.h
kspread_layout.o: /usr/local/kde/include/kom_var.h
kspread_layout.o: /usr/lib/qt/include/qwidget.h
kspread_layout.o: /usr/lib/qt/include/qwindefs.h
kspread_layout.o: /usr/lib/qt/include/qobjdefs.h
kspread_layout.o: /usr/lib/qt/include/qobject.h /usr/lib/qt/include/qevent.h
kspread_layout.o: /usr/lib/qt/include/qrect.h /usr/lib/qt/include/qsize.h
kspread_layout.o: /usr/lib/qt/include/qpoint.h /usr/lib/qt/include/qpaintd.h
kspread_layout.o: /usr/lib/qt/include/qpalette.h /usr/lib/qt/include/qcolor.h
kspread_layout.o: /usr/lib/qt/include/qcursor.h /usr/lib/qt/include/qfont.h
kspread_layout.o: /usr/lib/qt/include/qfontmet.h
kspread_layout.o: /usr/lib/qt/include/qfontinf.h /usr/local/kde/include/kwm.h
kspread_layout.o: /usr/lib/qt/include/qapp.h /usr/lib/qt/include/qpixmap.h
kspread_layout.o: /usr/include/X11/Xlib.h /usr/include/X11/X.h
kspread_layout.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
kspread_layout.o: /usr/local/kde/include/kapp.h
kspread_layout.o: /usr/local/kde/include/kconfig.h
kspread_layout.o: /usr/local/kde/include/kconfigbase.h
kspread_layout.o: /usr/lib/qt/include/qstrlist.h
kspread_layout.o: /usr/lib/qt/include/qdstream.h /usr/lib/qt/include/qiodev.h
kspread_layout.o: /usr/local/kde/include/kconfigdata.h
kspread_layout.o: /usr/lib/qt/include/qtstream.h /usr/lib/qt/include/qfile.h
kspread_layout.o: /usr/local/kde/include/kdebug.h
kspread_layout.o: /usr/local/kde/include/klocale.h
kspread_layout.o: /usr/lib/qt/include/qintdict.h
kspread_layout.o: /usr/local/kde/include/drag.h
kspread_layout.o: /usr/lib/qt/include/qpopmenu.h
kspread_layout.o: /usr/lib/qt/include/qtablevw.h /usr/lib/qt/include/qframe.h
kspread_layout.o: /usr/lib/qt/include/qmenudta.h
kspread_layout.o: /usr/local/kde/include/parts.h
kspread_layout.o: /usr/local/kde/include/part_impl.h
kspread_layout.o: /usr/local/kde/include/view_impl.h
kspread_layout.o: /usr/local/kde/include/document_impl.h
kspread_layout.o: /usr/local/kde/include/menu_impl.h
kspread_layout.o: /usr/local/kde/include/kmenubar.h
kspread_layout.o: /usr/lib/qt/include/qmenubar.h
kspread_layout.o: /usr/local/kde/include/ktopwidget.h
kspread_layout.o: /usr/local/kde/include/ktoolbar.h
kspread_layout.o: /usr/lib/qt/include/qbutton.h
kspread_layout.o: /usr/local/kde/include/kcombo.h
kspread_layout.o: /usr/lib/qt/include/qcombo.h
kspread_layout.o: /usr/local/kde/include/klined.h
kspread_layout.o: /usr/lib/qt/include/qlined.h
kspread_layout.o: /usr/local/kde/include/kstatusbar.h
kspread_layout.o: /usr/lib/qt/include/qlabel.h
kspread_layout.o: /usr/local/kde/include/toolbar_impl.h
kspread_layout.o: /usr/lib/qt/include/qpushbt.h /usr/lib/qt/include/qscrbar.h
kspread_layout.o: /usr/lib/qt/include/qrangect.h
kspread_layout.o: /usr/lib/qt/include/qdrawutl.h
kspread_layout.o: /usr/lib/qt/include/qpainter.h
kspread_layout.o: /usr/lib/qt/include/qregion.h /usr/lib/qt/include/qpen.h
kspread_layout.o: /usr/lib/qt/include/qbrush.h /usr/lib/qt/include/qpntarry.h
kspread_layout.o: /usr/lib/qt/include/qwmatrix.h kspread_tabbar.h
kspread_layout.o: kspread_table.h /usr/local/kde/include/komlParser.h
kspread_layout.o: /usr/local/kde/include/torben.h
kspread_layout.o: /usr/local/kde/include/komlFeed.h kspread_autofill.h
kspread_layout.o: kspread_layout.h kspread_cell.h kspread_dlg_layout.h
kspread_layout.o: /usr/lib/qt/include/qlistbox.h
kspread_layout.o: /usr/lib/qt/include/qgrpbox.h /usr/lib/qt/include/qtabdlg.h
kspread_layout.o: /usr/lib/qt/include/qdialog.h /usr/lib/qt/include/qmsgbox.h
kspread_layout.o: /usr/lib/qt/include/qbttngrp.h
kspread_layout.o: /usr/lib/qt/include/qchkbox.h /usr/lib/qt/include/qlcdnum.h
kspread_layout.o: /usr/lib/qt/include/qbitarry.h
kspread_layout.o: /usr/lib/qt/include/qradiobt.h
kspread_layout.o: /usr/lib/qt/include/qtooltip.h /usr/lib/qt/include/qtimer.h
kspread_layout.o: /usr/local/kde/include/kcolordlg.h
kspread_layout.o: /usr/local/kde/include/kselect.h kspread.h
kspread_layout.o: /opt/kde/idl/controls.h /opt/kde/idl/parts.h
kspread_layout.o: /usr/local/kde/include/komlWriter.h
kspread_map.o: kspread_map.h /usr/local/kde/include/komlParser.h
kspread_map.o: /usr/include/string.h /usr/include/features.h
kspread_map.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
kspread_map.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
kspread_map.o: /usr/local/include/ministl/vector.h
kspread_map.o: /usr/local/include/ministl/ministl.h
kspread_map.o: /usr/local/include/ministl/bool.h
kspread_map.o: /usr/local/include/ministl/defalloc.h
kspread_map.o: /usr/local/kde/include/torben.h
kspread_map.o: /usr/local/include/ministl/list.h
kspread_map.o: /usr/local/include/ministl/string
kspread_map.o: /usr/local/include/ministl/bstring.h /usr/include/ctype.h
kspread_map.o: /usr/include/endian.h /usr/include/bytesex.h
kspread_map.o: /usr/include/stdlib.h /usr/include/sys/types.h
kspread_map.o: /usr/include/gnu/types.h /usr/include/time.h
kspread_map.o: /usr/include/sys/select.h /usr/include/selectbits.h
kspread_map.o: /usr/include/alloca.h /usr/local/include/ministl/vector
kspread_map.o: /usr/local/kde/include/komlFeed.h /usr/lib/qt/include/qlist.h
kspread_map.o: /usr/lib/qt/include/qglist.h /usr/lib/qt/include/qcollect.h
kspread_map.o: /usr/lib/qt/include/qglobal.h /usr/lib/qt/include/qgeneric.h
kspread_map.o: /usr/lib/qt/include/qstring.h /usr/lib/qt/include/qarray.h
kspread_map.o: /usr/lib/qt/include/qgarray.h /usr/lib/qt/include/qshared.h
kspread_map.o: kspread_table.h /usr/local/kde/include/document_impl.h
kspread_map.o: /usr/local/kde/include/parts.h /usr/local/include/CORBA.h
kspread_map.o: /usr/local/include/mico/rtti.h
kspread_map.o: /usr/local/include/mico/config.h
kspread_map.o: /usr/local/include/mico/assert.h /usr/include/stdio.h
kspread_map.o: /usr/include/libio.h /usr/include/_G_config.h
kspread_map.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stdarg.h
kspread_map.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/va-alpha.h
kspread_map.o: /usr/include/stdio_lim.h ./Python-1.4/Include/assert.h
kspread_map.o: /usr/include/sys/time.h /usr/include/timebits.h
kspread_map.o: /usr/include/sys/socket.h /usr/include/socketbits.h
kspread_map.o: /usr/include/sockaddrcom.h /usr/include/asm/socket.h
kspread_map.o: /usr/include/asm/sockios.h /usr/local/include/ministl/map
kspread_map.o: /usr/local/include/ministl/map.h
kspread_map.o: /usr/local/include/ministl/function
kspread_map.o: /usr/local/include/ministl/function.h
kspread_map.o: /usr/local/include/ministl/pair
kspread_map.o: /usr/local/include/ministl/pair.h
kspread_map.o: /usr/local/include/ministl/set
kspread_map.o: /usr/local/include/ministl/set.h
kspread_map.o: /usr/local/include/ministl/list
kspread_map.o: /usr/local/include/mico/version.h
kspread_map.o: /usr/local/include/mico/types.h
kspread_map.o: /usr/local/include/mico/sequence.h
kspread_map.o: /usr/local/include/mico/array.h /usr/local/include/mico/var.h
kspread_map.o: /usr/local/include/mico/basic.h
kspread_map.o: /usr/local/include/mico/address.h
kspread_map.o: /usr/local/include/mico/ior.h
kspread_map.o: /usr/local/include/mico/transport.h
kspread_map.o: /usr/local/include/mico/shlib.h
kspread_map.o: /usr/local/include/mico/process.h
kspread_map.o: /usr/local/include/mico/buffer.h
kspread_map.o: /usr/local/include/mico/codec.h
kspread_map.o: /usr/local/include/mico/string.h
kspread_map.o: /usr/local/include/mico/except.h /usr/local/include/mico/any.h
kspread_map.o: /usr/local/include/mico/object.h /usr/local/include/mico/boa.h
kspread_map.o: /usr/local/include/mico/orb_fwd.h
kspread_map.o: /usr/local/include/mico/dii.h /usr/local/include/mico/dsi.h
kspread_map.o: /usr/local/include/mico/dispatch.h
kspread_map.o: /usr/local/include/mico/ir.h
kspread_map.o: /usr/local/include/mico/typecode.h
kspread_map.o: /usr/local/include/mico/orb.h
kspread_map.o: /usr/local/include/mico/ioptypes.h
kspread_map.o: /usr/local/include/mico/imr.h /usr/local/include/mico/throw.h
kspread_map.o: /opt/kde/idl/kom.h /usr/local/kde/include/controls.h
kspread_map.o: /usr/local/include/mico/template_impl.h
kspread_map.o: /usr/local/kde/include/component_impl.h
kspread_map.o: /usr/local/kde/include/kom.h
kspread_map.o: /usr/local/kde/include/base_impl.h /usr/lib/qt/include/qdict.h
kspread_map.o: /usr/lib/qt/include/qgdict.h /usr/local/kde/include/kom_var.h
kspread_map.o: /usr/local/kde/include/view_impl.h
kspread_map.o: /usr/local/kde/include/part_impl.h
kspread_map.o: /usr/local/kde/include/control_impl.h
kspread_map.o: /usr/lib/qt/include/qpen.h /usr/lib/qt/include/qcolor.h
kspread_map.o: /usr/lib/qt/include/qwindefs.h /usr/lib/qt/include/qobjdefs.h
kspread_map.o: /usr/lib/qt/include/qintdict.h /usr/lib/qt/include/qrect.h
kspread_map.o: /usr/lib/qt/include/qsize.h /usr/lib/qt/include/qpoint.h
kspread_map.o: kspread_autofill.h /usr/lib/qt/include/qstrlist.h
kspread_map.o: /usr/lib/qt/include/qdstream.h /usr/lib/qt/include/qiodev.h
kspread_map.o: kspread_layout.h /usr/lib/qt/include/qfont.h kspread_cell.h
kspread_map.o: /usr/lib/qt/include/qpainter.h /usr/lib/qt/include/qpaintd.h
kspread_map.o: /usr/lib/qt/include/qfontmet.h /usr/lib/qt/include/qfontinf.h
kspread_map.o: /usr/lib/qt/include/qregion.h /usr/lib/qt/include/qbrush.h
kspread_map.o: /usr/lib/qt/include/qpntarry.h /usr/lib/qt/include/qwmatrix.h
kspread_map.o: kspread_dlg_layout.h /usr/lib/qt/include/qwidget.h
kspread_map.o: /usr/lib/qt/include/qobject.h /usr/lib/qt/include/qevent.h
kspread_map.o: /usr/lib/qt/include/qpalette.h /usr/lib/qt/include/qcursor.h
kspread_map.o: /usr/lib/qt/include/qlistbox.h /usr/lib/qt/include/qtablevw.h
kspread_map.o: /usr/lib/qt/include/qframe.h /usr/lib/qt/include/qpixmap.h
kspread_map.o: /usr/lib/qt/include/qlined.h /usr/lib/qt/include/qcombo.h
kspread_map.o: /usr/lib/qt/include/qlabel.h /usr/lib/qt/include/qgrpbox.h
kspread_map.o: /usr/lib/qt/include/qtabdlg.h /usr/lib/qt/include/qdialog.h
kspread_map.o: /usr/lib/qt/include/qmsgbox.h /usr/lib/qt/include/qapp.h
kspread_map.o: /usr/lib/qt/include/qbttngrp.h /usr/lib/qt/include/qchkbox.h
kspread_map.o: /usr/lib/qt/include/qbutton.h /usr/lib/qt/include/qlcdnum.h
kspread_map.o: /usr/lib/qt/include/qbitarry.h /usr/lib/qt/include/qpushbt.h
kspread_map.o: /usr/lib/qt/include/qradiobt.h /usr/lib/qt/include/qscrbar.h
kspread_map.o: /usr/lib/qt/include/qrangect.h /usr/lib/qt/include/qdrawutl.h
kspread_map.o: /usr/lib/qt/include/qtooltip.h /usr/lib/qt/include/qtimer.h
kspread_map.o: /usr/local/kde/include/kcolordlg.h
kspread_map.o: /usr/local/kde/include/kselect.h kspread_doc.h
kspread_map.o: /usr/local/kde/include/part_frame_impl.h
kspread_map.o: /usr/local/kde/include/control_frame.h
kspread_map.o: /usr/local/kde/include/kwm.h /usr/include/X11/Xlib.h
kspread_map.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
kspread_map.o: /usr/include/X11/Xosdefs.h /usr/local/kde/include/kapp.h
kspread_map.o: /usr/local/kde/include/kconfig.h
kspread_map.o: /usr/local/kde/include/kconfigbase.h
kspread_map.o: /usr/local/kde/include/kconfigdata.h
kspread_map.o: /usr/lib/qt/include/qtstream.h /usr/lib/qt/include/qfile.h
kspread_map.o: /usr/local/kde/include/kdebug.h
kspread_map.o: /usr/local/kde/include/klocale.h /usr/local/kde/include/drag.h
kspread_map.o: /usr/lib/qt/include/qpopmenu.h /usr/lib/qt/include/qmenudta.h
kspread_map.o: /usr/lib/qt/include/qprinter.h kmetaedit.h kspread.h
kspread_map.o: /opt/kde/idl/controls.h /opt/kde/idl/parts.h kspread_python.h
kspread_map.o: ./Python-1.4/Include/Python.h
kspread_map.o: ./Python-1.4/Include/allobjects.h
kspread_map.o: /usr/local/kde/include/config.h /usr/include/errno.h
kspread_map.o: /usr/include/errnos.h /usr/include/linux/errno.h
kspread_map.o: /usr/include/asm/errno.h ./Python-1.4/Include/myproto.h
kspread_map.o: ./Python-1.4/Include/object.h ./Python-1.4/Include/objimpl.h
kspread_map.o: ./Python-1.4/Include/pydebug.h
kspread_map.o: ./Python-1.4/Include/accessobject.h
kspread_map.o: ./Python-1.4/Include/intobject.h
kspread_map.o: ./Python-1.4/Include/longobject.h
kspread_map.o: ./Python-1.4/Include/floatobject.h
kspread_map.o: ./Python-1.4/Include/complexobject.h
kspread_map.o: ./Python-1.4/Include/rangeobject.h
kspread_map.o: ./Python-1.4/Include/stringobject.h
kspread_map.o: ./Python-1.4/Include/tupleobject.h
kspread_map.o: ./Python-1.4/Include/listobject.h
kspread_map.o: ./Python-1.4/Include/mappingobject.h
kspread_map.o: ./Python-1.4/Include/methodobject.h
kspread_map.o: ./Python-1.4/Include/moduleobject.h
kspread_map.o: ./Python-1.4/Include/funcobject.h
kspread_map.o: ./Python-1.4/Include/classobject.h
kspread_map.o: ./Python-1.4/Include/fileobject.h
kspread_map.o: ./Python-1.4/Include/cobject.h
kspread_map.o: ./Python-1.4/Include/traceback.h
kspread_map.o: ./Python-1.4/Include/sliceobject.h
kspread_map.o: ./Python-1.4/Include/pyerrors.h
kspread_map.o: ./Python-1.4/Include/mymalloc.h
kspread_map.o: ./Python-1.4/Include/modsupport.h
kspread_map.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/varargs.h
kspread_map.o: ./Python-1.4/Include/ceval.h ./Python-1.4/Include/pythonrun.h
kspread_map.o: ./Python-1.4/Include/sysmodule.h
kspread_map.o: ./Python-1.4/Include/intrcheck.h ./Python-1.4/Include/import.h
kspread_map.o: ./Python-1.4/Include/bltinmodule.h
kspread_map.o: ./Python-1.4/Include/abstract.h
kspread_map.o: ./Python-1.4/Include/graminit.h kspread_undo.h
kspread_map.o: /usr/lib/qt/include/qstack.h kspread_view.h
kspread_map.o: /usr/local/kde/include/menu_impl.h
kspread_map.o: /usr/local/kde/include/kmenubar.h
kspread_map.o: /usr/lib/qt/include/qmenubar.h
kspread_map.o: /usr/local/kde/include/ktopwidget.h
kspread_map.o: /usr/local/kde/include/ktoolbar.h
kspread_map.o: /usr/local/kde/include/kcombo.h
kspread_map.o: /usr/local/kde/include/klined.h
kspread_map.o: /usr/local/kde/include/kstatusbar.h
kspread_map.o: /usr/local/kde/include/toolbar_impl.h kspread_tabbar.h
kspread_map.o: /usr/local/kde/include/komlWriter.h
kspread_dlg_layout.o: /usr/include/stdio.h /usr/include/libio.h
kspread_dlg_layout.o: /usr/include/features.h /usr/include/sys/cdefs.h
kspread_dlg_layout.o: /usr/include/gnu/stubs.h /usr/include/_G_config.h
kspread_dlg_layout.o: /usr/include/gnu/types.h
kspread_dlg_layout.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
kspread_dlg_layout.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stdarg.h
kspread_dlg_layout.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/va-alpha.h
kspread_dlg_layout.o: /usr/include/stdio_lim.h /usr/include/stdlib.h
kspread_dlg_layout.o: /usr/include/sys/types.h /usr/include/time.h
kspread_dlg_layout.o: /usr/include/endian.h /usr/include/bytesex.h
kspread_dlg_layout.o: /usr/include/sys/select.h /usr/include/selectbits.h
kspread_dlg_layout.o: /usr/include/alloca.h kspread_dlg_layout.h
kspread_dlg_layout.o: /usr/lib/qt/include/qwidget.h
kspread_dlg_layout.o: /usr/lib/qt/include/qwindefs.h
kspread_dlg_layout.o: /usr/lib/qt/include/qobjdefs.h
kspread_dlg_layout.o: /usr/lib/qt/include/qglobal.h
kspread_dlg_layout.o: /usr/lib/qt/include/qobject.h
kspread_dlg_layout.o: /usr/lib/qt/include/qstring.h
kspread_dlg_layout.o: /usr/lib/qt/include/qarray.h
kspread_dlg_layout.o: /usr/lib/qt/include/qgarray.h
kspread_dlg_layout.o: /usr/lib/qt/include/qshared.h
kspread_dlg_layout.o: /usr/lib/qt/include/qgeneric.h /usr/include/string.h
kspread_dlg_layout.o: /usr/lib/qt/include/qevent.h
kspread_dlg_layout.o: /usr/lib/qt/include/qrect.h /usr/lib/qt/include/qsize.h
kspread_dlg_layout.o: /usr/lib/qt/include/qpoint.h
kspread_dlg_layout.o: /usr/lib/qt/include/qpaintd.h
kspread_dlg_layout.o: /usr/lib/qt/include/qpalette.h
kspread_dlg_layout.o: /usr/lib/qt/include/qcolor.h
kspread_dlg_layout.o: /usr/lib/qt/include/qcursor.h
kspread_dlg_layout.o: /usr/lib/qt/include/qfont.h
kspread_dlg_layout.o: /usr/lib/qt/include/qfontmet.h
kspread_dlg_layout.o: /usr/lib/qt/include/qfontinf.h
kspread_dlg_layout.o: /usr/lib/qt/include/qlistbox.h
kspread_dlg_layout.o: /usr/lib/qt/include/qtablevw.h
kspread_dlg_layout.o: /usr/lib/qt/include/qframe.h
kspread_dlg_layout.o: /usr/lib/qt/include/qpixmap.h
kspread_dlg_layout.o: /usr/lib/qt/include/qlined.h
kspread_dlg_layout.o: /usr/lib/qt/include/qcombo.h
kspread_dlg_layout.o: /usr/lib/qt/include/qlabel.h
kspread_dlg_layout.o: /usr/lib/qt/include/qgrpbox.h
kspread_dlg_layout.o: /usr/lib/qt/include/qtabdlg.h
kspread_dlg_layout.o: /usr/lib/qt/include/qdialog.h
kspread_dlg_layout.o: /usr/lib/qt/include/qmsgbox.h
kspread_dlg_layout.o: /usr/lib/qt/include/qapp.h
kspread_dlg_layout.o: /usr/lib/qt/include/qbttngrp.h
kspread_dlg_layout.o: /usr/lib/qt/include/qchkbox.h
kspread_dlg_layout.o: /usr/lib/qt/include/qbutton.h
kspread_dlg_layout.o: /usr/lib/qt/include/qlcdnum.h
kspread_dlg_layout.o: /usr/lib/qt/include/qbitarry.h
kspread_dlg_layout.o: /usr/lib/qt/include/qpushbt.h
kspread_dlg_layout.o: /usr/lib/qt/include/qradiobt.h
kspread_dlg_layout.o: /usr/lib/qt/include/qscrbar.h
kspread_dlg_layout.o: /usr/lib/qt/include/qrangect.h
kspread_dlg_layout.o: /usr/lib/qt/include/qdrawutl.h
kspread_dlg_layout.o: /usr/lib/qt/include/qpainter.h
kspread_dlg_layout.o: /usr/lib/qt/include/qregion.h
kspread_dlg_layout.o: /usr/lib/qt/include/qpen.h /usr/lib/qt/include/qbrush.h
kspread_dlg_layout.o: /usr/lib/qt/include/qpntarry.h
kspread_dlg_layout.o: /usr/lib/qt/include/qwmatrix.h
kspread_dlg_layout.o: /usr/lib/qt/include/qtooltip.h
kspread_dlg_layout.o: /usr/lib/qt/include/qtimer.h
kspread_dlg_layout.o: /usr/local/kde/include/kcolordlg.h
kspread_dlg_layout.o: /usr/local/kde/include/kselect.h kspread_cell.h
kspread_dlg_layout.o: /usr/local/kde/include/komlParser.h
kspread_dlg_layout.o: /usr/local/include/ministl/vector.h
kspread_dlg_layout.o: /usr/local/include/ministl/ministl.h
kspread_dlg_layout.o: /usr/local/include/ministl/bool.h
kspread_dlg_layout.o: /usr/local/include/ministl/defalloc.h
kspread_dlg_layout.o: /usr/local/kde/include/torben.h
kspread_dlg_layout.o: /usr/local/include/ministl/list.h
kspread_dlg_layout.o: /usr/local/include/ministl/string
kspread_dlg_layout.o: /usr/local/include/ministl/bstring.h
kspread_dlg_layout.o: /usr/include/ctype.h /usr/local/include/ministl/vector
kspread_dlg_layout.o: /usr/local/kde/include/komlFeed.h
kspread_dlg_layout.o: /usr/lib/qt/include/qlist.h
kspread_dlg_layout.o: /usr/lib/qt/include/qglist.h
kspread_dlg_layout.o: /usr/lib/qt/include/qcollect.h kspread_layout.h
kspread_dlg_layout.o: kspread_undo.h kspread_doc.h
kspread_dlg_layout.o: /usr/local/kde/include/part_frame_impl.h
kspread_dlg_layout.o: /usr/local/kde/include/control_frame.h
kspread_dlg_layout.o: /usr/local/kde/include/kom.h /usr/local/include/CORBA.h
kspread_dlg_layout.o: /usr/local/include/mico/rtti.h
kspread_dlg_layout.o: /usr/local/include/mico/config.h
kspread_dlg_layout.o: /usr/local/include/mico/assert.h
kspread_dlg_layout.o: ./Python-1.4/Include/assert.h /usr/include/sys/time.h
kspread_dlg_layout.o: /usr/include/timebits.h /usr/include/sys/socket.h
kspread_dlg_layout.o: /usr/include/socketbits.h /usr/include/sockaddrcom.h
kspread_dlg_layout.o: /usr/include/asm/socket.h /usr/include/asm/sockios.h
kspread_dlg_layout.o: /usr/local/include/ministl/map
kspread_dlg_layout.o: /usr/local/include/ministl/map.h
kspread_dlg_layout.o: /usr/local/include/ministl/function
kspread_dlg_layout.o: /usr/local/include/ministl/function.h
kspread_dlg_layout.o: /usr/local/include/ministl/pair
kspread_dlg_layout.o: /usr/local/include/ministl/pair.h
kspread_dlg_layout.o: /usr/local/include/ministl/set
kspread_dlg_layout.o: /usr/local/include/ministl/set.h
kspread_dlg_layout.o: /usr/local/include/ministl/list
kspread_dlg_layout.o: /usr/local/include/mico/version.h
kspread_dlg_layout.o: /usr/local/include/mico/types.h
kspread_dlg_layout.o: /usr/local/include/mico/sequence.h
kspread_dlg_layout.o: /usr/local/include/mico/array.h
kspread_dlg_layout.o: /usr/local/include/mico/var.h
kspread_dlg_layout.o: /usr/local/include/mico/basic.h
kspread_dlg_layout.o: /usr/local/include/mico/address.h
kspread_dlg_layout.o: /usr/local/include/mico/ior.h
kspread_dlg_layout.o: /usr/local/include/mico/transport.h
kspread_dlg_layout.o: /usr/local/include/mico/shlib.h
kspread_dlg_layout.o: /usr/local/include/mico/process.h
kspread_dlg_layout.o: /usr/local/include/mico/buffer.h
kspread_dlg_layout.o: /usr/local/include/mico/codec.h
kspread_dlg_layout.o: /usr/local/include/mico/string.h
kspread_dlg_layout.o: /usr/local/include/mico/except.h
kspread_dlg_layout.o: /usr/local/include/mico/any.h
kspread_dlg_layout.o: /usr/local/include/mico/object.h
kspread_dlg_layout.o: /usr/local/include/mico/boa.h
kspread_dlg_layout.o: /usr/local/include/mico/orb_fwd.h
kspread_dlg_layout.o: /usr/local/include/mico/dii.h
kspread_dlg_layout.o: /usr/local/include/mico/dsi.h
kspread_dlg_layout.o: /usr/local/include/mico/dispatch.h
kspread_dlg_layout.o: /usr/local/include/mico/ir.h
kspread_dlg_layout.o: /usr/local/include/mico/typecode.h
kspread_dlg_layout.o: /usr/local/include/mico/orb.h
kspread_dlg_layout.o: /usr/local/include/mico/ioptypes.h
kspread_dlg_layout.o: /usr/local/include/mico/imr.h
kspread_dlg_layout.o: /usr/local/include/mico/throw.h
kspread_dlg_layout.o: /usr/local/include/mico/template_impl.h
kspread_dlg_layout.o: /usr/local/kde/include/controls.h /opt/kde/idl/kom.h
kspread_dlg_layout.o: /usr/local/kde/include/control_impl.h
kspread_dlg_layout.o: /usr/local/kde/include/component_impl.h
kspread_dlg_layout.o: /usr/local/kde/include/base_impl.h
kspread_dlg_layout.o: /usr/lib/qt/include/qdict.h
kspread_dlg_layout.o: /usr/lib/qt/include/qgdict.h
kspread_dlg_layout.o: /usr/local/kde/include/kom_var.h
kspread_dlg_layout.o: /usr/local/kde/include/kwm.h /usr/include/X11/Xlib.h
kspread_dlg_layout.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
kspread_dlg_layout.o: /usr/include/X11/Xosdefs.h
kspread_dlg_layout.o: /usr/local/kde/include/kapp.h
kspread_dlg_layout.o: /usr/local/kde/include/kconfig.h
kspread_dlg_layout.o: /usr/local/kde/include/kconfigbase.h
kspread_dlg_layout.o: /usr/lib/qt/include/qstrlist.h
kspread_dlg_layout.o: /usr/lib/qt/include/qdstream.h
kspread_dlg_layout.o: /usr/lib/qt/include/qiodev.h
kspread_dlg_layout.o: /usr/local/kde/include/kconfigdata.h
kspread_dlg_layout.o: /usr/lib/qt/include/qtstream.h
kspread_dlg_layout.o: /usr/lib/qt/include/qfile.h
kspread_dlg_layout.o: /usr/local/kde/include/kdebug.h
kspread_dlg_layout.o: /usr/local/kde/include/klocale.h
kspread_dlg_layout.o: /usr/lib/qt/include/qintdict.h
kspread_dlg_layout.o: /usr/local/kde/include/drag.h
kspread_dlg_layout.o: /usr/lib/qt/include/qpopmenu.h
kspread_dlg_layout.o: /usr/lib/qt/include/qmenudta.h
kspread_dlg_layout.o: /usr/local/kde/include/parts.h
kspread_dlg_layout.o: /usr/local/kde/include/part_impl.h
kspread_dlg_layout.o: /usr/local/kde/include/document_impl.h
kspread_dlg_layout.o: /usr/local/kde/include/view_impl.h
kspread_dlg_layout.o: /usr/lib/qt/include/qprinter.h kmetaedit.h kspread.h
kspread_dlg_layout.o: /opt/kde/idl/controls.h /opt/kde/idl/parts.h
kspread_dlg_layout.o: kspread_python.h ./Python-1.4/Include/Python.h
kspread_dlg_layout.o: ./Python-1.4/Include/allobjects.h
kspread_dlg_layout.o: /usr/local/kde/include/config.h /usr/include/errno.h
kspread_dlg_layout.o: /usr/include/errnos.h /usr/include/linux/errno.h
kspread_dlg_layout.o: /usr/include/asm/errno.h ./Python-1.4/Include/myproto.h
kspread_dlg_layout.o: ./Python-1.4/Include/object.h
kspread_dlg_layout.o: ./Python-1.4/Include/objimpl.h
kspread_dlg_layout.o: ./Python-1.4/Include/pydebug.h
kspread_dlg_layout.o: ./Python-1.4/Include/accessobject.h
kspread_dlg_layout.o: ./Python-1.4/Include/intobject.h
kspread_dlg_layout.o: ./Python-1.4/Include/longobject.h
kspread_dlg_layout.o: ./Python-1.4/Include/floatobject.h
kspread_dlg_layout.o: ./Python-1.4/Include/complexobject.h
kspread_dlg_layout.o: ./Python-1.4/Include/rangeobject.h
kspread_dlg_layout.o: ./Python-1.4/Include/stringobject.h
kspread_dlg_layout.o: ./Python-1.4/Include/tupleobject.h
kspread_dlg_layout.o: ./Python-1.4/Include/listobject.h
kspread_dlg_layout.o: ./Python-1.4/Include/mappingobject.h
kspread_dlg_layout.o: ./Python-1.4/Include/methodobject.h
kspread_dlg_layout.o: ./Python-1.4/Include/moduleobject.h
kspread_dlg_layout.o: ./Python-1.4/Include/funcobject.h
kspread_dlg_layout.o: ./Python-1.4/Include/classobject.h
kspread_dlg_layout.o: ./Python-1.4/Include/fileobject.h
kspread_dlg_layout.o: ./Python-1.4/Include/cobject.h
kspread_dlg_layout.o: ./Python-1.4/Include/traceback.h
kspread_dlg_layout.o: ./Python-1.4/Include/sliceobject.h
kspread_dlg_layout.o: ./Python-1.4/Include/pyerrors.h
kspread_dlg_layout.o: ./Python-1.4/Include/mymalloc.h
kspread_dlg_layout.o: ./Python-1.4/Include/modsupport.h
kspread_dlg_layout.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/varargs.h
kspread_dlg_layout.o: ./Python-1.4/Include/ceval.h
kspread_dlg_layout.o: ./Python-1.4/Include/pythonrun.h
kspread_dlg_layout.o: ./Python-1.4/Include/sysmodule.h
kspread_dlg_layout.o: ./Python-1.4/Include/intrcheck.h
kspread_dlg_layout.o: ./Python-1.4/Include/import.h
kspread_dlg_layout.o: ./Python-1.4/Include/bltinmodule.h
kspread_dlg_layout.o: ./Python-1.4/Include/abstract.h
kspread_dlg_layout.o: ./Python-1.4/Include/graminit.h kspread_view.h
kspread_dlg_layout.o: /usr/local/kde/include/menu_impl.h
kspread_dlg_layout.o: /usr/local/kde/include/kmenubar.h
kspread_dlg_layout.o: /usr/lib/qt/include/qmenubar.h
kspread_dlg_layout.o: /usr/local/kde/include/ktopwidget.h
kspread_dlg_layout.o: /usr/local/kde/include/ktoolbar.h
kspread_dlg_layout.o: /usr/local/kde/include/kcombo.h
kspread_dlg_layout.o: /usr/local/kde/include/klined.h
kspread_dlg_layout.o: /usr/local/kde/include/kstatusbar.h
kspread_dlg_layout.o: /usr/local/kde/include/toolbar_impl.h kspread_tabbar.h
kspread_dlg_layout.o: kspread_table.h kspread_autofill.h kspread_map.h
kspread_dlg_layout.o: /usr/lib/qt/include/qstack.h
kspread_dlg_scripts.o: kspread_dlg_scripts.h /usr/lib/qt/include/qdialog.h
kspread_dlg_scripts.o: /usr/lib/qt/include/qwidget.h
kspread_dlg_scripts.o: /usr/lib/qt/include/qwindefs.h
kspread_dlg_scripts.o: /usr/lib/qt/include/qobjdefs.h
kspread_dlg_scripts.o: /usr/lib/qt/include/qglobal.h
kspread_dlg_scripts.o: /usr/lib/qt/include/qobject.h
kspread_dlg_scripts.o: /usr/lib/qt/include/qstring.h
kspread_dlg_scripts.o: /usr/lib/qt/include/qarray.h
kspread_dlg_scripts.o: /usr/lib/qt/include/qgarray.h
kspread_dlg_scripts.o: /usr/lib/qt/include/qshared.h
kspread_dlg_scripts.o: /usr/lib/qt/include/qgeneric.h /usr/include/string.h
kspread_dlg_scripts.o: /usr/include/features.h /usr/include/sys/cdefs.h
kspread_dlg_scripts.o: /usr/include/gnu/stubs.h
kspread_dlg_scripts.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
kspread_dlg_scripts.o: /usr/lib/qt/include/qevent.h
kspread_dlg_scripts.o: /usr/lib/qt/include/qrect.h
kspread_dlg_scripts.o: /usr/lib/qt/include/qsize.h
kspread_dlg_scripts.o: /usr/lib/qt/include/qpoint.h
kspread_dlg_scripts.o: /usr/lib/qt/include/qpaintd.h
kspread_dlg_scripts.o: /usr/lib/qt/include/qpalette.h
kspread_dlg_scripts.o: /usr/lib/qt/include/qcolor.h
kspread_dlg_scripts.o: /usr/lib/qt/include/qcursor.h
kspread_dlg_scripts.o: /usr/lib/qt/include/qfont.h
kspread_dlg_scripts.o: /usr/lib/qt/include/qfontmet.h
kspread_dlg_scripts.o: /usr/lib/qt/include/qfontinf.h
kspread_dlg_scripts.o: /usr/lib/qt/include/qstrlist.h
kspread_dlg_scripts.o: /usr/lib/qt/include/qlist.h
kspread_dlg_scripts.o: /usr/lib/qt/include/qglist.h
kspread_dlg_scripts.o: /usr/lib/qt/include/qcollect.h
kspread_dlg_scripts.o: /usr/lib/qt/include/qdstream.h
kspread_dlg_scripts.o: /usr/lib/qt/include/qiodev.h
kspread_dlg_scripts.o: /usr/lib/qt/include/qlined.h
kspread_dlg_scripts.o: /usr/lib/qt/include/qlistbox.h
kspread_dlg_scripts.o: /usr/lib/qt/include/qtablevw.h
kspread_dlg_scripts.o: /usr/lib/qt/include/qframe.h
kspread_dlg_scripts.o: /usr/lib/qt/include/qpixmap.h
kspread_dlg_scripts.o: /usr/lib/qt/include/qpushbt.h
kspread_dlg_scripts.o: /usr/lib/qt/include/qbutton.h kmetaedit.h
kspread_dlg_scripts.o: /usr/lib/qt/include/qlabel.h
kspread_dlg_scripts.o: /usr/lib/qt/include/qdir.h
kspread_dlg_scripts.o: /usr/lib/qt/include/qfileinf.h
kspread_dlg_scripts.o: /usr/lib/qt/include/qfile.h /usr/include/stdio.h
kspread_dlg_scripts.o: /usr/include/libio.h /usr/include/_G_config.h
kspread_dlg_scripts.o: /usr/include/gnu/types.h
kspread_dlg_scripts.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stdarg.h
kspread_dlg_scripts.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/va-alpha.h
kspread_dlg_scripts.o: /usr/include/stdio_lim.h /usr/lib/qt/include/qdatetm.h
kspread_dlg_scripts.o: /usr/include/unistd.h /usr/include/posix_opt.h
kspread_dlg_scripts.o: /usr/include/confname.h /usr/lib/qt/include/qmsgbox.h
kspread_dlg_scripts.o: /usr/local/kde/include/kapp.h
kspread_dlg_scripts.o: /usr/local/kde/include/kconfig.h
kspread_dlg_scripts.o: /usr/local/kde/include/kconfigbase.h
kspread_dlg_scripts.o: /usr/local/kde/include/kconfigdata.h
kspread_dlg_scripts.o: /usr/lib/qt/include/qdict.h
kspread_dlg_scripts.o: /usr/lib/qt/include/qgdict.h
kspread_dlg_scripts.o: /usr/lib/qt/include/qtstream.h
kspread_dlg_scripts.o: /usr/local/kde/include/kdebug.h
kspread_dlg_scripts.o: /usr/local/kde/include/klocale.h
kspread_dlg_scripts.o: /usr/lib/qt/include/qintdict.h
kspread_dlg_scripts.o: /usr/local/kde/include/drag.h
kspread_dlg_scripts.o: /usr/lib/qt/include/qapp.h /usr/include/X11/X.h
kspread_dlg_scripts.o: /usr/lib/qt/include/qpopmenu.h
kspread_dlg_scripts.o: /usr/lib/qt/include/qmenudta.h /usr/include/stdlib.h
kspread_dlg_scripts.o: /usr/include/sys/types.h /usr/include/time.h
kspread_dlg_scripts.o: /usr/include/endian.h /usr/include/bytesex.h
kspread_dlg_scripts.o: /usr/include/sys/select.h /usr/include/selectbits.h
kspread_dlg_scripts.o: /usr/include/alloca.h
kspread_cell.o: /usr/include/stdlib.h /usr/include/features.h
kspread_cell.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
kspread_cell.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
kspread_cell.o: /usr/include/sys/types.h /usr/include/gnu/types.h
kspread_cell.o: /usr/include/time.h /usr/include/endian.h
kspread_cell.o: /usr/include/bytesex.h /usr/include/sys/select.h
kspread_cell.o: /usr/include/selectbits.h /usr/include/alloca.h
kspread_cell.o: /usr/include/stdio.h /usr/include/libio.h
kspread_cell.o: /usr/include/_G_config.h
kspread_cell.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stdarg.h
kspread_cell.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/va-alpha.h
kspread_cell.o: /usr/include/stdio_lim.h /usr/include/ctype.h
kspread_cell.o: /usr/include/math.h /usr/include/huge_val.h
kspread_cell.o: /usr/include/mathcalls.h
kspread_cell.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/float.h
kspread_cell.o: /usr/lib/qt/include/qpainter.h /usr/lib/qt/include/qpaintd.h
kspread_cell.o: /usr/lib/qt/include/qwindefs.h /usr/lib/qt/include/qobjdefs.h
kspread_cell.o: /usr/lib/qt/include/qglobal.h /usr/lib/qt/include/qrect.h
kspread_cell.o: /usr/lib/qt/include/qsize.h /usr/lib/qt/include/qpoint.h
kspread_cell.o: /usr/lib/qt/include/qcolor.h /usr/lib/qt/include/qfontmet.h
kspread_cell.o: /usr/lib/qt/include/qfont.h /usr/lib/qt/include/qstring.h
kspread_cell.o: /usr/lib/qt/include/qarray.h /usr/lib/qt/include/qgarray.h
kspread_cell.o: /usr/lib/qt/include/qshared.h /usr/lib/qt/include/qgeneric.h
kspread_cell.o: /usr/include/string.h /usr/lib/qt/include/qfontinf.h
kspread_cell.o: /usr/lib/qt/include/qregion.h /usr/lib/qt/include/qpen.h
kspread_cell.o: /usr/lib/qt/include/qbrush.h /usr/lib/qt/include/qpntarry.h
kspread_cell.o: /usr/lib/qt/include/qwmatrix.h /usr/lib/qt/include/qdrawutl.h
kspread_cell.o: /usr/lib/qt/include/qpalette.h /usr/lib/qt/include/qkeycode.h
kspread_cell.o: /usr/lib/qt/include/qregexp.h /usr/lib/qt/include/qprinter.h
kspread_cell.o: /usr/lib/qt/include/qcursor.h kspread_table.h
kspread_cell.o: /usr/local/kde/include/komlParser.h
kspread_cell.o: /usr/local/include/ministl/vector.h
kspread_cell.o: /usr/local/include/ministl/ministl.h
kspread_cell.o: /usr/local/include/ministl/bool.h
kspread_cell.o: /usr/local/include/ministl/defalloc.h
kspread_cell.o: /usr/local/kde/include/torben.h
kspread_cell.o: /usr/local/include/ministl/list.h
kspread_cell.o: /usr/local/include/ministl/string
kspread_cell.o: /usr/local/include/ministl/bstring.h
kspread_cell.o: /usr/local/include/ministl/vector
kspread_cell.o: /usr/local/kde/include/komlFeed.h
kspread_cell.o: /usr/local/kde/include/document_impl.h
kspread_cell.o: /usr/local/kde/include/parts.h /usr/local/include/CORBA.h
kspread_cell.o: /usr/local/include/mico/rtti.h
kspread_cell.o: /usr/local/include/mico/config.h
kspread_cell.o: /usr/local/include/mico/assert.h
kspread_cell.o: ./Python-1.4/Include/assert.h /usr/include/sys/time.h
kspread_cell.o: /usr/include/timebits.h /usr/include/sys/socket.h
kspread_cell.o: /usr/include/socketbits.h /usr/include/sockaddrcom.h
kspread_cell.o: /usr/include/asm/socket.h /usr/include/asm/sockios.h
kspread_cell.o: /usr/local/include/ministl/map
kspread_cell.o: /usr/local/include/ministl/map.h
kspread_cell.o: /usr/local/include/ministl/function
kspread_cell.o: /usr/local/include/ministl/function.h
kspread_cell.o: /usr/local/include/ministl/pair
kspread_cell.o: /usr/local/include/ministl/pair.h
kspread_cell.o: /usr/local/include/ministl/set
kspread_cell.o: /usr/local/include/ministl/set.h
kspread_cell.o: /usr/local/include/ministl/list
kspread_cell.o: /usr/local/include/mico/version.h
kspread_cell.o: /usr/local/include/mico/types.h
kspread_cell.o: /usr/local/include/mico/sequence.h
kspread_cell.o: /usr/local/include/mico/array.h /usr/local/include/mico/var.h
kspread_cell.o: /usr/local/include/mico/basic.h
kspread_cell.o: /usr/local/include/mico/address.h
kspread_cell.o: /usr/local/include/mico/ior.h
kspread_cell.o: /usr/local/include/mico/transport.h
kspread_cell.o: /usr/local/include/mico/shlib.h
kspread_cell.o: /usr/local/include/mico/process.h
kspread_cell.o: /usr/local/include/mico/buffer.h
kspread_cell.o: /usr/local/include/mico/codec.h
kspread_cell.o: /usr/local/include/mico/string.h
kspread_cell.o: /usr/local/include/mico/except.h
kspread_cell.o: /usr/local/include/mico/any.h
kspread_cell.o: /usr/local/include/mico/object.h
kspread_cell.o: /usr/local/include/mico/boa.h
kspread_cell.o: /usr/local/include/mico/orb_fwd.h
kspread_cell.o: /usr/local/include/mico/dii.h /usr/local/include/mico/dsi.h
kspread_cell.o: /usr/local/include/mico/dispatch.h
kspread_cell.o: /usr/local/include/mico/ir.h
kspread_cell.o: /usr/local/include/mico/typecode.h
kspread_cell.o: /usr/local/include/mico/orb.h
kspread_cell.o: /usr/local/include/mico/ioptypes.h
kspread_cell.o: /usr/local/include/mico/imr.h /usr/local/include/mico/throw.h
kspread_cell.o: /opt/kde/idl/kom.h /usr/local/kde/include/controls.h
kspread_cell.o: /usr/local/include/mico/template_impl.h
kspread_cell.o: /usr/local/kde/include/component_impl.h
kspread_cell.o: /usr/local/kde/include/kom.h
kspread_cell.o: /usr/local/kde/include/base_impl.h
kspread_cell.o: /usr/lib/qt/include/qlist.h /usr/lib/qt/include/qglist.h
kspread_cell.o: /usr/lib/qt/include/qcollect.h /usr/lib/qt/include/qdict.h
kspread_cell.o: /usr/lib/qt/include/qgdict.h /usr/local/kde/include/kom_var.h
kspread_cell.o: /usr/local/kde/include/view_impl.h
kspread_cell.o: /usr/local/kde/include/part_impl.h
kspread_cell.o: /usr/local/kde/include/control_impl.h
kspread_cell.o: /usr/lib/qt/include/qintdict.h kspread_autofill.h
kspread_cell.o: /usr/lib/qt/include/qstrlist.h /usr/lib/qt/include/qdstream.h
kspread_cell.o: /usr/lib/qt/include/qiodev.h kspread_layout.h kspread_cell.h
kspread_cell.o: kspread_dlg_layout.h /usr/lib/qt/include/qwidget.h
kspread_cell.o: /usr/lib/qt/include/qobject.h /usr/lib/qt/include/qevent.h
kspread_cell.o: /usr/lib/qt/include/qlistbox.h /usr/lib/qt/include/qtablevw.h
kspread_cell.o: /usr/lib/qt/include/qframe.h /usr/lib/qt/include/qpixmap.h
kspread_cell.o: /usr/lib/qt/include/qlined.h /usr/lib/qt/include/qcombo.h
kspread_cell.o: /usr/lib/qt/include/qlabel.h /usr/lib/qt/include/qgrpbox.h
kspread_cell.o: /usr/lib/qt/include/qtabdlg.h /usr/lib/qt/include/qdialog.h
kspread_cell.o: /usr/lib/qt/include/qmsgbox.h /usr/lib/qt/include/qapp.h
kspread_cell.o: /usr/lib/qt/include/qbttngrp.h /usr/lib/qt/include/qchkbox.h
kspread_cell.o: /usr/lib/qt/include/qbutton.h /usr/lib/qt/include/qlcdnum.h
kspread_cell.o: /usr/lib/qt/include/qbitarry.h /usr/lib/qt/include/qpushbt.h
kspread_cell.o: /usr/lib/qt/include/qradiobt.h /usr/lib/qt/include/qscrbar.h
kspread_cell.o: /usr/lib/qt/include/qrangect.h /usr/lib/qt/include/qtooltip.h
kspread_cell.o: /usr/lib/qt/include/qtimer.h
kspread_cell.o: /usr/local/kde/include/kcolordlg.h
kspread_cell.o: /usr/local/kde/include/kselect.h kspread_view.h
kspread_cell.o: /usr/local/kde/include/part_frame_impl.h
kspread_cell.o: /usr/local/kde/include/control_frame.h
kspread_cell.o: /usr/local/kde/include/kwm.h /usr/include/X11/Xlib.h
kspread_cell.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
kspread_cell.o: /usr/include/X11/Xosdefs.h /usr/local/kde/include/kapp.h
kspread_cell.o: /usr/local/kde/include/kconfig.h
kspread_cell.o: /usr/local/kde/include/kconfigbase.h
kspread_cell.o: /usr/local/kde/include/kconfigdata.h
kspread_cell.o: /usr/lib/qt/include/qtstream.h /usr/lib/qt/include/qfile.h
kspread_cell.o: /usr/local/kde/include/kdebug.h
kspread_cell.o: /usr/local/kde/include/klocale.h
kspread_cell.o: /usr/local/kde/include/drag.h /usr/lib/qt/include/qpopmenu.h
kspread_cell.o: /usr/lib/qt/include/qmenudta.h
kspread_cell.o: /usr/local/kde/include/menu_impl.h
kspread_cell.o: /usr/local/kde/include/kmenubar.h
kspread_cell.o: /usr/lib/qt/include/qmenubar.h
kspread_cell.o: /usr/local/kde/include/ktopwidget.h
kspread_cell.o: /usr/local/kde/include/ktoolbar.h
kspread_cell.o: /usr/local/kde/include/kcombo.h
kspread_cell.o: /usr/local/kde/include/klined.h
kspread_cell.o: /usr/local/kde/include/kstatusbar.h
kspread_cell.o: /usr/local/kde/include/toolbar_impl.h kspread_tabbar.h
kspread_cell.o: kspread.h /opt/kde/idl/controls.h /opt/kde/idl/parts.h
kspread_cell.o: kspread_python.h ./Python-1.4/Include/Python.h
kspread_cell.o: ./Python-1.4/Include/allobjects.h
kspread_cell.o: /usr/local/kde/include/config.h /usr/include/errno.h
kspread_cell.o: /usr/include/errnos.h /usr/include/linux/errno.h
kspread_cell.o: /usr/include/asm/errno.h ./Python-1.4/Include/myproto.h
kspread_cell.o: ./Python-1.4/Include/object.h ./Python-1.4/Include/objimpl.h
kspread_cell.o: ./Python-1.4/Include/pydebug.h
kspread_cell.o: ./Python-1.4/Include/accessobject.h
kspread_cell.o: ./Python-1.4/Include/intobject.h
kspread_cell.o: ./Python-1.4/Include/longobject.h
kspread_cell.o: ./Python-1.4/Include/floatobject.h
kspread_cell.o: ./Python-1.4/Include/complexobject.h
kspread_cell.o: ./Python-1.4/Include/rangeobject.h
kspread_cell.o: ./Python-1.4/Include/stringobject.h
kspread_cell.o: ./Python-1.4/Include/tupleobject.h
kspread_cell.o: ./Python-1.4/Include/listobject.h
kspread_cell.o: ./Python-1.4/Include/mappingobject.h
kspread_cell.o: ./Python-1.4/Include/methodobject.h
kspread_cell.o: ./Python-1.4/Include/moduleobject.h
kspread_cell.o: ./Python-1.4/Include/funcobject.h
kspread_cell.o: ./Python-1.4/Include/classobject.h
kspread_cell.o: ./Python-1.4/Include/fileobject.h
kspread_cell.o: ./Python-1.4/Include/cobject.h
kspread_cell.o: ./Python-1.4/Include/traceback.h
kspread_cell.o: ./Python-1.4/Include/sliceobject.h
kspread_cell.o: ./Python-1.4/Include/pyerrors.h
kspread_cell.o: ./Python-1.4/Include/mymalloc.h
kspread_cell.o: ./Python-1.4/Include/modsupport.h
kspread_cell.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/varargs.h
kspread_cell.o: ./Python-1.4/Include/ceval.h ./Python-1.4/Include/pythonrun.h
kspread_cell.o: ./Python-1.4/Include/sysmodule.h
kspread_cell.o: ./Python-1.4/Include/intrcheck.h
kspread_cell.o: ./Python-1.4/Include/import.h
kspread_cell.o: ./Python-1.4/Include/bltinmodule.h
kspread_cell.o: ./Python-1.4/Include/abstract.h
kspread_cell.o: ./Python-1.4/Include/graminit.h kspread_map.h kspread_doc.h
kspread_cell.o: kmetaedit.h kspread_undo.h /usr/lib/qt/include/qstack.h
kspread_cell.o: /usr/local/kde/include/koStream.h
kspread_cell.o: /usr/local/kde/include/komlWriter.h
kspread_table.o: /usr/include/stdlib.h /usr/include/features.h
kspread_table.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
kspread_table.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stddef.h
kspread_table.o: /usr/include/sys/types.h /usr/include/gnu/types.h
kspread_table.o: /usr/include/time.h /usr/include/endian.h
kspread_table.o: /usr/include/bytesex.h /usr/include/sys/select.h
kspread_table.o: /usr/include/selectbits.h /usr/include/alloca.h
kspread_table.o: /usr/include/stdio.h /usr/include/libio.h
kspread_table.o: /usr/include/_G_config.h
kspread_table.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/stdarg.h
kspread_table.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/va-alpha.h
kspread_table.o: /usr/include/stdio_lim.h /usr/include/ctype.h
kspread_table.o: /usr/include/math.h /usr/include/huge_val.h
kspread_table.o: /usr/include/mathcalls.h
kspread_table.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/float.h
kspread_table.o: /usr/lib/qt/include/qmsgbox.h /usr/lib/qt/include/qdialog.h
kspread_table.o: /usr/lib/qt/include/qwidget.h /usr/lib/qt/include/qwindefs.h
kspread_table.o: /usr/lib/qt/include/qobjdefs.h /usr/lib/qt/include/qglobal.h
kspread_table.o: /usr/lib/qt/include/qobject.h /usr/lib/qt/include/qstring.h
kspread_table.o: /usr/lib/qt/include/qarray.h /usr/lib/qt/include/qgarray.h
kspread_table.o: /usr/lib/qt/include/qshared.h /usr/lib/qt/include/qgeneric.h
kspread_table.o: /usr/include/string.h /usr/lib/qt/include/qevent.h
kspread_table.o: /usr/lib/qt/include/qrect.h /usr/lib/qt/include/qsize.h
kspread_table.o: /usr/lib/qt/include/qpoint.h /usr/lib/qt/include/qpaintd.h
kspread_table.o: /usr/lib/qt/include/qpalette.h /usr/lib/qt/include/qcolor.h
kspread_table.o: /usr/lib/qt/include/qcursor.h /usr/lib/qt/include/qfont.h
kspread_table.o: /usr/lib/qt/include/qfontmet.h
kspread_table.o: /usr/lib/qt/include/qfontinf.h
kspread_table.o: /usr/lib/qt/include/qpainter.h /usr/lib/qt/include/qregion.h
kspread_table.o: /usr/lib/qt/include/qpen.h /usr/lib/qt/include/qbrush.h
kspread_table.o: /usr/lib/qt/include/qpntarry.h
kspread_table.o: /usr/lib/qt/include/qwmatrix.h
kspread_table.o: /usr/lib/qt/include/qdrawutl.h
kspread_table.o: /usr/lib/qt/include/qkeycode.h /usr/lib/qt/include/qregexp.h
kspread_table.o: /usr/lib/qt/include/qprinter.h /usr/lib/qt/include/qstack.h
kspread_table.o: /usr/lib/qt/include/qglist.h /usr/lib/qt/include/qcollect.h
kspread_table.o: /usr/lib/qt/include/qbuffer.h /usr/lib/qt/include/qiodev.h
kspread_table.o: kspread_table.h /usr/local/kde/include/komlParser.h
kspread_table.o: /usr/local/include/ministl/vector.h
kspread_table.o: /usr/local/include/ministl/ministl.h
kspread_table.o: /usr/local/include/ministl/bool.h
kspread_table.o: /usr/local/include/ministl/defalloc.h
kspread_table.o: /usr/local/kde/include/torben.h
kspread_table.o: /usr/local/include/ministl/list.h
kspread_table.o: /usr/local/include/ministl/string
kspread_table.o: /usr/local/include/ministl/bstring.h
kspread_table.o: /usr/local/include/ministl/vector
kspread_table.o: /usr/local/kde/include/komlFeed.h
kspread_table.o: /usr/local/kde/include/document_impl.h
kspread_table.o: /usr/local/kde/include/parts.h /usr/local/include/CORBA.h
kspread_table.o: /usr/local/include/mico/rtti.h
kspread_table.o: /usr/local/include/mico/config.h
kspread_table.o: /usr/local/include/mico/assert.h
kspread_table.o: ./Python-1.4/Include/assert.h /usr/include/sys/time.h
kspread_table.o: /usr/include/timebits.h /usr/include/sys/socket.h
kspread_table.o: /usr/include/socketbits.h /usr/include/sockaddrcom.h
kspread_table.o: /usr/include/asm/socket.h /usr/include/asm/sockios.h
kspread_table.o: /usr/local/include/ministl/map
kspread_table.o: /usr/local/include/ministl/map.h
kspread_table.o: /usr/local/include/ministl/function
kspread_table.o: /usr/local/include/ministl/function.h
kspread_table.o: /usr/local/include/ministl/pair
kspread_table.o: /usr/local/include/ministl/pair.h
kspread_table.o: /usr/local/include/ministl/set
kspread_table.o: /usr/local/include/ministl/set.h
kspread_table.o: /usr/local/include/ministl/list
kspread_table.o: /usr/local/include/mico/version.h
kspread_table.o: /usr/local/include/mico/types.h
kspread_table.o: /usr/local/include/mico/sequence.h
kspread_table.o: /usr/local/include/mico/array.h
kspread_table.o: /usr/local/include/mico/var.h
kspread_table.o: /usr/local/include/mico/basic.h
kspread_table.o: /usr/local/include/mico/address.h
kspread_table.o: /usr/local/include/mico/ior.h
kspread_table.o: /usr/local/include/mico/transport.h
kspread_table.o: /usr/local/include/mico/shlib.h
kspread_table.o: /usr/local/include/mico/process.h
kspread_table.o: /usr/local/include/mico/buffer.h
kspread_table.o: /usr/local/include/mico/codec.h
kspread_table.o: /usr/local/include/mico/string.h
kspread_table.o: /usr/local/include/mico/except.h
kspread_table.o: /usr/local/include/mico/any.h
kspread_table.o: /usr/local/include/mico/object.h
kspread_table.o: /usr/local/include/mico/boa.h
kspread_table.o: /usr/local/include/mico/orb_fwd.h
kspread_table.o: /usr/local/include/mico/dii.h /usr/local/include/mico/dsi.h
kspread_table.o: /usr/local/include/mico/dispatch.h
kspread_table.o: /usr/local/include/mico/ir.h
kspread_table.o: /usr/local/include/mico/typecode.h
kspread_table.o: /usr/local/include/mico/orb.h
kspread_table.o: /usr/local/include/mico/ioptypes.h
kspread_table.o: /usr/local/include/mico/imr.h
kspread_table.o: /usr/local/include/mico/throw.h /opt/kde/idl/kom.h
kspread_table.o: /usr/local/kde/include/controls.h
kspread_table.o: /usr/local/include/mico/template_impl.h
kspread_table.o: /usr/local/kde/include/component_impl.h
kspread_table.o: /usr/local/kde/include/kom.h
kspread_table.o: /usr/local/kde/include/base_impl.h
kspread_table.o: /usr/lib/qt/include/qlist.h /usr/lib/qt/include/qdict.h
kspread_table.o: /usr/lib/qt/include/qgdict.h
kspread_table.o: /usr/local/kde/include/kom_var.h
kspread_table.o: /usr/local/kde/include/view_impl.h
kspread_table.o: /usr/local/kde/include/part_impl.h
kspread_table.o: /usr/local/kde/include/control_impl.h
kspread_table.o: /usr/lib/qt/include/qintdict.h kspread_autofill.h
kspread_table.o: /usr/lib/qt/include/qstrlist.h
kspread_table.o: /usr/lib/qt/include/qdstream.h kspread_layout.h
kspread_table.o: kspread_cell.h kspread_dlg_layout.h
kspread_table.o: /usr/lib/qt/include/qlistbox.h
kspread_table.o: /usr/lib/qt/include/qtablevw.h /usr/lib/qt/include/qframe.h
kspread_table.o: /usr/lib/qt/include/qpixmap.h /usr/lib/qt/include/qlined.h
kspread_table.o: /usr/lib/qt/include/qcombo.h /usr/lib/qt/include/qlabel.h
kspread_table.o: /usr/lib/qt/include/qgrpbox.h /usr/lib/qt/include/qtabdlg.h
kspread_table.o: /usr/lib/qt/include/qapp.h /usr/lib/qt/include/qbttngrp.h
kspread_table.o: /usr/lib/qt/include/qchkbox.h /usr/lib/qt/include/qbutton.h
kspread_table.o: /usr/lib/qt/include/qlcdnum.h /usr/lib/qt/include/qbitarry.h
kspread_table.o: /usr/lib/qt/include/qpushbt.h /usr/lib/qt/include/qradiobt.h
kspread_table.o: /usr/lib/qt/include/qscrbar.h /usr/lib/qt/include/qrangect.h
kspread_table.o: /usr/lib/qt/include/qtooltip.h /usr/lib/qt/include/qtimer.h
kspread_table.o: /usr/local/kde/include/kcolordlg.h
kspread_table.o: /usr/local/kde/include/kselect.h kspread_view.h
kspread_table.o: /usr/local/kde/include/part_frame_impl.h
kspread_table.o: /usr/local/kde/include/control_frame.h
kspread_table.o: /usr/local/kde/include/kwm.h /usr/include/X11/Xlib.h
kspread_table.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
kspread_table.o: /usr/include/X11/Xosdefs.h /usr/local/kde/include/kapp.h
kspread_table.o: /usr/local/kde/include/kconfig.h
kspread_table.o: /usr/local/kde/include/kconfigbase.h
kspread_table.o: /usr/local/kde/include/kconfigdata.h
kspread_table.o: /usr/lib/qt/include/qtstream.h /usr/lib/qt/include/qfile.h
kspread_table.o: /usr/local/kde/include/kdebug.h
kspread_table.o: /usr/local/kde/include/klocale.h
kspread_table.o: /usr/local/kde/include/drag.h /usr/lib/qt/include/qpopmenu.h
kspread_table.o: /usr/lib/qt/include/qmenudta.h
kspread_table.o: /usr/local/kde/include/menu_impl.h
kspread_table.o: /usr/local/kde/include/kmenubar.h
kspread_table.o: /usr/lib/qt/include/qmenubar.h
kspread_table.o: /usr/local/kde/include/ktopwidget.h
kspread_table.o: /usr/local/kde/include/ktoolbar.h
kspread_table.o: /usr/local/kde/include/kcombo.h
kspread_table.o: /usr/local/kde/include/klined.h
kspread_table.o: /usr/local/kde/include/kstatusbar.h
kspread_table.o: /usr/local/kde/include/toolbar_impl.h kspread_tabbar.h
kspread_table.o: kspread.h /opt/kde/idl/controls.h /opt/kde/idl/parts.h
kspread_table.o: kspread_python.h ./Python-1.4/Include/Python.h
kspread_table.o: ./Python-1.4/Include/allobjects.h
kspread_table.o: /usr/local/kde/include/config.h /usr/include/errno.h
kspread_table.o: /usr/include/errnos.h /usr/include/linux/errno.h
kspread_table.o: /usr/include/asm/errno.h ./Python-1.4/Include/myproto.h
kspread_table.o: ./Python-1.4/Include/object.h ./Python-1.4/Include/objimpl.h
kspread_table.o: ./Python-1.4/Include/pydebug.h
kspread_table.o: ./Python-1.4/Include/accessobject.h
kspread_table.o: ./Python-1.4/Include/intobject.h
kspread_table.o: ./Python-1.4/Include/longobject.h
kspread_table.o: ./Python-1.4/Include/floatobject.h
kspread_table.o: ./Python-1.4/Include/complexobject.h
kspread_table.o: ./Python-1.4/Include/rangeobject.h
kspread_table.o: ./Python-1.4/Include/stringobject.h
kspread_table.o: ./Python-1.4/Include/tupleobject.h
kspread_table.o: ./Python-1.4/Include/listobject.h
kspread_table.o: ./Python-1.4/Include/mappingobject.h
kspread_table.o: ./Python-1.4/Include/methodobject.h
kspread_table.o: ./Python-1.4/Include/moduleobject.h
kspread_table.o: ./Python-1.4/Include/funcobject.h
kspread_table.o: ./Python-1.4/Include/classobject.h
kspread_table.o: ./Python-1.4/Include/fileobject.h
kspread_table.o: ./Python-1.4/Include/cobject.h
kspread_table.o: ./Python-1.4/Include/traceback.h
kspread_table.o: ./Python-1.4/Include/sliceobject.h
kspread_table.o: ./Python-1.4/Include/pyerrors.h
kspread_table.o: ./Python-1.4/Include/mymalloc.h
kspread_table.o: ./Python-1.4/Include/modsupport.h
kspread_table.o: /usr/lib/gcc-lib/alpha-redhat-linux/2.7.2.3/include/varargs.h
kspread_table.o: ./Python-1.4/Include/ceval.h
kspread_table.o: ./Python-1.4/Include/pythonrun.h
kspread_table.o: ./Python-1.4/Include/sysmodule.h
kspread_table.o: ./Python-1.4/Include/intrcheck.h
kspread_table.o: ./Python-1.4/Include/import.h
kspread_table.o: ./Python-1.4/Include/bltinmodule.h
kspread_table.o: ./Python-1.4/Include/abstract.h
kspread_table.o: ./Python-1.4/Include/graminit.h kspread_dlg_print.h
kspread_table.o: kspread_undo.h kspread_doc.h kmetaedit.h kspread_map.h
kspread_table.o: /usr/local/kde/include/koIMR.h
kspread_table.o: /usr/local/kde/include/komlWriter.h
