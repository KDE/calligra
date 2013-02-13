project( kokasten )

include_directories(${KOKASTEN_INCLUDES}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${KDE4_INCLUDES}
    ${QT_INCLUDES}
)

set( KASTEN_MAJOR_VERSION 0 )
set( KASTEN_MINOR_VERSION 2 )
set( KASTEN_PATCHLEVEL_VERSION 0 )
set( KASTEN_VERSION "${KASTEN_MAJOR_VERSION}.${KASTEN_MINOR_VERSION}.${KASTEN_PATCHLEVEL_VERSION}" )
set( KASTEN_ABI_VERSION 2 )
set( KASTEN_INCLUDE_INSTALL_DIR  ${INCLUDE_INSTALL_DIR}/kasten${KASTEN_ABI_VERSION} )

set( kastencore_LIB         kokastencore )
set( kastengui_LIB          kokastengui )
set( kastencontrollers_LIB  kokastencontrollers )
set( kastencore_LIB_NAME        kokasten${KASTEN_ABI_VERSION}core )
set( kastengui_LIB_NAME         kokasten${KASTEN_ABI_VERSION}gui )
set( kastencontrollers_LIB_NAME kokasten${KASTEN_ABI_VERSION}controllers )

set( KASTEN_LIB_SOVERSION ${KASTEN_ABI_VERSION} )
set( KASTEN_LIB_VERSION ${KASTEN_VERSION} )

add_subdirectory( core )
add_subdirectory( gui )
add_subdirectory( controllers )

add_subdirectory( includes )

#add_subdirectory( kparts )
