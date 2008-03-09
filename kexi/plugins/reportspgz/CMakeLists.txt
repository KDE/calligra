include_directories( ${CMAKE_SOURCE_DIR}/libs/main ${KOODF_INCLUDES} ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/plugins/reportspgz/backend/common ${CMAKE_SOURCE_DIR}/kexi/plugins/reportspgz/backend/renderer ${KDE4_INCLUDES} )

#this is just a basic CMakeLists.txt, for more information see the cmake manpage
add_subdirectory( backend )

#build a shared library
set(pgzkexireportpart2_LIB_SRCS 
	kexireports.cpp
	kexireportpart.cpp
	kexireportview.cpp
	kexireportpage.cpp
	kexireportdesignview.cpp
	reportentityselector.cpp
	)

kde4_add_plugin(pgzkexireportpart2 ${pgzkexireportpart2_LIB_SRCS})

target_link_libraries(pgzkexireportpart2 pgzrptcommon pgzrptrenderer pgzrptwrtembed ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} QtGui kexidb kexicore keximain koproperty koguiutils komain kexiguiutils)

install(TARGETS pgzkexireportpart2  DESTINATION ${PLUGIN_INSTALL_DIR})
install( FILES  pgzkexireporthandler.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/kexi)
