#make it a project so cmake spits out a codeblocks project
#project(reports)
add_subdirectory( koreport )

include_directories( ${KOMAIN_INCLUDES} 

${KOODF_INCLUDES} 
${CMAKE_SOURCE_DIR}/kexi
${CMAKE_SOURCE_DIR}/kexi/koproperty
${CMAKE_SOURCE_DIR}/kexi/core
${CMAKE_SOURCE_DIR}/kexi/widget
${CMAKE_SOURCE_DIR}/kexi/plugins/reportspgz/koreport/
${CMAKE_SOURCE_DIR}/kexi/plugins/reportspgz/koreport/common
${CMAKE_SOURCE_DIR}/kexi/plugins/reportspgz/koreport/renderer
${CMAKE_SOURCE_DIR}/kchart/kdchart/include
${KDE4_INCLUDES} )

#build a shared library
set(pgzkexireportpart2_LIB_SRCS 
	kexireports.cpp
	kexireportpart.cpp
	kexireportview.cpp
	kexireportpage.cpp
	kexireportdesignview.cpp
	kexidbreportdata.cpp
	keximigratereportdata.cpp
    kexisourceselector.cpp
	)

qt4_wrap_cpp(pgzkexireportpart2_LIB_SRCS ../scripting/kexiscripting/kexiscriptadaptor.h)

kde4_add_plugin(pgzkexireportpart2 ${pgzkexireportpart2_LIB_SRCS})

target_link_libraries(pgzkexireportpart2 koreport ${KDE4_KDEUI_LIBS} ${QT_QTGUI_LIBRARY} 
  ${QT_QTSCRIPT_LIBRARY} ${KDE4_KPARTS_LIBS} kexidb kexicore keximain ${KOPROPERTY_LIBS}  ${QT_QT3SUPPORT_LIBRARY} kexiguiutils 
    keximigrate ${KDE4_KROSSCORE_LIBS})
if (SHOULD_BUILD_KSPREAD)
  target_link_libraries(pgzkexireportpart2 kspreadcommon)
endif (SHOULD_BUILD_KSPREAD)

install(TARGETS pgzkexireportpart2  DESTINATION ${PLUGIN_INSTALL_DIR})
install( FILES  pgzkexireporthandler.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/kexi)
