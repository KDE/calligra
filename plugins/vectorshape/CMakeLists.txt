project( vectorshape)

add_definitions(-DTRANSLATION_DOMAIN=\"calligra_shape_vector\")

include_directories(${FLAKE_INCLUDES} ${VECTORIMAGE_INCLUDES})

set ( VectorShape_SRCS
    VectorDebug.cpp
    VectorShapePlugin.cpp
    VectorShape.cpp
    VectorShapeConfigWidget.cpp
    VectorShapeFactory.cpp
    #VectorCollection.cpp
    #VectorData.cpp
    VectorTool.cpp
    VectorToolFactory.cpp
    ChangeVectorDataCommand.cpp
)

#ki18n_wrap_ui(VectorShape_SRCS
#)

add_library(calligra_shape_vector MODULE ${VectorShape_SRCS})
calligra_shape_desktop_to_json(calligra_shape_vector calligra_shape_vector.desktop)

target_link_libraries(calligra_shape_vector
    flake
    kovectorimage
    KF5::I18n
    KF5::KIOFileWidgets
    Qt5::Svg
)

install(TARGETS calligra_shape_vector DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/shapes)

########### install files ###############

if (SharedMimeInfo_FOUND)
    install(FILES calligra_svm.xml DESTINATION ${KDE_INSTALL_MIMEDIR})
    update_xdg_mimetypes(${KDE_INSTALL_MIMEDIR})
endif ()

########### subdirectories ###############

#add_subdirectory( libemf )
