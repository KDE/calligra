include_directories( ${CMAKE_SOURCE_DIR}/src/backend/common  ${KDE4_INCLUDES}  )

set(pgzrptrenderer_LIB_SRCS
	3of9.cpp
	code128.cpp
	codeean.cpp
	ext3of9.cpp
	orutils.cpp
	renderobjects.cpp
	orprintrender.cpp
	orprerender.cpp
	krscreenrender.cpp
	krscriptfunctions.cpp
)

kde4_add_library(pgzrptrenderer SHARED ${pgzrptrenderer_LIB_SRCS})

target_link_libraries(pgzrptrenderer pgzrptcommon kexidb ${KDE4_KDECORE_LIBS} ${KDE4_KPARTS_LIBS} kexiutils QtScript)

set_target_properties(pgzrptrenderer PROPERTIES VERSION 0.0.1 SOVERSION 0 )

install(TARGETS pgzrptrenderer  DESTINATION ${LIB_INSTALL_DIR})

