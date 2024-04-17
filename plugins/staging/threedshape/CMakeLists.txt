add_definitions(-DTRANSLATION_DOMAIN=\"calligra_shape_threed\")

include_directories( ${FLAKE_INCLUDES} )

########### Flake Plugin library ###############

set (threedshape_LIB_SRCS
    ThreedDebug.cpp
    ThreedShapePlugin.cpp
    ThreedShapeFactory.cpp
    #ThreedToolFactory.cpp
    #ThreedTool.cpp
    #ThreedShapeConfigWidget.cpp

    Object3D.cpp
    SceneObject.cpp
    Objects.cpp
    utils.cpp
)

add_library(calligra_shape_threed MODULE ${threedshape_LIB_SRCS})

target_link_libraries(calligra_shape_threed flake)
install(TARGETS calligra_shape_threed DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/shapes)
