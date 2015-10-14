include_directories(
    ${KOMAIN_INCLUDES}
    ${KOTEXT_INCLUDES}
)

set(kpr2odp_PART_SRCS Filterkpr2odf.cpp )

add_library(calligra_filter_kpr2odp MODULE ${kpr2odp_PART_SRCS})

kcoreaddons_desktop_to_json(calligra_filter_kpr2odp calligra_filter_kpr2odp.desktop)

target_link_libraries(calligra_filter_kpr2odp komain)

install(TARGETS calligra_filter_kpr2odp DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})

if(SHOULD_BUILD_FILEMANAGER_THUMBNAIL)
    install( FILES  stage_kpr_thumbnail.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
endif()
