include_directories( ${CMAKE_SOURCE_DIR}/kpresenter ${CMAKE_SOURCE_DIR}/karbon/common ${CMAKE_SOURCE_DIR}/karbon/ui ${CMAKE_SOURCE_DIR}/filter/karbon/svg ${CMAKE_SOURCE_DIR}/plugins/pathshapes ${CMAKE_SOURCE_DIR}/filters/libsvg ${KDE4_INCLUDES}  )

########## next traget ################
set(LIBSVG_SOURCES
  ${CMAKE_SOURCE_DIR}/filters/libsvg/SvgWriter_generic.cpp
  ${CMAKE_SOURCE_DIR}/filters/libsvg/SvgUtil.cpp
  )

set(SHAPES_SOURCES
    ${CMAKE_SOURCE_DIR}/plugins/pathshapes/ellipse/EllipseShape.cpp
    ${CMAKE_SOURCE_DIR}/plugins/pathshapes/rectangle/RectangleShape.cpp
    ${CMAKE_SOURCE_DIR}/plugins/pathshapes/star/StarShape.cpp
    ${CMAKE_SOURCE_DIR}/plugins/artistictextshape/ArtisticTextShape.cpp
    ${CMAKE_SOURCE_DIR}/plugins/artistictextshape/ArtisticTextShapeLoadingUpdater.cpp
    ${CMAKE_SOURCE_DIR}/plugins/artistictextshape/ArtisticTextRange.cpp
    ${CMAKE_SOURCE_DIR}/plugins/presentationviewportshape/PresentationViewPortShape.cpp
    ${CMAKE_SOURCE_DIR}/plugins/presentationviewportshape/PresentationViewPortShapeUtil.cpp
)

set(svgsinglecanvasexport_PART_SRCS 
    ${CMAKE_SOURCE_DIR}/karbon/ui/KarbonPart.h
    ${LIBSVG_SOURCES}
    ${SHAPES_SOURCES}
)

kde4_add_plugin(svgsinglecanvasexport ${svgsinglecanvasexport_PART_SRCS})



target_link_libraries(svgsinglecanvasexport  ${KDE4_KDECORE_LIBS} calligrastageprivate )


install(TARGETS svgsinglecanvasexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  svg_singlecanvas_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})



