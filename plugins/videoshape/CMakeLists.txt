add_definitions(-DTRANSLATION_DOMAIN=\"calligra_shape_video\")

include_directories(
    ${FLAKE_INCLUDES}
    ${KOWIDGETS_INCLUDES}
)

if(PHONON_FOUND_EXPERIMENTAL)
   set(SHOULD_BUILD_THUMBNAIL TRUE)
   add_definitions( -DSHOULD_BUILD_THUMBNAIL )
else()
    set(SHOULD_BUILD_THUMBNAIL FALSE)
endif()


########### Flake Plugin library ###############

set (videoshape_LIB_SRCS
    VideoDebug.cpp
    VideoData.cpp
    VideoCollection.cpp
    VideoShape.cpp
    VideoShapeFactory.cpp
    VideoEventAction.cpp
    Plugin.cpp
    VideoShapeConfigWidget.cpp
    FullScreenPlayer.cpp
    VideoTool.cpp
    VideoToolFactory.cpp
    ChangeVideoCommand.cpp
    SelectVideoWidget.cpp
)

if(SHOULD_BUILD_THUMBNAIL)
    set(videoshape_LIB_SRCS ${videoshape_LIB_SRCS} VideoThumbnailer.cpp)
endif()

ki18n_wrap_ui(videoshape_LIB_SRCS forms/VideoToolWidget.ui)
add_library(calligra_shape_video MODULE ${videoshape_LIB_SRCS})

target_link_libraries(calligra_shape_video
    flake
    kowidgets
    KF6::KIOFileWidgets
    Phonon::phonon4qt6
)
if(PHONON_FOUND_EXPERIMENTAL)
    target_link_libraries(calligra_shape_video Phonon::phonon4qt6experimental)
endif()

install(TARGETS calligra_shape_video DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/shapes)
