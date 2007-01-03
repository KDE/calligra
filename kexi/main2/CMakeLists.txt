project(kexi)

#include_directories( ${CMAKE_SOURCE_DIR}/interfaces ${KOTEXT_INCLUDES} )
#include_directories( ${KOFFICEUI_INCLUDES} ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/core ${CMAKE_SOURCE_DIR}/kexi/main ${CMAKE_SOURCE_DIR}/kexi/main/startup ${CMAKE_SOURCE_DIR}/kexi/widget ${CMAKE_SOURCE_DIR}/kexi/migration ${CMAKE_SOURCE_DIR}/libs ${CMAKE_SOURCE_DIR}/libs/kofficecore ${KDE4_INCLUDES} )
include_directories( ${KOFFICEUI_INCLUDES} ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/main2 ${CMAKE_SOURCE_DIR}/libs ${CMAKE_SOURCE_DIR}/libs/kofficecore ${KDE4_INCLUDES} )

SET (keximain_LIB_SRCS
	KexiFactory.cpp
	KexiView.cpp
	KexiDocument.cpp
	)

kde4_automoc(${keximain_LIB_SRCS})

kde4_add_library(keximain SHARED ${keximain_LIB_SRCS})

target_link_libraries(keximain
	${QT_QTSQL_LIBRARY}
	kofficecore kofficeui
	${KDE4_KDE3SUPPORT_LIBS}
	)

set_target_properties(keximain PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )
install(TARGETS keximain DESTINATION ${LIB_INSTALL_DIR})

########### next target ###############
set(kexi_KDEINIT_SRCS main.cpp )

kde4_automoc(${kexi_KDEINIT_SRCS})
kde4_add_kdeinit_executable( kexi ${kexi_KDEINIT_SRCS})
#target_link_libraries(kdeinit_kexi  ${KDE4_KDECORE_LIBS} kexicore keximain )
target_link_libraries(kdeinit_kexi  ${KDE4_KDECORE_LIBS} kofficecore keximain)
install(TARGETS kdeinit_kexi  DESTINATION ${LIB_INSTALL_DIR})
target_link_libraries( kexi kdeinit_kexi )
install(TARGETS kexi DESTINATION ${BIN_INSTALL_DIR})
#install( FILES  kexi.desktop  DESTINATION ${APPLNK_INSTALL_DIR}/Office)

########### next target ###############

SET(kexipart_PART_SRCS KexiFactory_init.cpp )
kde4_automoc(${kexipart_PART_SRCS})
kde4_add_plugin(kexipart WITH_PREFIX ${kexipart_PART_SRCS})
target_link_libraries(kexipart ${KDE4_KDECORE_LIBS} keximain)
install(TARGETS kexipart  DESTINATION ${PLUGIN_INSTALL_DIR})
