
include_directories( ${KWMF_INCLUDES} ${CMAKE_SOURCE_DIR}/karbon/commands/ ${CMAKE_SOURCE_DIR}/shapes/ )

########### next target ###############

set(SHAPES_SOURCES ${CMAKE_SOURCE_DIR}/shapes/ )
set(wmfimport_PART_SRCS 
    wmfimport.cc 
    wmfimportparser.cc 
    ${SHAPES_SOURCES}/pathshapes/ellipse/KoEllipseShape.cpp 
    ${SHAPES_SOURCES}/pathshapes/rectangle/KoRectangleShape.cpp
    ${SHAPES_SOURCES}/pictureshape/PictureShape.cpp)


kde4_add_plugin(wmfimport WITH_PREFIX ${wmfimport_PART_SRCS})

target_link_libraries(wmfimport  ${KDE4_KDECORE_LIBS} kowmf karboncommon )

install(TARGETS wmfimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############


set(wmfexport_PART_SRCS wmfexport.cc )

kde4_add_plugin(wmfexport WITH_PREFIX ${wmfexport_PART_SRCS})


target_link_libraries(wmfexport  ${KDE4_KDECORE_LIBS} kowmf karboncommon )

install(TARGETS wmfexport  DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES  karbon_wmf_import.desktop karbon_wmf_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

