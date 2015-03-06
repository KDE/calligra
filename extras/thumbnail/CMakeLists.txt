# the general thumbnailers, used for a lot of file types (see filters/*)
include_directories(
    ${KOMAIN_INCLUDES}
)

set(calligrathumbnail_SRCS calligracreator.cpp)
add_library(calligrathumbnail MODULE ${calligrathumbnail_SRCS})
target_link_libraries(calligrathumbnail komain)
install(TARGETS calligrathumbnail  DESTINATION ${PLUGIN_INSTALL_DIR})

if( SHOULD_BUILD_APP_FLOW OR SHOULD_BUILD_APP_KARBON )
    set(calligraimagethumbnail_SRCS calligracreator.cpp)
    add_library(calligraimagethumbnail MODULE ${calligraimagethumbnail_SRCS})
    macro_add_compile_flags(calligraimagethumbnail "-DNO_ICON_BLENDING")
    target_link_libraries(calligraimagethumbnail komain)
    install(TARGETS calligraimagethumbnail  DESTINATION ${PLUGIN_INSTALL_DIR})
endif()

# thumbnailing for the native opendocument formats
set( THUMBNAIL_SERVICES
)

# both flow and karbon load odg natively, so if any is build add support for odg
if( SHOULD_BUILD_APP_FLOW OR SHOULD_BUILD_APP_KARBON )
    list( APPEND THUMBNAIL_SERVICES calligra_odg_thumbnail.desktop )
endif()

if( SHOULD_BUILD_PART_SHEETS )
    list( APPEND THUMBNAIL_SERVICES sheets_ods_thumbnail.desktop )
endif()

if( SHOULD_BUILD_PART_STAGE )
    list( APPEND THUMBNAIL_SERVICES stage_odp_thumbnail.desktop )
endif()

if( SHOULD_BUILD_PART_WORDS )
    list( APPEND THUMBNAIL_SERVICES words_odt_thumbnail.desktop )
endif()

install( FILES ${THUMBNAIL_SERVICES} DESTINATION ${SERVICES_INSTALL_DIR})
