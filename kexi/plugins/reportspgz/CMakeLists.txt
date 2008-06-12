include_directories( ${KOMAIN_INCLUDES} 
${KOGUIUTILS_INCLUDES} 
${KOODF_INCLUDES} 
${CMAKE_SOURCE_DIR}/kexi
${CMAKE_SOURCE_DIR}/kexi/core
${CMAKE_SOURCE_DIR}/kexi/widget
${CMAKE_SOURCE_DIR}/kexi/plugins/reportspgz/backend/common 
${CMAKE_SOURCE_DIR}/kexi/plugins/reportspgz/backend/renderer
${CMAKE_SOURCE_DIR}/kchart/kdchart/include
${KDE4_INCLUDES} )

#build a shared library
set(pgzkexireportpart2_LIB_SRCS 
	kexireports.cpp
	kexireportpart.cpp
	kexireportview.cpp
	kexireportpage.cpp
	kexireportdesignview.cpp
	reportentityselector.cpp
	
	backend/common/krpos.cpp
	backend/common/krsize.cpp
	backend/common/krobjectdata.cpp
	backend/common/krchartdata.cpp
	backend/common/krlabeldata.cpp
	backend/common/krfielddata.cpp
	backend/common/krtextdata.cpp
	backend/common/krbarcodedata.cpp
	backend/common/krimagedata.cpp
	backend/common/krlinedata.cpp
	backend/common/krshapedata.cpp
	backend/common/krsectiondata.cpp
	backend/common/labelsizeinfo.cpp
	backend/common/reportpageoptions.cpp
	backend/common/krreportdata.cpp
	backend/common/krdetailsectiondata.cpp
	backend/common/parsexmlutils.cpp
	
	backend/renderer/3of9.cpp
	backend/renderer/code128.cpp
	backend/renderer/codeean.cpp
	backend/renderer/ext3of9.cpp
	backend/renderer/orutils.cpp
	backend/renderer/renderobjects.cpp
	backend/renderer/orprintrender.cpp
	backend/renderer/orprerender.cpp
	backend/renderer/krscreenrender.cpp
	backend/renderer/krhtmlrender.cpp

	backend/renderer/scripting/krscripthandler.cpp
	backend/renderer/scripting/krscriptfunctions.cpp
	backend/renderer/scripting/krscriptconstants.cpp
	backend/renderer/scripting/krscriptdebug.cpp
	backend/renderer/scripting/krscriptdraw.cpp
	backend/renderer/scripting/krscriptreport.cpp
	backend/renderer/scripting/krscriptsection.cpp
	backend/renderer/scripting/krscriptlabel.cpp
	backend/renderer/scripting/krscriptfield.cpp
	backend/renderer/scripting/krscripttext.cpp
	backend/renderer/scripting/krscriptbarcode.cpp
	backend/renderer/scripting/krscriptline.cpp
	backend/renderer/scripting/krscriptimage.cpp
	backend/renderer/scripting/krscriptchart.cpp
	
	backend/wrtembed/detailgroupsectiondialog.cpp
	backend/wrtembed/reportentities.cpp
	backend/wrtembed/reportrectentity.cpp
	backend/wrtembed/reportentitylabel.cpp
	backend/wrtembed/reportentityfield.cpp
	backend/wrtembed/reportentitytext.cpp
	backend/wrtembed/reportentityline.cpp
	backend/wrtembed/reportentitybarcode.cpp
	backend/wrtembed/reportentityimage.cpp
	backend/wrtembed/reportentitychart.cpp
	backend/wrtembed/reportentityshape.cpp
	backend/wrtembed/reportsection.cpp
	backend/wrtembed/reportsectiondetailgroup.cpp
	backend/wrtembed/reportsectiondetail.cpp
	backend/wrtembed/reportscene.cpp
	backend/wrtembed/reportsceneview.cpp
	backend/wrtembed/reportdesigner.cpp
	backend/wrtembed/sectioneditor.cpp
	backend/wrtembed/3of9paint.cpp
	backend/wrtembed/ext3of9paint.cpp
	backend/wrtembed/codeeanpaint.cpp
	backend/wrtembed/code128paint.cpp
	)
if (SHOULD_BUILD_KSPREAD)
  add_definitions(-DHAVE_KSPREAD)
  set(pgzkexireportpart2_LIB_SRCS
	backend/renderer/krkspreadrender.cpp
)
endif (SHOULD_BUILD_KSPREAD)

kde4_add_ui_files(pgzkexireportpart2_LIB_SRCS backend/wrtembed/detailgroupsectiondialog.ui backend/wrtembed/sectioneditor.ui )

kde4_add_plugin(pgzkexireportpart2 ${pgzkexireportpart2_LIB_SRCS})

target_link_libraries(pgzkexireportpart2 ${KDE4_KDEUI_LIBS} ${QT_QTGUI_LIBRARY} ${QT_QTSCRIPT_LIBRARY}  kexidb kexicore keximain koproperty koguiutils komain kexiguiutils 
kspreadcommon kdchart ${KDE4_KROSSCORE_LIBS})

install(TARGETS pgzkexireportpart2  DESTINATION ${PLUGIN_INSTALL_DIR})
install( FILES  pgzkexireporthandler.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/kexi)
