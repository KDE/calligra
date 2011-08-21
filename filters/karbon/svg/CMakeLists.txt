
include_directories( ${CMAKE_SOURCE_DIR}/karbon
${CMAKE_SOURCE_DIR}/karbon/core ${CMAKE_SOURCE_DIR}/plugins ${CMAKE_SOURCE_DIR}/filters/kpresenter/svgsinglecanvas ${CMAKE_SOURCE_DIR}/filters/libsvg ${KDE4_INCLUDES}  )

########### next target ###############

#These have been added in this way temporarily, 
#since the CMakeLists of libsvg are still not worked out correctly.
set(LIBSVG_SOURCES
    ${CMAKE_SOURCE_DIR}/filters/libsvg/SvgWriter_generic.cpp
    ${CMAKE_SOURCE_DIR}/filters/libsvg/SvgParser_generic.cpp
    ${CMAKE_SOURCE_DIR}/filters/libsvg/SvgUtil.cpp
    ${CMAKE_SOURCE_DIR}/filters/libsvg/SvgGradientHelper.cpp
    ${CMAKE_SOURCE_DIR}/filters/libsvg/SvgPatternHelper.cpp
    ${CMAKE_SOURCE_DIR}/filters/libsvg/SvgFilterHelper.cpp
    ${CMAKE_SOURCE_DIR}/filters/libsvg/SvgCssHelper.cpp
    ${CMAKE_SOURCE_DIR}/filters/libsvg/SvgClipPathHelper.cpp
    ${CMAKE_SOURCE_DIR}/filters/libsvg/SvgTextHelper.cpp
    ${CMAKE_SOURCE_DIR}/filters/libsvg/SvgLoadingContext.cpp
    )

set(karbonsvgexport_PART_SRCS 
    SvgExport.cpp
    SvgWriter.cpp
    ${CMAKE_SOURCE_DIR}/filters/kpresenter/svgsinglecanvas/SvgWriter_Stage.cpp #for testing in Karbon
    ${CMAKE_SOURCE_DIR}/plugins/presentationviewportshape/PresentationViewPortShape.cpp
    ${CMAKE_SOURCE_DIR}/plugins/presentationviewportshape/PresentationViewPortShapeUtil.cpp
    ${SHAPES_SOURCES} 
    ${LIBSVG_SOURCES}
    )


kde4_add_plugin(karbonsvgexport ${karbonsvgexport_PART_SRCS})

target_link_libraries(karbonsvgexport karbonui)

install(TARGETS karbonsvgexport  DESTINATION ${PLUGIN_INSTALL_DIR})

########### next target ###############

set(karbonsvgimport_PART_SRCS 
    SvgImport.cpp
    SvgParser.cpp    
    ${CMAKE_SOURCE_DIR}/filters/kpresenter/svgsinglecanvas/SvgParser_Stage.cpp
    ${SHAPES_SOURCES} 
    ${CMAKE_SOURCE_DIR}/plugins/presentationviewportshape/PresentationViewPortShape.cpp
${CMAKE_SOURCE_DIR}/plugins/presentationviewportshape/PresentationViewPortShapeUtil.cpp
    ${LIBSVG_SOURCES}
)

kde4_add_plugin(karbonsvgimport ${karbonsvgimport_PART_SRCS})

target_link_libraries(karbonsvgimport karbonui)

install(TARGETS karbonsvgimport  DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES  karbon_svg_export.desktop karbon_svg_import.desktop karbon_svgz_import.desktop DESTINATION ${SERVICES_INSTALL_DIR})
