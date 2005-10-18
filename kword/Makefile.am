
KDE_CXXFLAGS = $(USE_RTTI)
INCLUDES = $(KOFFICE_INCLUDES) -I$(top_srcdir)/lib/kformula \
	$(KOTEXT_INCLUDES) $(all_includes)

kde_module_LTLIBRARIES = libkwordpart.la

## The part
libkwordpart_la_SOURCES = KWTableFrameSet.cpp KWCanvas.cpp KWFrame.cpp KWFormulaFrameSet.cpp \
		KWFactory.cpp KWDocument.cpp \
		KWView.cpp  \
                KWViewMode.cpp \
		KWTextParag.cpp KWTextDocument.cpp \
		KWTextFrameSet.cpp KWPartFrameSet.cpp \
                KWTextImage.cpp KWAnchor.cpp \
                KWConfig.cpp \
		KWVariable.cpp \
		KWFrameDia.cpp KWFootNoteDia.cpp \
                KWFrameStyle.cpp KWFrameStyleManager.cpp \
                KWTableStyle.cpp KWTableStyleManager.cpp \
                KWTableTemplate.cpp KWTableTemplateSelector.cpp KWTableDia.cpp \
                KWInsertDia.cpp KWDeleteDia.cpp KWDocStruct.cpp \
                KWInsertTOCCommand.cpp KWStyleManager.cpp KWFindReplace.cpp \
                KWResizeHandle.cpp KWCommand.cpp \
		KWordMailMergeDatabaseIface.skel kwordmailmergedatabaseiface.cc KWMailMergeDataBase.cpp \
		KWMailMergeLabelAction.cpp \
		KWEditPersonnalExpression.cpp \
		KWSplitCellDia.cpp KWInsertPicDia.cpp \
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
		KWBgSpellCheck.cpp KWResizeTableDia.cpp KWFrameLayout.cpp \
		KWConfigFootNoteDia.cpp KWCreateBookmarkDia.cpp KWImportStyleDia.cpp \
		KWordFootNoteFrameSetIface.skel KWordFootNoteFrameSetIface.cc \
	        kwcreatebookmarkdiabase.ui  kwselectbookmarkdiabase.ui \
		KWordHorizontalLineFrameSetIface.cc KWInsertHorizontalLineDia.cpp \
		KWSortDia.cpp KWInsertPageDia.cpp KWBookMark.cpp KWCollectFramesetsVisitor.cpp \
		KWOasisLoader.cpp KWOasisSaver.cpp KWFrameList.cpp KWPage.cpp \
		KWPageManager.cpp
# Remove files out of this list: KWordHorizontalLineFrameSetIface.skel


libkwordpart_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN)
libkwordpart_la_LIBADD = $(LIB_KOFFICEUI) $(LIB_KOTEXT) ../lib/kformula/libkformulalib.la ./libkwmailmerge_interface.la 
METASOURCES = AUTO

lib_LTLIBRARIES = libkwmailmerge_interface.la

## The mailmerge interface library
libkwmailmerge_interface_la_SOURCES = KWMailMergeDataSource.cpp KWMailMergeDataSource.skel
libkwmailmerge_interface_la_LDFLAGS = $(all_libraries) -version-info 4:0 -no-undefined
libkwmailmerge_interface_la_LIBADD = $(LIB_QT) -lDCOP
kwincludedir = $(includedir)/kword
kwinclude_HEADERS = KWMailMergeDataSource.h

kwmailmerge_servicetype_DATA=kwmailmerge.desktop
kwmailmerge_servicetypedir=$(kde_servicetypesdir)

## The kdeinit loadable module and the kword binary
kdeinit_LTLIBRARIES = kword.la
bin_PROGRAMS =
kword_la_SOURCES = main.cpp
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
	$(XGETTEXT) rc.cpp *.cc `find mailmerge -name \*.cc` KWAboutData.h -o $(podir)/kword.pot
	-rm xml_doc.cc
