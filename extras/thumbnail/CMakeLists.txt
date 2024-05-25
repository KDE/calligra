# the general thumbnailers, used for a lot of file types (see filters/*)
include_directories(
    ${KOMAIN_INCLUDES}
)

add_compile_definitions(DOCS=0 IMAGES=1)

set(calligrathumbnail_SRCS calligracreator.cpp)
add_library(calligrathumbnail MODULE ${calligrathumbnail_SRCS})
target_compile_definitions(calligrathumbnail PRIVATE TARGETTYPES=DOCS)
target_link_libraries(calligrathumbnail komain KF6::KIOWidgets)
install(TARGETS calligrathumbnail  DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf6/thumbcreator/)

if( SHOULD_BUILD_APP_KARBON )
    set(calligraimagethumbnail_SRCS calligracreator.cpp)
    add_library(calligraimagethumbnail MODULE ${calligraimagethumbnail_SRCS})
    target_compile_definitions(calligraimagethumbnail PRIVATE TARGETTYPES=IMAGES)
    target_link_libraries(calligraimagethumbnail komain KF6::KIOWidgets)
    install(TARGETS calligraimagethumbnail  DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf6/thumbcreator/)
endif()

# thumbnailing for the native opendocument formats
set( THUMBNAIL_SERVICES
)

