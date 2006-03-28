kde4_header()

project(kword)

add_subdirectory( mailmerge )
add_subdirectory( templates )
add_subdirectory( dtd )
add_subdirectory( pics )
add_subdirectory( toolbar )
add_subdirectory( expression )
add_subdirectory( data )
add_subdirectory( tests )

include_directories( ${CMAKE_SOURCE_DIR}/lib/kformula ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kwordpart_PART_SRCS KWFactoryInit.cpp )

kde4_automoc(${kwordpart_PART_SRCS})

kde4_add_plugin(kwordpart WITH_PREFIX ${kwordpart_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kwordpart )

target_link_libraries(kwordpart  ${KDE4_KDECORE_LIBS} kwordprivate )

install_targets(${LIB_INSTALL_DIR}/kde4 kwordpart )


########### next target ###############

set(kwmailmerge_interface_LIB_SRCS KWMailMergeDataSource.cpp )

kde4_automoc(${kwmailmerge_interface_LIB_SRCS})

kde4_add_dcop_skels(kwmailmerge_interface_LIB_SRCS KWMailMergeDataSource.h )

kde4_add_library(kwmailmerge_interface SHARED ${kwmailmerge_interface_LIB_SRCS})

target_link_libraries(kwmailmerge_interface  ${KDE4_KDECORE_LIBS} DCOP )

set_target_properties(kwmailmerge_interface PROPERTIES VERSION 4.0.0 SOVERSION 4 )
install_targets(${LIB_INSTALL_DIR} kwmailmerge_interface )


########### next target ###############

set(kwordprivate_LIB_SRCS 
   KWTableFrameSet.cpp 
   KWCanvas.cpp 
   KWFrame.cpp 
   KWFormulaFrameSet.cpp 
   KWFactory.cpp 
   KWDocument.cpp 
   KWView.cpp 
   KWViewMode.cpp 
   KWTextParag.cpp 
   KWTextDocument.cpp 
   KWTextFrameSet.cpp 
   KWPartFrameSet.cpp 
   KWTextImage.cpp 
   KWAnchor.cpp 
   KWConfig.cpp 
   KWVariable.cpp 
   KWFrameDia.cpp 
   KWFootNoteDia.cpp 
   KWFrameStyle.cpp 
   KWFrameStyleManager.cpp 
   KWTableStyle.cpp 
   KWTableStyleManager.cpp 
   KWTableTemplate.cpp 
   KWTableTemplateSelector.cpp 
   KWTableDia.cpp 
   KWInsertDia.cpp 
   KWDeleteDia.cpp 
   KWDocStruct.cpp 
   KWInsertTOCCommand.cpp 
   KWStyleManager.cpp 
   KWFindReplace.cpp 
   KWCommand.cpp 
   KWStatisticsDialog.cpp 
   kwordmailmergedatabaseiface.cpp 
   KWMailMergeDataBase.cpp 
   KWMailMergeLabelAction.cpp 
   KWEditPersonnalExpression.cpp 
   KWSplitCellDia.cpp 
   KWInsertPicDia.cpp 
   KWordDocIface.cpp 
   KWordViewIface.cpp 
   KWordTextFrameSetIface.cpp 
   KWordTextFrameSetEditIface.cpp 
   KWordFrameSetIface.cpp 
   KWordFormulaFrameSetIface.cpp 
   KWordFormulaFrameSetEditIface.cpp 
   KWordPictureFrameSetIface.cpp 
   KWordTableFrameSetIface.cpp 
   KWordPartFrameSetIface.cpp 
   KWBgSpellCheck.cpp 
   KWResizeTableDia.cpp 
   KWFrameLayout.cpp 
   KWConfigFootNoteDia.cpp 
   KWCreateBookmarkDia.cpp 
   KWImportStyleDia.cpp 
   KWordFootNoteFrameSetIface.cpp 
   KWSortDia.cpp 
   KWInsertPageDia.cpp 
   KWCollectFramesetsVisitor.cpp 
   KWOasisLoader.cpp 
   KWOasisSaver.cpp 
   KWFrameList.cpp 
   KWPage.cpp 
   KWPageManager.cpp 
   KWFrameSet.cpp 
   KWFrameSetEdit.cpp 
   KWPictureFrameSet.cpp 
   KWFrameViewManager.cpp 
   KWFrameView.cpp 
   KWStartupWidget.cpp 
   KWLoadingInfo.cpp )

kde4_automoc(${kwordprivate_LIB_SRCS})

kde4_add_ui3_files(kwordprivate_LIB_SRCS KWCreateBookmarkDiaBase.ui KWSelectBookmarkDiaBase.ui KWStartupWidgetBase.ui )

kde4_add_dcop_skels(kwordprivate_LIB_SRCS 
   KWordMailMergeDatabaseIface.h 
   KWordDocIface.h 
   KWordViewIface.h 
   KWordTextFrameSetIface.h 
   KWordTextFrameSetEditIface.h 
   KWordFrameSetIface.h 
   KWordFormulaFrameSetIface.h 
   KWordFormulaFrameSetEditIface.h 
   KWordPictureFrameSetIface.h 
   KWordTableFrameSetIface.h 
   KWordPartFrameSetIface.h 
   KWordFootNoteFrameSetIface.h )

kde4_add_library(kwordprivate SHARED ${kwordprivate_LIB_SRCS})

target_link_libraries(kwordprivate  ${KDE4_KDECORE_LIBS} kofficeui kotext kformulalib kwmailmerge_interface )

set_target_properties(kwordprivate PROPERTIES VERSION 4.0.0 SOVERSION 4 )
install_targets(${LIB_INSTALL_DIR} kwordprivate )


########### next target ###############

set(kword_KDEINIT_SRCS main.cpp )

kde4_automoc(${kword_KDEINIT_SRCS})

kde4_add_kdeinit_executable( kword ${kword_KDEINIT_SRCS})

target_link_libraries(kdeinit_kword  ${KDE4_KDECORE_LIBS} kofficecore )

install_targets(${LIB_INSTALL_DIR} kdeinit_kword )

target_link_libraries( kword kdeinit_kword )
install_targets(/bin kword )

########### install files ###############

install_files( ${SERVICETYPES_INSTALL_DIR} FILES  kwmailmerge.desktop )
install_files( ${DATA_INSTALL_DIR}/kword FILES  kword.rc kword_readonly.rc )
install_files( ${SERVICES_INSTALL_DIR} FILES  kwordpart.desktop )
install_files( ${APPLNK_INSTALL_DIR}/Office FILES  kword.desktop )
install_files( /include/kword FILES  KWMailMergeDataSource.h )

kde4_create_doxygen_docs(  )

kde4_footer()



#original Makefile.am contents follow:

## We use STL sorting so we need some typedefs
#KDE_CXXFLAGS = -UQT_NO_STL
#
#INCLUDES = $(KOFFICE_INCLUDES) -I$(top_srcdir)/lib/kformula \
#	$(KOTEXT_INCLUDES) $(all_includes)
#
#kde_module_LTLIBRARIES = libkwordpart.la
#lib_LTLIBRARIES = libkwmailmerge_interface.la libkwordprivate.la
#
### The part
#libkwordpart_la_SOURCES = KWFactoryInit.cpp
#libkwordpart_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#libkwordpart_la_LIBADD = libkwordprivate.la
#
### A lib with all the code
#libkwordprivate_la_SOURCES = KWTableFrameSet.cpp KWCanvas.cpp KWFrame.cpp KWFormulaFrameSet.cpp \
#		KWFactory.cpp KWDocument.cpp \
#		KWView.cpp  \
#		KWViewMode.cpp \
#		KWTextParag.cpp KWTextDocument.cpp \
#		KWTextFrameSet.cpp KWPartFrameSet.cpp \
#		KWTextImage.cpp KWAnchor.cpp \
#		KWConfig.cpp \
#		KWVariable.cpp \
#		KWFrameDia.cpp KWFootNoteDia.cpp \
#		KWFrameStyle.cpp KWFrameStyleManager.cpp \
#		KWTableStyle.cpp KWTableStyleManager.cpp \
#		KWTableTemplate.cpp KWTableTemplateSelector.cpp KWTableDia.cpp \
#		KWInsertDia.cpp KWDeleteDia.cpp KWDocStruct.cpp \
#		KWInsertTOCCommand.cpp KWStyleManager.cpp KWFindReplace.cpp \
#		KWCommand.cpp KWStatisticsDialog.cpp \
#		KWordMailMergeDatabaseIface.skel kwordmailmergedatabaseiface.cpp KWMailMergeDataBase.cpp \
#		KWMailMergeLabelAction.cpp \
#		KWEditPersonnalExpression.cpp \
#		KWSplitCellDia.cpp KWInsertPicDia.cpp \
#		KWordDocIface.skel KWordDocIface.cpp \
#		KWordViewIface.skel KWordViewIface.cpp \
#		KWordTextFrameSetIface.skel KWordTextFrameSetIface.cpp \
#		KWordTextFrameSetEditIface.skel KWordTextFrameSetEditIface.cpp \
#		KWordFrameSetIface.skel KWordFrameSetIface.cpp \
#		KWordFormulaFrameSetIface.skel KWordFormulaFrameSetIface.cpp \
#		KWordFormulaFrameSetEditIface.skel KWordFormulaFrameSetEditIface.cpp \
#		KWordPictureFrameSetIface.skel KWordPictureFrameSetIface.cpp \
#		KWordTableFrameSetIface.skel KWordTableFrameSetIface.cpp \
#		KWordPartFrameSetIface.skel KWordPartFrameSetIface.cpp \
#		KWBgSpellCheck.cpp KWResizeTableDia.cpp KWFrameLayout.cpp \
#		KWConfigFootNoteDia.cpp KWCreateBookmarkDia.cpp KWImportStyleDia.cpp \
#		KWordFootNoteFrameSetIface.skel KWordFootNoteFrameSetIface.cpp \
#		KWCreateBookmarkDiaBase.ui  KWSelectBookmarkDiaBase.ui \
#		KWSortDia.cpp KWInsertPageDia.cpp KWCollectFramesetsVisitor.cpp \
#		KWOasisLoader.cpp KWOasisSaver.cpp KWFrameList.cpp KWPage.cpp \
#		KWPageManager.cpp KWFrameSet.cpp KWFrameSetEdit.cpp KWPictureFrameSet.cpp \
#		KWFrameViewManager.cpp KWFrameView.cpp KWStartupWidgetBase.ui KWStartupWidget.cpp \
#		KWLoadingInfo.cpp
#
#
#libkwordprivate_la_LDFLAGS = $(all_libraries) -version-info 4:0 -no-undefined
#libkwordprivate_la_LIBADD = $(LIB_KOFFICEUI) $(LIB_KOTEXT) ../lib/kformula/libkformulalib.la ./libkwmailmerge_interface.la 
#
#METASOURCES = AUTO
#
### The mailmerge interface library
#libkwmailmerge_interface_la_SOURCES = KWMailMergeDataSource.cpp KWMailMergeDataSource.skel
#libkwmailmerge_interface_la_LDFLAGS = $(all_libraries) -version-info 4:0 -no-undefined
#libkwmailmerge_interface_la_LIBADD = $(LIB_QT) -lDCOP
#kwincludedir = $(includedir)/kword
#kwinclude_HEADERS = KWMailMergeDataSource.h
#
#kwmailmerge_servicetype_DATA=kwmailmerge.desktop
#kwmailmerge_servicetypedir=$(kde_servicetypesdir)
#
### The kdeinit loadable module and the kword binary
#kdeinit_LTLIBRARIES = kword.la
#bin_PROGRAMS =
#kword_la_SOURCES = main.cpp
#kword_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#kword_la_LIBADD = $(LIB_KOFFICECORE)
#
#kdelnk_DATA = kword.desktop
#kdelnkdir = $(kde_appsdir)/Office
#
#kde_services_DATA = kwordpart.desktop
#
#rcdir = $(kde_datadir)/kword
#rc_DATA = kword.rc kword_readonly.rc
#
#SUBDIRS = . mailmerge templates dtd pics toolbar expression data tests
#
#messages: rc.cpp
#	$(EXTRACTRC) --tag-group=koffice expression/*.xml > xml_doc.cpp
#	$(EXTRACTRC) `find mailmerge -name \*.ui` >> rc.cpp
#	$(XGETTEXT) rc.cpp *.cpp `find mailmerge -name \*.cpp -o -name \*.cpp` KWAboutData.h -o $(podir)/kword.pot
#	-rm xml_doc.cpp
#
#include $(top_srcdir)/admin/Doxyfile.am
