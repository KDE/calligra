
include_directories( ${CMAKE_SOURCE_DIR}/karbon
${CMAKE_SOURCE_DIR}/karbon/core ${CMAKE_SOURCE_DIR}/shapes/ ${KDE4_INCLUDES} ${KOGUIUTILS_INCLUDES} )

########### next target ###############

set(karbon1ximport_PART_SRCS 
    Karbon1xImport.cpp
    ${SHAPES_SOURCES} )

kde4_add_plugin(karbon1ximport WITH_PREFIX ${karbon1ximport_PART_SRCS})

target_link_libraries(
    karbon1ximport
    koguiutils
    karboncommon
    koguiutils
    kotext
    flake
    kstore
    komain
    ${KDE4_KDECORE_LIBS}
    ${QT_QT3SUPPORT_LIBRARY}
    )

install(TARGETS karbon1ximport DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES karbon_1x_import.desktop DESTINATION ${SERVICES_INSTALL_DIR})
