#make it a project so cmake spits out a codeblocks project
#project(reports)

include_directories( ${KOMAIN_INCLUDES} 
 
${KOODF_INCLUDES} 
${CMAKE_SOURCE_DIR}/kexi/kexidb/
${CMAKE_SOURCE_DIR}/kexi/core/
${CMAKE_SOURCE_DIR}/kexi/koproperty2/
${CMAKE_SOURCE_DIR}/kexi/plugins/reportspgz/koreport/
${CMAKE_SOURCE_DIR}/kexi/plugins/reportspgz/koreport/common
${CMAKE_SOURCE_DIR}/kexi/plugins/reportspgz/koreport/renderer
${CMAKE_SOURCE_DIR}/kchart/kdchart/include
${KDE4_INCLUDES} )

#build a shared library
set(koreport_LIB_SRCS
	common/krpos.cpp
	common/krsize.cpp
	common/krobjectdata.cpp
	common/krchartdata.cpp
	common/krlabeldata.cpp
	common/krfielddata.cpp
	common/krtextdata.cpp
	common/krbarcodedata.cpp
	common/krimagedata.cpp
	common/krlinedata.cpp
	common/krshapedata.cpp
	common/krcheckdata.cpp
	common/krsectiondata.cpp
	common/labelsizeinfo.cpp
	common/reportpageoptions.cpp
	common/krreportdata.cpp
	common/krdetailsectiondata.cpp
	common/parsexmlutils.cpp
	
	renderer/3of9.cpp
	renderer/code128.cpp
	renderer/codeean.cpp
	renderer/ext3of9.cpp
	renderer/renderobjects.cpp
	renderer/orprintrender.cpp
	renderer/orprerender.cpp
	renderer/krscreenrender.cpp
	renderer/krhtmlrender.cpp

	renderer/scripting/krscripthandler.cpp
	renderer/scripting/krscriptfunctions.cpp
	renderer/scripting/krscriptconstants.cpp
	renderer/scripting/krscriptdebug.cpp
	renderer/scripting/krscriptdraw.cpp
	renderer/scripting/krscriptreport.cpp
	renderer/scripting/krscriptsection.cpp
	renderer/scripting/krscriptlabel.cpp
	renderer/scripting/krscriptfield.cpp
	renderer/scripting/krscripttext.cpp
	renderer/scripting/krscriptbarcode.cpp
	renderer/scripting/krscriptline.cpp
	renderer/scripting/krscriptimage.cpp
	renderer/scripting/krscriptchart.cpp
	
	wrtembed/detailgroupsectiondialog.cpp
	wrtembed/reportentities.cpp
	wrtembed/reportrectentity.cpp
	wrtembed/reportentitylabel.cpp
	wrtembed/reportentityfield.cpp
	wrtembed/reportentitytext.cpp
	wrtembed/reportentityline.cpp
	wrtembed/reportentitybarcode.cpp
	wrtembed/reportentityimage.cpp
	wrtembed/reportentitychart.cpp
	wrtembed/reportentityshape.cpp
	wrtembed/reportentitycheck.cpp
	wrtembed/reportsection.cpp
	wrtembed/reportsectiondetailgroup.cpp
	wrtembed/reportsectiondetail.cpp
	wrtembed/reportscene.cpp
	wrtembed/reportsceneview.cpp
	wrtembed/reportdesigner.cpp
	wrtembed/sectioneditor.cpp
	wrtembed/3of9paint.cpp
	wrtembed/ext3of9paint.cpp
	wrtembed/codeeanpaint.cpp
	wrtembed/code128paint.cpp
	)
qt4_wrap_cpp(koreport_LIB_SRCS ../../scripting/kexiscripting/kexiscriptadaptor.h)

if (SHOULD_BUILD_KSPREAD)
  add_definitions(-DHAVE_KSPREAD)
  set(koreport_LIB_SRCS ${koreport_LIB_SRCS} renderer/krkspreadrender.cpp)
endif (SHOULD_BUILD_KSPREAD)

kde4_add_ui_files(koreport_LIB_SRCS wrtembed/detailgroupsectiondialog.ui wrtembed/sectioneditor.ui )

kde4_add_library(koreport SHARED ${koreport_LIB_SRCS})

target_link_libraries(koreport ${KDE4_KDEUI_LIBS} ${QT_QTGUI_LIBRARY}
  kexidb kexicore ${KOPROPERTY_LIBS} kdchart ${KDE4_KROSSCORE_LIBS})
if (SHOULD_BUILD_KSPREAD)
    target_link_libraries(koreport kspreadcommon)
endif (SHOULD_BUILD_KSPREAD)

set_target_properties(koreport PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )
install(TARGETS koreport  ${INSTALL_TARGETS_DEFAULT_ARGS})
