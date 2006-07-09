KDE_CXXFLAGS = $(USE_RTTI)
INCLUDES = $(KOFFICE_INCLUDES) $(KOTEXT_INCLUDES) $(KOPAINTER_INCLUDES) -I$(kde_includes)/arts $(all_includes)

bin_SCRIPTS = kprconverter.pl

## A lib with all the code; can be used by filters
lib_LTLIBRARIES = libkpresenterprivate.la
libkpresenterprivate_la_SOURCES = KoPointArray.cpp KPrCanvas.cpp \
	KPrPage.cpp KPrFindReplace.cpp KPrBackDia.cpp \
	KPrWebPresentation.cpp KPrView.cpp  KPrMSPresentationSetup.cpp \
	KPrBrush.cpp \
	KPrDocument.cpp \
	KPrSoundPlayer.cpp \
	KPrPgConfDia.cpp KPrEffectDia.cpp KPrCommand.cpp \
	KPrUtils.cpp  \
	KPrEffectHandler.cpp KPrPageEffects.cpp\
	KPrTransEffectDia.cpp \
	KPrObject.cpp KPrLineObject.cpp KPrRectObject.cpp KPrEllipseObject.cpp \
	KPrAutoformObject.cpp KPrTextObject.cpp \
	KPrPixmapObject.cpp KPrPieObject.cpp KPrPartObject.cpp KPrGroupObject.cpp \
	KPrBackground.cpp KPrGradient.cpp \
	KPrGradientCollection.cpp \
	KPrGotoPage.cpp \
	KPrFactory.cpp \
	KPrDocumentIface.skel KPrDocumentIface.cpp \
	KPrViewIface.skel KPrViewIface.cpp\
	KPrObjectIface.skel KPrObjectIface.cpp \
	KPrPageIface.skel KPrPageIface.cpp \
	KPrSideBar.cpp \
	insertpagedia.ui \
	KPrConfig.cpp KPrVariableCollection.cpp \
	KPrTextDocument.cpp KPrPointObject.cpp KPrFreehandObject.cpp KPrPolylineObject.cpp \
    KPrBezierCurveObject.cpp \
	KPrPolygonObject.cpp KPrNoteBar.cpp \
	KPrTextObjectIface.skel KPrTextObjectIface.cpp \
	KPrTextViewIface.skel KPrTextViewIface.cpp KPrPresDurationDia.cpp \
	KPrPieObjectIface.skel KPrPieObjectIface.cpp \
	KPrPolygonObjectIface.skel KPrPolygonObjectIface.cpp \
	KPrRectObjectIface.skel KPrRectObjectIface.cpp \
	KPrPixmapObjectIface.skel KPrPixmapObjectIface.cpp \
	KPrAutoFormObjectIface.skel KPrAutoFormObjectIface.cpp \
	KPrObject2DIface.skel KPrObject2DIface.cpp \
	KPrLineObjectIface.skel KPrLineObjectIface.cpp \
	KPrQuadricBezierCurveObjectIface.skel KPrQuadricBezierCurveObjectIface.cpp \
	KPrCubicBezierCurveObjectIface.skel KPrCubicBezierCurveObjectIface.cpp \
    KPrSVGPathParser.cpp \
	shadowdialog.ui KPrShadowDialogImpl.cpp KPrTextPreview.cpp \
	KPrRotationDialogImpl.cpp rotationpropertyui.ui \
	KPrFreehandObjectIface.skel KPrFreehandObjectIface.cpp KPrStyleManager.cpp \
	KPrBgSpellCheck.cpp KPrMoveHelpLineDia.cpp \
	KPrPolyLineObjectIface.skel KPrPolyLineObjectIface.cpp \
	KPrDuplicateObjDia.cpp KPrClosedLineObject.cpp \
	KPrImportStyleDia.cpp \
	KPrImageEffectDia.cpp imageEffectBase.ui KPrCustomSlideShowDia.cpp \
	KPrPBPreview.cpp penstyle.ui KPrPenStyleWidget.cpp \
	KPrRectPreview.cpp rectpropertyui.ui KPrRectProperty.cpp \
	KPrPiePreview.cpp \
	KPrPolygonPreview.cpp \
	KPrPicturePreview.cpp \
	brushpropertyui.ui gradientpropertyui.ui KPrBrushProperty.cpp \
	KPrGeneralProperty.cpp \
	polygonpropertyui.ui KPrPolygonProperty.cpp \
	piepropertyui.ui KPrPieProperty.cpp \
	picturepropertyui.ui KPrPictureProperty.cpp \
	marginui.ui KPrMarginWidget.cpp KPrTextProperty.cpp \
	KPrObjectProperties.cpp \
	KPrPropertyEditor.cpp \
	slidetransitionwidget.ui KPrSlideTransitionDia.cpp \
	KPrPrinterDlg.cpp

libkpresenterprivate_la_LDFLAGS = $(all_libraries) -version-info 4:0 -no-undefined
if include_ARTS
ARTSKDE = -lartskde
endif
libkpresenterprivate_la_LIBADD = autoformEdit/libaf.la $(LIB_KOFFICEUI) $(LIB_KOTEXT) $(LIB_KOPAINTER) $(ARTSKDE)

## The part
kde_module_LTLIBRARIES = libkpresenterpart.la
libkpresenterpart_la_SOURCES = KPrFactoryInit.cpp
libkpresenterpart_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
libkpresenterpart_la_LIBADD = libkpresenterprivate.la

## The kdeinit loadable module and the executable
kdeinit_LTLIBRARIES = kpresenter.la
bin_PROGRAMS =
kpresenter_la_SOURCES = main.cpp
kpresenter_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kpresenter_la_LIBADD = $(LIB_KOFFICECORE)

METASOURCES = AUTO

kdelnk_DATA = kpresenter.desktop
kdelnkdir = $(kde_appsdir)/Office

kde_services_DATA = kpresenterpart.desktop

rcdir = $(kde_datadir)/kpresenter
rc_DATA = kpresenter.rc kpresenter_readonly.rc

SUBDIRS = autoformEdit autoforms . pics templates dtd slideshow toolbar

messages: rc.cpp
	$(XGETTEXT) rc.cpp */*.cpp *.cpp *.h KPrAboutData.h -o $(podir)/kpresenter.pot

include $(top_srcdir)/admin/Doxyfile.am
