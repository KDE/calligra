
KDE_CXXFLAGS = $(USE_RTTI)
INCLUDES = $(KOFFICE_INCLUDES) -I$(top_srcdir)/lib/kformula \
	$(KOTEXT_INCLUDES) $(all_includes)

kde_module_LTLIBRARIES = libkwordpart.la

## The part
libkwordpart_la_SOURCES = kwtableframeset.cc kwcanvas.cc kwframe.cc kwformulaframe.cc \
		kwfactory.cc kwdoc.cc \
		kwview.cc  \
                kwviewmode.cc \
		kwtextparag.cc kwtextdocument.cc \
		kwtextframeset.cc kwpartframeset.cc \
                kwtextimage.cc kwanchor.cc \
                kwconfig.cc \
		kwvariable.cc \
		framedia.cc footnotedia.cc \
                kwframestyle.cc kwframestylemanager.cc \
                kwtablestyle.cc kwtablestylemanager.cc \
                kwtabletemplate.cc kwtabletemplateselector.cc tabledia.cc \
                insdia.cc deldia.cc docstruct.cc \
                contents.cc kwstylemanager.cc searchdia.cc \
                resizehandles.cc kwcommand.cc \
		KWordMailMergeDatabaseIface.skel kwordmailmergedatabaseiface.cc mailmerge.cc \
		mailmerge_actions.cc \
		kweditpersonnalexpressiondia.cc \
		splitcellsdia.cc kwinsertpicdia.cc \
		KWordDocIface.skel KWordDocIface.cc \
		KWordViewIface.skel KWordViewIface.cc \
		KWordTextFrameSetIface.skel KWordTextFrameSetIface.cc \
		KWordTextFrameSetEditIface.skel KWordTextFrameSetEditIface.cc \
		KWordFrameSetIface.skel KWordFrameSetIface.cc \
		KWordFormulaFrameSetIface.skel KWordFormulaFrameSetIface.cc \
		KWordFormulaFrameSetEditIface.skel KWordFormulaFrameSetEditIface.cc \
		KWordPictureFrameSetIface.skel KWordPictureFrameSetIface.cc \
		KWordTableFrameSetIface.skel KWordTableFrameSetIface.cc \
		KWordPartFrameSetIface.skel KWordPartFrameSetIface.cc \
		kwbgspellcheck.cc resizetabledia.cc kwframelayout.cc \
		configfootnotedia.cc kwcreatebookmarkdia.cc kwimportstyledia.cc \
		KWordFootNoteFrameSetIface.skel KWordFootNoteFrameSetIface.cc \
	        kwcreatebookmarkdiabase.ui  kwselectbookmarkdiabase.ui \
		KWordHorizontalLineFrameSetIface.cc kwinserthorizontallinedia.cc \
		sortdia.cc kwinsertpagedia.cc kwbookmark.cc paragvisitors.cc \
		kwoasisloader.cc kwoasissaver.cc KWFrameList.cpp KWPage.cpp \
		KWPageManager.cpp
# Remove files out of this list: KWordHorizontalLineFrameSetIface.skel


libkwordpart_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN)
libkwordpart_la_LIBADD = $(LIB_KOFFICEUI) $(LIB_KOTEXT) ../lib/kformula/libkformulalib.la ./libkwmailmerge_interface.la 
METASOURCES = AUTO

lib_LTLIBRARIES = libkwmailmerge_interface.la

## The mailmerge interface library
libkwmailmerge_interface_la_SOURCES = mailmerge_interface.cc mailmerge_interface.skel
libkwmailmerge_interface_la_LDFLAGS = $(all_libraries) -version-info 4:0 -no-undefined
libkwmailmerge_interface_la_LIBADD = $(LIB_QT) -lDCOP
kwincludedir = $(includedir)/kword
kwinclude_HEADERS = mailmerge_interface.h

kwmailmerge_servicetype_DATA=kwmailmerge.desktop
kwmailmerge_servicetypedir=$(kde_servicetypesdir)

## The kdeinit loadable module and the kword binary
kdeinit_LTLIBRARIES = kword.la
bin_PROGRAMS =
kword_la_SOURCES = main.cc
kword_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kword_la_LIBADD = $(LIB_KOFFICECORE)

kdelnk_DATA = kword.desktop
kdelnkdir = $(kde_appsdir)/Office

kde_services_DATA = kwordpart.desktop

rcdir = $(kde_datadir)/kword
rc_DATA = kword.rc kword_readonly.rc

SUBDIRS = . mailmerge templates dtd pics toolbar expression data tests

messages: rc.cpp
	$(EXTRACTRC) --tag-group=koffice expression/*.xml > xml_doc.cc
	$(EXTRACTRC) `find mailmerge -name \*.ui` >> rc.cpp
	$(XGETTEXT) rc.cpp *.cc `find mailmerge -name \*.cc` kwaboutdata.h -o $(podir)/kword.pot
	-rm xml_doc.cc
