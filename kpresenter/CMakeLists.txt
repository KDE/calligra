kde4_header()

project(kpresenter)
add_subdirectory( autoformEdit )
add_subdirectory( autoforms )
add_subdirectory( pics )
add_subdirectory( templates )
add_subdirectory( dtd )
add_subdirectory( slideshow )
add_subdirectory( toolbar )




########### next target ###############

set(kpresenterprivate_LIB_SRCS 
   KoPointArray.cpp 
   KPrCanvas.cpp 
   KPrPage.cpp 
   KPrFindReplace.cpp 
   KPrBackDia.cpp 
   KPrWebPresentation.cpp 
   KPrView.cpp 
   KPrMSPresentationSetup.cpp 
   KPrBrush.cpp 
   KPrDocument.cpp 
   KPrSoundPlayer.cpp 
   KPrPgConfDia.cpp 
   KPrEffectDia.cpp 
   KPrCommand.cpp 
   KPrUtils.cpp 
   KPrEffectHandler.cpp 
   KPrPageEffects.cpp 
   KPrTransEffectDia.cpp 
   KPrObject.cpp 
   KPrLineObject.cpp 
   KPrRectObject.cpp 
   KPrEllipseObject.cpp 
   KPrAutoformObject.cpp 
   KPrTextObject.cpp 
   KPrPixmapObject.cpp 
   KPrPieObject.cpp 
   KPrPartObject.cpp 
   KPrGroupObject.cpp 
   KPrBackground.cpp 
   KPrGradient.cpp 
   KPrGradientCollection.cpp 
   KPrGotoPage.cpp 
   KPrFactory.cpp 
   KPrDocumentIface.cpp 
   KPrViewIface.cpp 
   KPrObjectIface.cpp 
   KPrPageIface.cpp 
   KPrSideBar.cpp 
   KPrConfig.cpp 
   KPrVariableCollection.cpp 
   KPrTextDocument.cpp 
   KPrPointObject.cpp 
   KPrFreehandObject.cpp 
   KPrPolylineObject.cpp 
   KPrBezierCurveObject.cpp 
   KPrPolygonObject.cpp 
   KPrNoteBar.cpp 
   KPrTextObjectIface.cpp 
   KPrTextViewIface.cpp 
   KPrPresDurationDia.cpp 
   KPrPieObjectIface.cpp 
   KPrPolygonObjectIface.cpp 
   KPrRectObjectIface.cpp 
   KPrPixmapObjectIface.cpp 
   KPrAutoFormObjectIface.cpp 
   KPrObject2DIface.cpp 
   KPrLineObjectIface.cpp 
   KPrQuadricBezierCurveObjectIface.cpp 
   KPrCubicBezierCurveObjectIface.cpp 
   KPrSVGPathParser.cpp 
   KPrShadowDialogImpl.cpp 
   KPrTextPreview.cpp 
   KPrRotationDialogImpl.cpp 
   KPrFreehandObjectIface.cpp 
   KPrStyleManager.cpp 
   KPrBgSpellCheck.cpp 
   KPrMoveHelpLineDia.cpp 
   KPrPolyLineObjectIface.cpp 
   KPrDuplicateObjDia.cpp 
   KPrClosedLineObject.cpp 
   KPrImportStyleDia.cpp 
   KPrImageEffectDia.cpp 
   KPrCustomSlideShowDia.cpp 
   KPrPBPreview.cpp 
   KPrPenStyleWidget.cpp 
   KPrRectPreview.cpp 
   KPrRectProperty.cpp 
   KPrPiePreview.cpp 
   KPrPolygonPreview.cpp 
   KPrPicturePreview.cpp 
   KPrBrushProperty.cpp 
   KPrGeneralProperty.cpp 
   KPrPolygonProperty.cpp 
   KPrPieProperty.cpp 
   KPrPictureProperty.cpp 
   KPrMarginWidget.cpp 
   KPrTextProperty.cpp 
   KPrObjectProperties.cpp 
   KPrPropertyEditor.cpp 
   KPrSlideTransitionDia.cpp 
   KPrPrinterDlg.cpp )

kde4_automoc(${kpresenterprivate_LIB_SRCS})

kde4_add_ui3_files(kpresenterprivate_LIB_SRCS 
   insertpagedia.ui 
   shadowdialog.ui 
   rotationpropertyui.ui 
   imageEffectBase.ui 
   penstyle.ui 
   rectpropertyui.ui 
   brushpropertyui.ui 
   gradientpropertyui.ui 
   polygonpropertyui.ui 
   piepropertyui.ui 
   picturepropertyui.ui 
   marginui.ui 
   slidetransitionwidget.ui )

kde4_add_dcop_skels(kpresenterprivate_LIB_SRCS 
   KPrDocumentIface.h 
   KPrViewIface.h 
   KPrObjectIface.h 
   KPrPageIface.h 
   KPrTextObjectIface.h 
   KPrTextViewIface.h 
   KPrPieObjectIface.h 
   KPrPolygonObjectIface.h 
   KPrRectObjectIface.h 
   KPrPixmapObjectIface.h 
   KPrAutoFormObjectIface.h 
   KPrObject2DIface.h 
   KPrLineObjectIface.h 
   KPrQuadricBezierCurveObjectIface.h 
   KPrCubicBezierCurveObjectIface.h 
   KPrFreehandObjectIface.h 
   KPrPolyLineObjectIface.h )

kde4_add_library(kpresenterprivate SHARED ${kpresenterprivate_LIB_SRCS})

target_link_libraries(kpresenterprivate  ${KDE4_KDECORE_LIBS} af kofficeui kotext kopainter )

set_target_properties(kpresenterprivate PROPERTIES VERSION 4.0.0 SOVERSION 4 )
install_targets(${LIB_INSTALL_DIR} kpresenterprivate )


########### next target ###############

set(kpresenterpart_PART_SRCS KPrFactoryInit.cpp )

kde4_automoc(${kpresenterpart_PART_SRCS})

kde4_add_plugin(kpresenterpart WITH_PREFIX ${kpresenterpart_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kpresenterpart )

target_link_libraries(kpresenterpart  ${KDE4_KDECORE_LIBS} kpresenterprivate )

install_targets(${PLUGIN_INSTALL_DIR} kpresenterpart )


########### next target ###############

set(kpresenter_KDEINIT_SRCS main.cpp )

kde4_automoc(${kpresenter_KDEINIT_SRCS})

kde4_add_kdeinit_executable( kpresenter ${kpresenter_KDEINIT_SRCS})

target_link_libraries(kdeinit_kpresenter  ${KDE4_KDECORE_LIBS} kofficecore )

install_targets(${LIB_INSTALL_DIR} kdeinit_kpresenter )

target_link_libraries( kpresenter kdeinit_kpresenter )
install_targets(/bin kpresenter )

########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kpresenter FILES  kpresenter.rc kpresenter_readonly.rc )
install_files( ${SERVICES_INSTALL_DIR} FILES  kpresenterpart.desktop )
install_files( ${APPLNK_INSTALL_DIR}/Office FILES  kpresenter.desktop )
install_files(  FILES  kprconverter.pl )



kde4_footer()



#original Makefile.am contents follow:

#KDE_CXXFLAGS = $(USE_RTTI)
#INCLUDES = $(KOFFICE_INCLUDES) $(KOTEXT_INCLUDES) $(KOPAINTER_INCLUDES) -I$(kde_includes)/arts $(all_includes)
#
#bin_SCRIPTS = kprconverter.pl
#
### A lib with all the code; can be used by filters
#lib_LTLIBRARIES = libkpresenterprivate.la
#libkpresenterprivate_la_SOURCES = KoPointArray.cpp KPrCanvas.cpp \
#	KPrPage.cpp KPrFindReplace.cpp KPrBackDia.cpp \
#	KPrWebPresentation.cpp KPrView.cpp  KPrMSPresentationSetup.cpp \
#	KPrBrush.cpp \
#	KPrDocument.cpp \
#	KPrSoundPlayer.cpp \
#	KPrPgConfDia.cpp KPrEffectDia.cpp KPrCommand.cpp \
#	KPrUtils.cpp  \
#	KPrEffectHandler.cpp KPrPageEffects.cpp\
#	KPrTransEffectDia.cpp \
#	KPrObject.cpp KPrLineObject.cpp KPrRectObject.cpp KPrEllipseObject.cpp \
#	KPrAutoformObject.cpp KPrTextObject.cpp \
#	KPrPixmapObject.cpp KPrPieObject.cpp KPrPartObject.cpp KPrGroupObject.cpp \
#	KPrBackground.cpp KPrGradient.cpp \
#	KPrGradientCollection.cpp \
#	KPrGotoPage.cpp \
#	KPrFactory.cpp \
#	KPrDocumentIface.skel KPrDocumentIface.cpp \
#	KPrViewIface.skel KPrViewIface.cpp\
#	KPrObjectIface.skel KPrObjectIface.cpp \
#	KPrPageIface.skel KPrPageIface.cpp \
#	KPrSideBar.cpp \
#	insertpagedia.ui \
#	KPrConfig.cpp KPrVariableCollection.cpp \
#	KPrTextDocument.cpp KPrPointObject.cpp KPrFreehandObject.cpp KPrPolylineObject.cpp \
#    KPrBezierCurveObject.cpp \
#	KPrPolygonObject.cpp KPrNoteBar.cpp \
#	KPrTextObjectIface.skel KPrTextObjectIface.cpp \
#	KPrTextViewIface.skel KPrTextViewIface.cpp KPrPresDurationDia.cpp \
#	KPrPieObjectIface.skel KPrPieObjectIface.cpp \
#	KPrPolygonObjectIface.skel KPrPolygonObjectIface.cpp \
#	KPrRectObjectIface.skel KPrRectObjectIface.cpp \
#	KPrPixmapObjectIface.skel KPrPixmapObjectIface.cpp \
#	KPrAutoFormObjectIface.skel KPrAutoFormObjectIface.cpp \
#	KPrObject2DIface.skel KPrObject2DIface.cpp \
#	KPrLineObjectIface.skel KPrLineObjectIface.cpp \
#	KPrQuadricBezierCurveObjectIface.skel KPrQuadricBezierCurveObjectIface.cpp \
#	KPrCubicBezierCurveObjectIface.skel KPrCubicBezierCurveObjectIface.cpp \
#    KPrSVGPathParser.cpp \
#	shadowdialog.ui KPrShadowDialogImpl.cpp KPrTextPreview.cpp \
#	KPrRotationDialogImpl.cpp rotationpropertyui.ui \
#	KPrFreehandObjectIface.skel KPrFreehandObjectIface.cpp KPrStyleManager.cpp \
#	KPrBgSpellCheck.cpp KPrMoveHelpLineDia.cpp \
#	KPrPolyLineObjectIface.skel KPrPolyLineObjectIface.cpp \
#	KPrDuplicateObjDia.cpp KPrClosedLineObject.cpp \
#	KPrImportStyleDia.cpp \
#	KPrImageEffectDia.cpp imageEffectBase.ui KPrCustomSlideShowDia.cpp \
#	KPrPBPreview.cpp penstyle.ui KPrPenStyleWidget.cpp \
#	KPrRectPreview.cpp rectpropertyui.ui KPrRectProperty.cpp \
#	KPrPiePreview.cpp \
#	KPrPolygonPreview.cpp \
#	KPrPicturePreview.cpp \
#	brushpropertyui.ui gradientpropertyui.ui KPrBrushProperty.cpp \
#	KPrGeneralProperty.cpp \
#	polygonpropertyui.ui KPrPolygonProperty.cpp \
#	piepropertyui.ui KPrPieProperty.cpp \
#	picturepropertyui.ui KPrPictureProperty.cpp \
#	marginui.ui KPrMarginWidget.cpp KPrTextProperty.cpp \
#	KPrObjectProperties.cpp \
#	KPrPropertyEditor.cpp \
#	slidetransitionwidget.ui KPrSlideTransitionDia.cpp \
#	KPrPrinterDlg.cpp
#
#libkpresenterprivate_la_LDFLAGS = $(all_libraries) -version-info 4:0 -no-undefined
#if include_ARTS
#ARTSKDE = -lartskde
#endif
#libkpresenterprivate_la_LIBADD = autoformEdit/libaf.la $(LIB_KOFFICEUI) $(LIB_KOTEXT) $(LIB_KOPAINTER) $(ARTSKDE)
#
### The part
#kde_module_LTLIBRARIES = libkpresenterpart.la
#libkpresenterpart_la_SOURCES = KPrFactoryInit.cpp
#libkpresenterpart_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#libkpresenterpart_la_LIBADD = libkpresenterprivate.la
#
### The kdeinit loadable module and the executable
#kdeinit_LTLIBRARIES = kpresenter.la
#bin_PROGRAMS =
#kpresenter_la_SOURCES = main.cpp
#kpresenter_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#kpresenter_la_LIBADD = $(LIB_KOFFICECORE)
#
#METASOURCES = AUTO
#
#kdelnk_DATA = kpresenter.desktop
#kdelnkdir = $(kde_appsdir)/Office
#
#kde_services_DATA = kpresenterpart.desktop
#
#rcdir = $(kde_datadir)/kpresenter
#rc_DATA = kpresenter.rc kpresenter_readonly.rc
#
#SUBDIRS = autoformEdit autoforms . pics templates dtd slideshow toolbar
#
#messages: rc.cpp
#	$(XGETTEXT) rc.cpp */*.cpp *.cpp *.h KPrAboutData.h -o $(podir)/kpresenter.pot
#
#include $(top_srcdir)/admin/Doxyfile.am
