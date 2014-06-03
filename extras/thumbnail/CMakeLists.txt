# the general thumbnailers, used for a lot of file types (see filters/*)
include_directories(
    ${KOMAIN_INCLUDES}
)

set(calligrathumbnail_SRCS calligracreator.cpp)
kde4_add_plugin(calligrathumbnail ${calligrathumbnail_SRCS})
target_link_libraries(calligrathumbnail komain)
install(TARGETS calligrathumbnail  DESTINATION ${PLUGIN_INSTALL_DIR})

if( SHOULD_BUILD_KRITA OR SHOULD_BUILD_FLOW OR SHOULD_BUILD_KARBON )
    set(calligraimagethumbnail_SRCS calligracreator.cpp)
    kde4_add_plugin(calligraimagethumbnail ${calligraimagethumbnail_SRCS})
    macro_add_compile_flags(calligraimagethumbnail "-DNO_ICON_BLENDING")
    target_link_libraries(calligraimagethumbnail komain)
    install(TARGETS calligraimagethumbnail  DESTINATION ${PLUGIN_INSTALL_DIR})
endif( SHOULD_BUILD_KRITA OR SHOULD_BUILD_FLOW OR SHOULD_BUILD_KARBON )

# thumbnailing for the native opendocument formats
set( THUMBNAIL_SERVICES
)

# both flow and karbon load odg natively, so if any is build add support for odg
if( SHOULD_BUILD_FLOW OR SHOULD_BUILD_KARBON )
    set( THUMBNAIL_SERVICES ${THUMBNAIL_SERVICES} calligra_odg_thumbnail.desktop )
endif( SHOULD_BUILD_FLOW OR SHOULD_BUILD_KARBON )

if( SHOULD_BUILD_KRITA )
    set( THUMBNAIL_SERVICES ${THUMBNAIL_SERVICES} krita_kra_thumbnail.desktop )
endif( SHOULD_BUILD_KRITA )

if( SHOULD_BUILD_SHEETS_PART )
    set( THUMBNAIL_SERVICES ${THUMBNAIL_SERVICES} sheets_ods_thumbnail.desktop )
endif( SHOULD_BUILD_SHEETS_PART )

if( SHOULD_BUILD_STAGE_PART )
    set( THUMBNAIL_SERVICES ${THUMBNAIL_SERVICES} stage_odp_thumbnail.desktop )
endif( SHOULD_BUILD_STAGE_PART )

if( SHOULD_BUILD_WORDS_PART )
    set( THUMBNAIL_SERVICES ${THUMBNAIL_SERVICES} words_odt_thumbnail.desktop )
endif( SHOULD_BUILD_WORDS_PART )

install( FILES ${THUMBNAIL_SERVICES} DESTINATION ${SERVICES_INSTALL_DIR})
