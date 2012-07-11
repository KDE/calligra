# the general thumbnailers, used for a lot of file types (see filters/*)
include_directories(
    ${KOMAIN_INCLUDES}
)

set(calligrathumbnail_SRCS calligracreator.cpp)
kde4_add_plugin(calligrathumbnail ${calligrathumbnail_SRCS})
target_link_libraries(calligrathumbnail komain)
install(TARGETS calligrathumbnail  DESTINATION ${PLUGIN_INSTALL_DIR})

if( BUILD_krita OR BUILD_flow OR BUILD_karbon )
    set(calligraimagethumbnail_SRCS calligracreator.cpp)
    kde4_add_plugin(calligraimagethumbnail ${calligraimagethumbnail_SRCS})
    macro_add_compile_flags(calligraimagethumbnail "-DNO_ICON_BLENDING")
    target_link_libraries(calligraimagethumbnail komain)
    install(TARGETS calligraimagethumbnail  DESTINATION ${PLUGIN_INSTALL_DIR})
endif( BUILD_krita OR BUILD_flow OR BUILD_karbon )

# thumbnailing for the native opendocument formats
set( THUMBNAIL_SERVICES
)

# both flow and karbon load odg natively, so if any is build add support for odg
if( BUILD_flow OR BUILD_karbon )
    set( THUMBNAIL_SERVICES ${THUMBNAIL_SERVICES} calligra_odg_thumbnail.desktop )
endif( BUILD_flow OR BUILD_karbon )

if( BUILD_krita )
    set( THUMBNAIL_SERVICES ${THUMBNAIL_SERVICES} krita_kra_thumbnail.desktop )
endif( BUILD_krita )

if( BUILD_sheets )
    set( THUMBNAIL_SERVICES ${THUMBNAIL_SERVICES} sheets_ods_thumbnail.desktop )
endif( BUILD_sheets )

if( BUILD_stage )
    set( THUMBNAIL_SERVICES ${THUMBNAIL_SERVICES} stage_odp_thumbnail.desktop )
endif( BUILD_stage )

if( BUILD_words )
    set( THUMBNAIL_SERVICES ${THUMBNAIL_SERVICES} words_odt_thumbnail.desktop )
endif( BUILD_words )

install( FILES ${THUMBNAIL_SERVICES} DESTINATION ${SERVICES_INSTALL_DIR})
